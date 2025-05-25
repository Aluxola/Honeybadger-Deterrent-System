# 🐧 Penguin Predator Detection, Deterrent & Identification System

This repository contains the full implementation of a smart, modular system designed to assist in the **preservation of African penguin colonies** by detecting, identifying, and deterring predators—particularly honey badgers. The system combines:

-  **Machine learning models** for predator identification,
-  **PIR-based motion detection** with event logging,
-  **Auditory and visual deterrent systems**, and
-  Circuit schematics and deployment instructions for replication or extension.

---

## Project Goals

This system was developed as part of the **EEE4113F Engineering Design** course at the University of Cape Town. It aims to:

- Detect motion in protected zones around penguin colonies,
- Trigger deterrents when predators are detected,
- Log data in real-time to Google Sheets for offline analysis,
- Capture images via ESP32-CAM for identification,
- Use ML-based classification for predator recognition.

---

##  System Components

### 1. Detection Subsystem (Motion Trigger)
- Dual PIR sensors monitor **Zone A** and **Zone B**
- Motion is confirmed using a timestamp-based filter
- Data is logged to Google Sheets via a webhook
- ESP32-CAM captures an image during each detection

### 2. Deterrent Subsystem
- Four LEDs (random flashing patterns)
- Buzzer emits varying tones
- Runs for 15 seconds per confirmed detection
- Triggered every **second detection** to conserve power

### 3. Identification Subsystem (ML Model)
- Python-based classifier trained on sample predator images
- Receives captured images (future implementation: local or cloud)
- Designed to distinguish between penguins and target threats

---

## 🔧 Hardware Requirements

| Component        | Description                            |
|------------------|----------------------------------------|
| ESP32-WROOM      | Main microcontroller (motion + deterrent) |
| ESP32-CAM        | Image capture module                   |
| 2x PIR Sensors   | Motion detection (Zone A & B)          |
| 4x LEDs          | Visual deterrent                       |
| Buzzer           | Auditory deterrent                     |
| 10kΩ Resistors   | Pull-downs for PIR stability           |
| Power Supply     | Stable 5V source or regulated battery  |
| Plexiglass       | Used for weatherproof housing          |

---

## 📁 Repository Structure

