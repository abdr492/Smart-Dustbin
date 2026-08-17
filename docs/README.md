# 🗑️ Smart Dustbin — Industry-Oriented Embedded System

![Arduino](https://img.shields.io/badge/Arduino-ESP32-00979D?style=for-the-badge&logo=arduino)
![Platform](https://img.shields.io/badge/Platform-Wokwi%20%7C%20Hardware-blue?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge)

> An automated IoT waste management system using ESP32, dual HC-SR04 ultrasonic sensors, servo motor, and a real-time web dashboard. Industry-relevant proof-of-work project with virtual Wokwi simulation.

---

## 🎯 Features

- ✅ **Automatic lid opening** via hand/object proximity detection (HC-SR04 Sensor 1)
- ✅ **Real-time bin level monitoring** with fill percentage calculation (HC-SR04 Sensor 2)
- ✅ **5 fill states**: Empty → Quarter → Half → Three-Quarters → FULL
- ✅ **Buzzer + Red LED alert** when bin exceeds 90% capacity
- ✅ **Auto lid close** after 5 seconds of no hand detected
- ✅ **Serial commands**: OPEN, CLOSE, STATUS, RESET
- ✅ **LCD display** for real-time status (optional)
- ✅ **Premium web dashboard** with animated gauges and charts
- ✅ **Wokwi virtual simulation** — no hardware required
- ✅ **GitHub Actions CI** for automated code validation

---

## 🔧 Hardware Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ESP32 DevKit V1 | 1 | Main microcontroller (WiFi-capable) |
| HC-SR04 Ultrasonic Sensor | 2 | Hand detection + bin level measurement |
| SG90 Servo Motor | 1 | Automatic lid open/close |
| Active Buzzer | 1 | Full-bin alert sound |
| LED (Green) | 1 | Normal status indicator |
| LED (Red) | 1 | Full-bin alert indicator |
| LCD 16x2 I2C | 1 | Status display (optional) |
| 220Ω Resistors | 2 | LED current limiting |
| Breadboard + Jumper Wires | — | Circuit connections |
| 5V Power Supply | 1 | System power |

---

## 📌 Pin Configuration

| Component | ESP32 Pin | Description |
|-----------|-----------|-------------|
| HC-SR04 #1 TRIG | GPIO 23 | Hand sensor trigger |
| HC-SR04 #1 ECHO | GPIO 22 | Hand sensor echo |
| HC-SR04 #2 TRIG | GPIO 21 | Bin level trigger |
| HC-SR04 #2 ECHO | GPIO 19 | Bin level echo |
| Servo Motor | GPIO 18 | PWM lid control |
| Buzzer | GPIO 5 | Alert buzzer |
| Green LED | GPIO 4 | Normal status |
| Red LED | GPIO 2 | Alert status |
| LCD SDA | GPIO 21 | I2C data |
| LCD SCL | GPIO 22 | I2C clock |

---

## 🚀 Quick Start

### Option A — Virtual Simulation (No Hardware Needed)
1. Go to [https://wokwi.com/projects/new](https://wokwi.com/projects/new)
2. Select **ESP32 DevKit V1**
3. Replace diagram.json with simulation/diagram.json
4. Replace sketch.ino with simulation/sketch.ino
5. Click ▶️ **Start Simulation**
6. Drag the HC-SR04 distance sliders to simulate hand and bin levels

### Option B — Arduino IDE (Real Hardware)
1. Install Arduino IDE 2.x
2. Add ESP32 board: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
3. Install libraries: NewPing, ESP32Servo, LiquidCrystal_I2C
4. Open irmware/src/main.ino
5. Select board: **ESP32 Dev Module**
6. Upload and open Serial Monitor at 9600 baud

### Option C — PlatformIO
`ash
cd firmware
pio run --target upload
pio device monitor
`

---

## 🌐 Web Dashboard
Open dashboard/index.html directly in your browser — no server needed.
- Simulated sensor data updates every 2 seconds
- Animated circular fill gauge with color transitions
- Real-time fill history chart (Chart.js)
- Manual lid controls

---

## 📁 Folder Structure
`
Smart-Dustbin/
├── .github/workflows/arduino-ci.yml   # GitHub Actions CI
├── firmware/
│   ├── src/main.ino                   # ESP32 Arduino firmware
│   └── platformio.ini                 # PlatformIO config
├── simulation/
│   ├── diagram.json                   # Wokwi circuit diagram
│   └── sketch.ino                     # Wokwi-compatible firmware
├── dashboard/
│   ├── index.html                     # Premium IoT dashboard
│   └── style.css                      # Stylesheet
├── circuit/                           # Circuit diagram images
└── docs/
    ├── README.md                      # This file
    └── interview_prep.md              # Interview Q&A guide
`

---

## 🏙️ Industry Applications
- Smart city municipal waste management
- Hospital hygiene stations (touchless)
- Shopping mall public restrooms
- Airport terminal waste bins
- Corporate office buildings
- Industrial facility waste tracking

---

## 📸 Screenshots Checklist (for GitHub)
- [ ] Wokwi simulation running (servo rotating)
- [ ] Serial Monitor output showing sensor readings
- [ ] Web dashboard with fill gauge animated
- [ ] Circuit breadboard photo (if real hardware)
- [ ] LED indicators (green/red) in action

---

## 🛠️ Tech Stack
| Technology | Version | Use |
|------------|---------|-----|
| Arduino Framework | 2.x | Firmware base |
| ESP32 SDK | 2.0.x | Microcontroller |
| NewPing Library | 1.9.7 | Ultrasonic sensors |
| ESP32Servo | 0.13.0 | Servo PWM control |
| LiquidCrystal_I2C | 1.1.4 | LCD display |
| Chart.js | Latest | Dashboard charts |
| Wokwi | Cloud | Virtual simulation |

---

## 📝 License
MIT License — free for personal and educational use.

## 👤 Author
Embedded Systems Project — Industry-Oriented Proof of Work
