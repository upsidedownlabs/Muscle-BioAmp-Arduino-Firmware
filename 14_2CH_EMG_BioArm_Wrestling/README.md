# BioArm Wrestling – Arm Wrestle with Your Muscles

Turn real muscle activity into a head-to-head arm wrestling match! This project uses **Electromyography (EMG)** signals from two players' forearms to drive a servo-controlled arm, live-visualized in a browser dashboard. Whoever flexes harder wins the round.

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

BioArm Wrestling is a fun two-player game where you compete using your muscle activity instead of physical strength. Each player wears a Muscle BioAmp Patchy sensor on their forearm, which detects their muscle contractions.

At the start of the match, each player has a different objective:

- **Player 1:** Flex your forearm muscles to **close the servo claw**.
- **Player 2:** Flex your forearm muscles to **open the servo claw**.

As both players compete, the servo moves toward the player producing stronger muscle activity. The first player to move the servo completely to their end position wins the round.

Throughout the match, a live web interface displays both players' muscle activity and the overall match progress, making it easy for players and spectators to follow the action in real time.

---

## Features

- **Real-time EMG visualization** waveform traces for both players
- **3-second countdown** before every round, so both players start fair and ready
- **Automatic reconnect** if the USB cable is pulled mid-match. The website detects it, resets cleanly, and reconnects the moment the board is plugged back in
- **Editable player names**, rematch and new-opponent flows, and a clean winner overlay

---

## Hardware Required

This project uses the **Muscle BioAmp Patchy** by Upside Down Labs:

- 2× Muscle BioAmp Patchy (one per player) with its cables
- Arduino Uno R4 Minima along with its shield
- 1× Servo motor
- Gel electrodes (3 per channel: positive, negative, reference)
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

<p align="center">
  <img src="full-setup.png" alt="Hardware Setup" width="80%">
</p>

### Step 1: Assemble the hardware

Mount the **Arduino Shield** onto the **Arduino UNO R4 Minima**, ensuring that all the header pins are properly aligned and firmly seated.

### Step 2: Connect the Muscle BioAmp Patchy sensors

Each **Muscle BioAmp Patchy** comes with a dedicated connection cable included in the kit. Connect the cable to the Patchy, then plug the other end into the corresponding channel on the Arduino Shield.

- **Player 1's Patchy** → **Channel 1 (A0)**
- **Player 2's Patchy** → **Channel 2 (A1)**

Ensure both Patchy boards are securely connected before proceeding.

### Step 3: Connect the servo motor

Connect the servo motor to the **`D2`** digital pin on the Arduino Shield. Make sure the connector is oriented correctly, with the signal, power, and ground pins aligned as marked on the shield.

### Step 4: Connect the Arduino

Connect the Arduino UNO R4 Minima to your computer using a USB cable. This powers the board and provides the serial connection used by the web interface to communicate with the game.

---

## Skin Preparation & Electrode Placement

This is a **2-channel system**:
- **Channel 1** → **Player 1**'s forearm
- **Channel 2** → **Player 2**'s forearm

### Skin Prep
Clean the skin with an alcohol swab to remove oils and dead skin cells. Allow it to dry completely before applying the electrodes to ensure good electrical contact.
Refer [this guide](https://docs.upsidedownlabs.tech/guides/usage-guides/skin-preparation/index.html) for proper skin preparation.

### Electrode Placement

The muscle bioAmp Patchy is designed to directly plug into adjacent electrodes, stick the Patchy directly over the forearm muscle you intend to contract and connect the reference electrode on the bony region of your elbow using the snap cable provided on the cable of Muscle BioAmp Patchy. Refer [this documentation](https://docs.upsidedownlabs.tech/hardware/bioamp/muscle-bioamp-patchy/index.html#step-4-electrode-placements) for proper electrode placement using Muscle BioAmp Patchy.

Good skin contact is the single biggest factor in clean EMG signal, spend the extra minute here.

---

## Uploading the Code

1. On the home page of this repository, click the green **`<> Code`** button.
2. From the dropdown menu, select **`Download ZIP`** to download the repository as a ZIP file.
3. Extract the downloaded ZIP file to any location on your computer.
4. Open the extracted repository folder.
5. Navigate to the `14_2CH_EMG_BioArm_Wrestling` folder.
6. Open the `14_2CH_EMG_BioArm_Wrestling.ino` sketch in the Arduino IDE.
7. Connect your Arduino board to your computer using a USB cable.
8. In the Arduino IDE, select your board from **`Tools → Board → Arduino UNO R4 Minima`**.
9. Select the correct serial port from **`Tools → Port`**. If the port is not listed, disconnect and reconnect the board, then check again.
10. Click **Upload** and wait for the upload to complete.

> **Important:** For best signal quality, **unplug your laptop charger** and sit **at least 5 meters away from AC appliances** while playing. This minimizes electrical interference (50/60 Hz noise) on the EMG signal.

---

## Launching the Web App

1. Open the extracted repository folder and navigate to the `14_2CH_EMG_BioArm_Wrestling` folder.
2. Open the `index.html` file in a desktop browser that supports Web Serial (Google Chrome or Microsoft Edge).
3. Click **Connect to Arduino** and select the correct serial port.
4. Once connected, the status indicator will turn green, and the application will be ready to start a match.

> Web Serial requires a secure context or a local file, and does not work on mobile browsers.

---

## Playing a Match

1. Click **Connect to Arduino** and select the correct serial port
2. Enter both player names
3. Click **Start Duel**, a 3-second countdown begins
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
| Web Serial not supported | Use Chrome or Edge on desktop or check whether your browser supports web serial interface |

---

## To Do

Design and 3D print a custom indicator that mounts onto the servo horn to make the winner easier to visualize. As the servo rotates from **0° to 180°**, the indicator moves toward the player with stronger EMG activity, clearly showing the match progress.