# BioArm Wrestling – Arm Wrestle with Your Muscles

Turn real muscle activity into a head-to-head arm wrestling match! This project uses **Electromyography (EMG)** signals from two players' forearms to drive a servo-controlled arm, live-visualized in a browser dashboard. Whoever flexes harder wins the round.

- Flex your forearm harder than your opponent → the arm tips toward your side
- Push the arm all the way to your win angle → you take the round
- A 3-second countdown, live waveform traces, and win-proximity meters keep every match fair and fun to watch

---

## Table of Contents

- [How It Works](#how-it-works)
- [Features](#features)
- [Hardware Required](#hardware-required)
- [Software Required](#software-required)
- [Hardware Setup](#hardware-setup)
- [Skin Preparation & Electrode Placement](#skin-preparation--electrode-placement)
- [Uploading the Code](#uploading-the-code)
- [Launching the Web App](#launching-the-web-app)
- [Playing a Match](#playing-a-match)
- [Troubleshooting](#troubleshooting)

---

## How It Works

This project builds a **2-channel EMG acquisition system**, one channel per player:

- **Channel 1 (A0)** records EMG from **Player 1**'s forearm → closes the claw
- **Channel 2 (A1)** records EMG from **Player 2**'s forearm → opens the claw

The Arduino samples both channels at 500 Hz, runs each signal through a bandpass Butterworth filter (74.5–149.5 Hz) tuned for EMG, then computes a smoothed **envelope** of muscle activity using a circular buffer (moving average).

The difference between the two envelopes nudges a servo-driven arm toward whichever player is squeezing harder. Push the arm past your win angle first, and you win the round. A connected web app streams this telemetry live over USB and renders it as scrolling waveforms, win-proximity meters, and a full match flow, no software installation required beyond a browser that supports Web Serial.

---

## Features

- **Real-time EMG visualization** — live scrolling waveform traces for both players
- **3-second countdown** before every round, so both players start fair and ready
- **Automatic reconnect** — if the USB cable is pulled mid-match, the website detects it, resets cleanly, and reconnects the moment the board is plugged back in
- **Editable player names**, rematch and new-opponent flows, and a clean winner overlay

---

## Hardware Required

This project uses the **Muscle BioAmp Patchy** by Upside Down Labs:

- 2× Muscle BioAmp Patchy (one per player) with its cables
- Arduino Uno R4 Minima (or compatible board)
- 1× Servo motor
- Gel electrodes (3 per channel — positive, negative, reference)
- Alcohol / wet wipes
- USB cable (for Arduino)

> 🛒 Available from the [Upside Down Labs Store](https://store.upsidedownlabs.tech/) and other UDL resellers (Amazon, Robu, Tindie, DigiKey).

## Software Required

- [Arduino IDE](https://www.arduino.cc/en/software)
- Arduino UNO R4 board package:
  `Tools → Board → Boards Manager → Search "Arduino UNO R4 Boards" → Install`
- A desktop browser with [Web Serial](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API) support (Chrome or Edge)

---

## Hardware Setup

### Step 1: Wire up the Patchy boards
Connect each Muscle BioAmp Patchy to the cable provided with it, and connect the supplied snap cable to female-to-female jumper wires. Then connect those cables to respective channels:
- Player 1's Patchy → `A0`
- Player 2's Patchy → `A1`

### Step 2: Attach the servo
Connect the servo signal wire to pin `2`, and power/ground to the Arduino's 5V and GND rails.

### Step 3: Power up
Plug the Arduino into your computer via USB. This both powers the board and provides the serial link the web app uses.

---

## Skin Preparation & Electrode Placement

This is a **2-channel system**:
- **Channel 1** → **Player 1**'s forearm
- **Channel 2** → **Player 2**'s forearm

### Skin Prep
Clean the skin with an alcohol swab to remove oils and dead skin cells. Allow it to dry completely before applying the electrodes to ensure good electrical contact.

### Electrode Placement

The muscle bioAmp Patchy is designed to directly plug into adjacent electrodes, stick the Patchy directly over the forearm muscle you intend to contract and connect the reference electrode on the bony region of your elbow using the snap cable provided on the cable of Muscle BioAmp Patchy.

Good skin contact is the single biggest factor in clean EMG signal — spend the extra minute here.

---

## Uploading the Code

1. Copy the firmware sketch `(.ino)` from this repository.
2. Open the **Arduino IDE**.
3. Install the **Arduino UNO R4 board package**:
   `Tools → Board → Boards Manager → Search "Arduino UNO R4 Boards" → Install latest`
   - Accept any installation/permission prompts.
4. Create a new sketch: `File → New Sketch`.
5. Select all the default code and paste in the project firmware.
6. Connect the Arduino to your computer via USB.
7. Select the board: `Tools → Board → Arduino UNO R4 Minima` (or your board).
8. Select the correct port: `Tools → Port` (disconnect/reconnect the Arduino if the port isn't listed).
9. Click **Upload** (→) and wait for it to complete.

> **Important:** For best signal quality, **unplug your laptop charger** and sit **at least 5 meters away from AC appliances** while playing. This minimizes electrical interference (50/60 Hz noise) on the EMG signal.

---

## Launching the Web App

1. Download the project's `index.html` file and open it in a desktop browser that supports Web Serial (Chrome or Edge).
2. Click **Connect to Arduino** and select the correct serial port.
3. Once connected, the status pill will turn green and the app will wait for a match to start.

> Web Serial requires a secure context or a local file, and does not work on mobile browsers.

---

## Playing a Match

1. Click **Connect to Arduino** and select the correct serial port
2. Enter both player names
3. Click **Start Duel** — a 3-second countdown begins
4. Flex your forearm muscles. The waveform traces and win meter update in real time.
5. First player to push the arm to their win angle takes the round
6. Hit **Rematch** to go again, or **New Opponents** to reset names and start fresh

---


## Troubleshooting

| Issue | Possible Fix |
|---|---|
| Web app won't connect | Confirm you're using Chrome or Edge on desktop; check the correct serial port is selected |
| No `READY` message on Serial Monitor | Re-upload the firmware; check the board selection under `Tools → Board`. Unplug and plug the board once and check if any other application is using the serial port |
| Noisy / inconsistent EMG readings | Re-do skin prep; check electrode placement and contact; sit away from AC appliances and chargers. See the [UDL troubleshooting guide](https://docs.upsidedownlabs.tech/guides/troubleshoot/tips/index.html) for more tips |
| USB unplugged mid-match | The app auto-detects this, resets the UI, and reconnects automatically once the board is plugged back in |
| Web Serial not supported | Web Serial does not work on mobile browsers or older Firefox/Safari — use Chrome or Edge on desktop |

---