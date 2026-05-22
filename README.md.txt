# 🚗 AI-Based Driver Drowsiness Detection System

An intelligent real-time driver monitoring system that detects drowsiness, yawning, blink rate, and head tilt using Computer Vision and Artificial Intelligence.

---

# 📌 Project Overview

This project detects driver fatigue in real time using a webcam and MediaPipe Face Mesh. The system monitors:

- Eye closure
- Yawning
- Blink rate
- Head tilt

When fatigue is detected, Python sends signal to Arduino UNO, which controls a vibration motor through L298N motor driver.

---

# ✨ Features

- Real-time driver monitoring
- Eye closure detection using EAR
- Yawning detection using MAR
- Blink rate monitoring
- Head pose detection
- Arduino integration
- Real-time hardware alert

---

# 🧠 Technologies Used

## Software
- Python
- OpenCV
- MediaPipe
- NumPy
- PySerial

## Hardware
- Arduino UNO
- L298N Motor Driver
- Vibration Motor
- Webcam

---

# ⚙️ System Architecture

Webcam → OpenCV → MediaPipe → EAR/MAR Calculation → Fatigue Detection → Arduino → Motor Driver → Motor

---

# 👁️ Eye Aspect Ratio (EAR)

The system calculates Eye Aspect Ratio using eye landmarks.

- Eyes Open → EAR High
- Eyes Closed → EAR Low

---

# 😮 Mouth Aspect Ratio (MAR)

The system detects yawning using mouth landmarks.

- Mouth Open → MAR High
- Mouth Closed → MAR Low

---

# 📷 Output

The system displays:
- EAR value
- MAR value
- Blink count
- Driver status
- Fatigue alerts

---

# 🚨 Detection Conditions

Fatigue detected when:
- Eyes remain closed
- Yawning detected
- Head tilt detected

---

# 🔌 Hardware Used

- Arduino UNO
- L298N Motor Driver
- Vibration Motor
- Webcam
- Jumper Wires

---

# ▶️ Installation

Install dependencies:

```bash
pip install -r requirements.txt