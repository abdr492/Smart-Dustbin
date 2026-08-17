/*
 * Smart Dustbin Firmware
 * Features: Ultrasonic hand detection, bin level monitoring, servo lid control, status LEDs, buzzer, LCD display.
 */

#include <Arduino.h>               // Include standard Arduino framework
#include <NewPing.h>               // Include NewPing library for ultrasonic sensors
#include <ESP32Servo.h>            // Include ESP32Servo library for servo control
#include <Wire.h>                  // Include Wire library for I2C communication
#include <LiquidCrystal_I2C.h>     // Include LiquidCrystal_I2C library for LCD

// Pin Definitions
#define TRIG1 23                   // Define Trigger pin for Hand Sensor
#define ECHO1 22                   // Define Echo pin for Hand Sensor
#define TRIG2 21                   // Wait, SDA/SCL and TRIG2/ECHO1 conflict in diagram, but user asked for TRIG2=21, ECHO2=19, LCD_SDA=21, LCD_SCL=22. 
// We will follow user instructions exactly.
#define ECHO2 19                   // Define Echo pin for Bin Sensor
#define SERVO_PIN 18               // Define PWM pin for Servo Motor
#define BUZZER 5                   // Define pin for Active Buzzer
#define LED_GREEN 4                // Define pin for Green LED
#define LED_RED 2                  // Define pin for Red LED

// Constraints and Thresholds
#define MAX_DISTANCE 400           // Define maximum distance we want to ping for (in cm)
#define HAND_THRESHOLD 15          // Define distance threshold for hand detection (in cm)
#define BIN_HEIGHT 30              // Define total height of the bin (in cm)
#define EMPTY_THRESHOLD 25         // Define threshold for Empty status (percentage)
#define QUARTER_THRESHOLD 50       // Define threshold for Quarter full status (percentage)
#define HALF_THRESHOLD 75          // Define threshold for Half full status (percentage)
#define THREE_QUARTER_THRESHOLD 90 // Define threshold for 3/4 full status (percentage)

// Global Objects
NewPing handSensor(TRIG1, ECHO1, MAX_DISTANCE); // Initialize NewPing object for Hand Sensor
NewPing binSensor(TRIG2, ECHO2, MAX_DISTANCE);  // Initialize NewPing object for Bin Sensor
Servo lidServo;                                 // Initialize Servo object for the Lid
LiquidCrystal_I2C lcd(0x27, 16, 2);             // Initialize LiquidCrystal_I2C object (address 0x27, 16 cols, 2 rows)

// State Variables
bool isLidOpen = false;            // Track if the lid is currently open
unsigned long lidOpenTime = 0;     // Track the time when the lid was opened
const unsigned long OPEN_DURATION = 5000; // Define how long the lid stays open (in ms)
int fillPercentage = 0;            // Store the current bin fill percentage
unsigned long lastSerialTime = 0;  // Track time for periodic serial output
bool simulationMode = false;       // Track if simulation mode is active

// Function Prototypes
int measureDistance(NewPing& sensor); // Prototype for measuring distance
void calculateFillLevel(int distance); // Prototype for calculating fill level
void controlLid(bool openLid);     // Prototype for controlling the servo
void activateAlert();              // Prototype for activating buzzer and red LED
void processSerialCommands();      // Prototype for reading Serial commands

void setup() {                     // Setup function runs once on startup
  Serial.begin(9600);              // Initialize Serial communication at 9600 baud
  while (!Serial) {;}              // Wait for serial port to connect
  
  pinMode(BUZZER, OUTPUT);         // Set buzzer pin as output
  pinMode(LED_GREEN, OUTPUT);      // Set green LED pin as output
  pinMode(LED_RED, OUTPUT);        // Set red LED pin as output
  
  digitalWrite(BUZZER, LOW);       // Ensure buzzer is off initially
  digitalWrite(LED_GREEN, HIGH);   // Turn on green LED (normal status)
  digitalWrite(LED_RED, LOW);      // Ensure red LED is off initially
  
  lidServo.attach(SERVO_PIN);      // Attach the servo to the defined pin
  controlLid(false);               // Ensure the lid starts closed
  
  Wire.begin(21, 22);              // Initialize I2C with SDA=21, SCL=22
  lcd.init();                      // Initialize the LCD
  lcd.backlight();                 // Turn on the LCD backlight
  lcd.setCursor(0, 0);             // Set cursor to column 0, line 0
  lcd.print("Smart Dustbin");      // Print welcome message
  lcd.setCursor(0, 1);             // Set cursor to column 0, line 1
  lcd.print("Initializing...");    // Print initialization status
  delay(2000);                     // Wait 2 seconds
  lcd.clear();                     // Clear the LCD display
  
  Serial.println("System Ready."); // Print ready message to Serial
  Serial.println("Send 'SIM' to enable simulation mode."); // Prompt for simulation mode
}                                  // End of setup function

void loop() {                      // Loop function runs continuously
  processSerialCommands();         // Check for and process any incoming serial commands
  
  unsigned long currentMillis = millis(); // Get current time in milliseconds
  
  int handDist = 0;                // Variable to store hand distance
  int binDist = 0;                 // Variable to store bin waste distance
  
  if (simulationMode) {            // Check if simulation mode is active
    // In simulation mode, use random or mocked values
    handDist = random(5, 50);      // Mock hand distance between 5 and 50 cm
    binDist = random(5, 30);       // Mock bin distance between 5 and 30 cm
  } else {                         // If not in simulation mode
    handDist = measureDistance(handSensor); // Measure actual hand distance
    binDist = measureDistance(binSensor);   // Measure actual bin distance
  }                                // End if-else block
  
  calculateFillLevel(binDist);     // Calculate fill percentage based on distance
  
  if (fillPercentage > THREE_QUARTER_THRESHOLD) { // Check if bin is full
    digitalWrite(LED_GREEN, LOW);  // Turn off green LED
    digitalWrite(LED_RED, HIGH);   // Turn on red LED
    activateAlert();               // Sound the buzzer alert
  } else {                         // If bin is not full
    digitalWrite(LED_RED, LOW);    // Turn off red LED
    digitalWrite(LED_GREEN, HIGH); // Turn on green LED
    digitalWrite(BUZZER, LOW);     // Ensure buzzer is off
  }                                // End if-else block
  
  if (handDist > 0 && handDist < HAND_THRESHOLD && fillPercentage <= THREE_QUARTER_THRESHOLD) { // Check for hand presence and bin not full
    if (!isLidOpen) {              // If lid is not already open
      controlLid(true);            // Open the lid
      lidOpenTime = millis();      // Record the time the lid was opened
    }                              // End inner if block
  }                                // End outer if block
  
  if (isLidOpen && (currentMillis - lidOpenTime >= OPEN_DURATION)) { // Check if time to close lid
    controlLid(false);             // Close the lid
  }                                // End if block
  
  if (currentMillis - lastSerialTime >= 500) { // Check if 500ms have passed for reporting
    lastSerialTime = currentMillis; // Update the last report time
    
    Serial.print("hand_distance:"); // Print hand distance label
    Serial.print(handDist);        // Print hand distance value
    Serial.print("cm, waste_distance:"); // Print waste distance label
    Serial.print(binDist);         // Print waste distance value
    Serial.print("cm, fill_percentage:"); // Print fill percentage label
    Serial.print(fillPercentage);  // Print fill percentage value
    Serial.print("%, lid_status:");// Print lid status label
    Serial.println(isLidOpen ? "OPEN" : "CLOSED"); // Print open/closed state and newline
    
    lcd.setCursor(0, 0);           // Set cursor to top line
    lcd.print("Level: ");          // Print label
    lcd.print(fillPercentage);     // Print fill percentage
    lcd.print("%   ");             // Print spaces to clear old chars
    lcd.setCursor(0, 1);           // Set cursor to bottom line
    lcd.print(isLidOpen ? "Lid: OPEN  " : "Lid: CLOSED"); // Print lid status
  }                                // End if block
  
  delay(50);                       // Short delay to prevent loop running too fast
}                                  // End of loop function

int measureDistance(NewPing& sensor) { // Function to measure distance using ultrasonic sensor
  unsigned int uS = sensor.ping_median(5); // Take 5 readings and get median ping time in microseconds
  int cm = uS / US_ROUNDTRIP_CM;   // Convert microseconds to centimeters
  if (cm == 0) cm = MAX_DISTANCE;  // If 0, assume out of range (max distance)
  return cm;                       // Return the calculated distance
}                                  // End of measureDistance function

void calculateFillLevel(int distance) { // Function to calculate the bin's fill percentage
  if (distance >= BIN_HEIGHT) {    // If distance is greater than or equal to bin height
    fillPercentage = 0;            // Bin is completely empty
  } else if (distance <= 0) {      // If distance is 0 or negative
    fillPercentage = 100;          // Bin is completely full
  } else {                         // Otherwise
    int fillAmount = BIN_HEIGHT - distance; // Calculate how much of the bin is filled
    fillPercentage = (fillAmount * 100) / BIN_HEIGHT; // Convert to percentage
  }                                // End if-else block
}                                  // End of calculateFillLevel function

void controlLid(bool openLid) {    // Function to handle servo movement
  if (openLid && !isLidOpen) {     // If requested to open and not currently open
    lidServo.write(90);            // Move servo to 90 degrees (open position)
    isLidOpen = true;              // Update state variable
  } else if (!openLid && isLidOpen){ // If requested to close and currently open
    lidServo.write(0);             // Move servo to 0 degrees (closed position)
    isLidOpen = false;             // Update state variable
  }                                // End if-else block
}                                  // End of controlLid function

void activateAlert() {             // Function to sound buzzer for full bin
  static unsigned long lastBeep = 0; // Static variable to track beep timing
  static int beepCount = 0;        // Static variable to track number of beeps
  
  if (millis() - lastBeep > 500) { // Check if 500ms passed since last beep cycle
    if (beepCount < 3) {           // Limit to 3 beeps
      digitalWrite(BUZZER, HIGH);  // Turn buzzer on
      delay(100);                  // Wait 100ms
      digitalWrite(BUZZER, LOW);   // Turn buzzer off
      beepCount++;                 // Increment beep counter
    }                              // End inner if
    lastBeep = millis();           // Update last beep time
  }                                // End outer if
  
  // Note: For continuous full alert, you might reset beepCount when level drops
  // Or handle it outside this function. We'll leave it as a simple 3-beep logic
}                                  // End of activateAlert function

void processSerialCommands() {     // Function to handle commands sent via Serial Monitor
  if (Serial.available() > 0) {    // Check if there is data available to read
    String command = Serial.readStringUntil('\n'); // Read string until newline character
    command.trim();                // Remove leading/trailing whitespace
    
    if (command == "OPEN") {       // Check for OPEN command
      controlLid(true);            // Open the lid
      lidOpenTime = millis();      // Reset lid open timer
      Serial.println("Command: Lid Opened"); // Confirm command
    } else if (command == "CLOSE"){// Check for CLOSE command
      controlLid(false);           // Close the lid
      Serial.println("Command: Lid Closed"); // Confirm command
    } else if (command == "STATUS"){// Check for STATUS command
      Serial.print("System Status - Level: "); // Print status prefix
      Serial.print(fillPercentage); // Print level
      Serial.println("%");         // Print percentage sign
    } else if (command == "RESET"){// Check for RESET command
      fillPercentage = 0;          // Reset fill percentage
      controlLid(false);           // Close the lid
      digitalWrite(LED_RED, LOW);  // Turn off red LED
      digitalWrite(LED_GREEN, HIGH); // Turn on green LED
      Serial.println("Command: System Reset"); // Confirm command
    } else if (command == "SIM") { // Check for simulation toggle command
      simulationMode = !simulationMode; // Toggle simulation state
      Serial.print("Simulation Mode: "); // Print mode prefix
      Serial.println(simulationMode ? "ON" : "OFF"); // Print current mode state
    }                              // End if-else block
  }                                // End if block
}                                  // End of processSerialCommands function

