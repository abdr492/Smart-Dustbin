/*
 * Smart Dustbin - Wokwi ESP32 Arduino v3 (ZERO LIBRARIES)
 * Uses NEW ESP32 LEDC API: ledcAttach() + ledcWrite(pin, duty)
 */

// ── PINS ─────────────────────────────────────────────────────
#define TRIG1      12
#define ECHO1      14
#define TRIG2      27
#define ECHO2      26
#define SERVO_PIN  18
#define BUZZER      5
#define LED_GREEN   4
#define LED_RED     2

// ── SERVO (ESP32 Arduino v3 LEDC API) ────────────────────────
#define LEDC_FREQ   50
#define LEDC_BITS   16
#define SERVO_0    1638    // 0 deg  = 1ms pulse
#define SERVO_90   4915    // 90 deg = 1.5ms pulse

void servoBegin() {
  ledcAttach(SERVO_PIN, LEDC_FREQ, LEDC_BITS);  // v3 API
}

void servoWrite(int angle) {
  int duty = map(angle, 0, 180, SERVO_0, 8192);
  ledcWrite(SERVO_PIN, duty);                    // v3: pin, not channel
}

// ── SETTINGS ─────────────────────────────────────────────────
#define HAND_THRESHOLD  15
#define BIN_HEIGHT      30
#define FULL_THRESHOLD  90

// ── STATE ────────────────────────────────────────────────────
bool          lidOpen     = false;
unsigned long lidOpenedAt = 0;
unsigned long lastPrint   = 0;

// ── HC-SR04 read (pulseIn, no library) ───────────────────────
long readCm(int trig, int echo) {
  digitalWrite(trig, LOW);  delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long d = pulseIn(echo, HIGH, 25000UL);
  return d ? d * 0.034 / 2 : 999;
}

// ── SETUP ────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  pinMode(TRIG1,     OUTPUT);
  pinMode(ECHO1,     INPUT);
  pinMode(TRIG2,     OUTPUT);
  pinMode(ECHO2,     INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED,   OUTPUT);
  pinMode(BUZZER,    OUTPUT);

  servoBegin();
  servoWrite(0);

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED,   LOW);
  digitalWrite(BUZZER,    LOW);

  Serial.println("=== Smart Dustbin Ready ===");
  Serial.println("sensor_hand < 15cm  => lid opens");
  Serial.println("sensor_bin  < 3cm   => FULL alert");
}

// ── LOOP ─────────────────────────────────────────────────────
void loop() {
  long handDist = readCm(TRIG1, ECHO1);
  long binDist  = readCm(TRIG2, ECHO2);

  if (binDist > BIN_HEIGHT || binDist == 999) binDist = BIN_HEIGHT;

  int fillPct = constrain(((BIN_HEIGHT - binDist) * 100) / BIN_HEIGHT, 0, 100);

  // Hand → open lid
  bool handSeen = (handDist > 1 && handDist < HAND_THRESHOLD);
  if (handSeen && !lidOpen) {
    lidOpen = true; lidOpenedAt = millis();
    servoWrite(90);
    Serial.println(">> HAND DETECTED - Lid OPEN");
  }

  // Auto close after 5s
  if (lidOpen && !handSeen && millis() - lidOpenedAt > 5000) {
    lidOpen = false;
    servoWrite(0);
    Serial.println(">> Lid CLOSED");
  }

  // Full bin alert
  if (fillPct >= FULL_THRESHOLD) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED,   HIGH);
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER, HIGH); delay(150);
      digitalWrite(BUZZER, LOW);  delay(150);
    }
  } else {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED,   LOW);
    digitalWrite(BUZZER,    LOW);
  }

  // Serial log
  if (millis() - lastPrint > 600) {
    lastPrint = millis();
    Serial.print("Hand:"); Serial.print(handDist);
    Serial.print("cm  Bin:"); Serial.print(binDist);
    Serial.print("cm  Fill:"); Serial.print(fillPct);
    Serial.print("%  Lid:"); Serial.print(lidOpen ? "OPEN" : "CLOSED");
    if      (fillPct < 25) Serial.println("  [EMPTY]");
    else if (fillPct < 50) Serial.println("  [QUARTER]");
    else if (fillPct < 75) Serial.println("  [HALF]");
    else if (fillPct < 90) Serial.println("  [3/4 FULL]");
    else                   Serial.println("  [** FULL - ALERT **]");
  }

  delay(100);
}
