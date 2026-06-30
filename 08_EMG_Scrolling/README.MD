# EMG Scroll Controller – Scroll Through Pages With Your Muscles 🦾🖱️

Control page scrolling (Up/Down navigation) using **Electromyography (EMG)** signals from your forearms! This project turns muscle flexes into keyboard scroll inputs using the **BioAmp EXG Pill** and **Muscle BioAmp Shield**, letting you scroll up and down through documents, web pages, or slides with nothing but your muscles.

- Flex your **right forearm** → Scroll Up (`Up Arrow`)
- Flex your **left forearm** → Scroll Down (`Down Arrow`)

---

## Table of Contents

- [How It Works](#how-it-works)
- [Hardware Required](#hardware-required)
- [Software Required](#software-required)
- [Hardware Setup](#hardware-setup)
- [Skin Preparation & Electrode Placement](#skin-preparation--electrode-placement)
- [Uploading the Arduino Code](#uploading-the-arduino-code)
- [Running the Python Script](#running-the-python-script)
- [Calibration](#calibration)
- [Controls](#controls)
- [Troubleshooting](#troubleshooting)

---

## How It Works

This project builds a **2-channel EMG acquisition system**:

- **Channel 1 (A0)** records EMG from the **right forearm**
- **Channel 2 (A2)** records EMG from the **left forearm**

The Arduino samples both channels at 500 Hz, filters out noise , and computes a smoothed **envelope** of muscle activity using a circular buffer (moving average).

The Arduino streams a simple result over **Serial** (`"1"`, `"2"`, or `"0"`) to a connected computer:

| Serial Output | Meaning |
|---|---|
| `1` | Right forearm envelope crossed its threshold and is greater than the left forearm envelope |
| `2` | Left forearm envelope crossed its threshold and is greater than the right forearm envelope |
| `0` | Neither channel is dominant / no qualifying flex |

A companion **Python script** (`EMG_Scroll.py`) listens to this serial stream and uses `pyautogui` to emulate the **Up** and **Down** arrow keys on your computer, scrolling whatever window is currently in focus.

> ⚠️ **Important:** The Arduino only starts sending serial data once you **press the red SW1 button on the Muscle BioAmp Shield**. This button toggles an internal "transaction" state (`ledState`) - until it's pressed, the board stays idle and the Python script will not receive any `1`/`2`/`0` values, so scrolling will not work. Press it once before each session to start, and press it again if you want to pause/stop data transmission.

---

## Hardware Required

This project uses the **DIY Neuroscience Kit Pro** by Upside Down Labs:

- BioAmp EXG Pill (Assembled)
- Arduino Uno R4 Minima
- Muscle BioAmp Shield
- BioAmp Cable (100cm) x 2
- STEMMA Cable (JST PH 2.0mm to 3 female jumpers)
- Gel Electrodes (6 total: 3 per channel - positive, negative, reference)
- NuPrep Skin Preparation Gel
- Alcohol/Wet Wipes
- USB Cable (for Arduino)

> 🛒 Available from: [Upside Down Labs Store](https://store.upsidedownlabs.tech/product/diy-neuroscience-kit-pro/), [Amazon India](https://www.amazon.in/Neuroscience-2025-Record-Signals-Projects/dp/B0G1LYG675/ref=sr_1_4?crid=2P4ZFMER5IXPK&dib=eyJ2IjoiMSJ9.Si8rYQUPOYfCp5IsHt0XAti_TAZ6_kxfN5ZA9-ZWKJQ6EWltKL7POaBOmzELt9r2sVgYldSpr5NuiDzTak29wIXsFQ9222gP-T5AIeQ0X-w3Egi3hRCQHE0Gl0gSC9Xeu7YhCiuy9a3x325l-82jzZjTRGrPZIOb2qF29rGTdMA.0LRojMOZFgaA3zhX2WPD9JbqTezKc8EUhy1mr_Qb6HE&dib_tag=se&keywords=diy+neuroscience+kit+pro&qid=1782799087&sprefix=diy+neur%2Caps%2C303&sr=8-4), [Robu India](https://robu.in/product/diy-neuroscience-kit-pro/), [Tindie](https://www.tindie.com/products/upsidedownlabs/diy-neuroscience-kit-pro-2025-edition/), or [DigiKey](https://www.digikey.in/en/products/detail/upside-down-labs/0049/19525709).


## Software Required

- [Arduino IDE](https://www.arduino.cc/en/software)
- Arduino UNO R4 board package:
  `Tools → Board → Boards Manager → Search "Arduino UNO R4 Boards" → Install`
- [Python 3](https://www.python.org/downloads/) (3.8+ recommended)
- Python packages:
  - `pyserial`
  - `pyautogui`

Install the Python dependencies with:

\`\`\`bash
pip install pyserial pyautogui
\`\`\`

### Project Files
- `08_EMG_Scrolling.ino` – Arduino firmware (EMG acquisition + envelope detection)
- `EMG_Scroll.py` – Python script that reads serial data and emulates Up/Down key presses
- `DetectComPort.py` – Helper script that auto-detects the correct COM port the Arduino is connected to

> 📁 Make sure `EMG_Scroll.py` and `DetectComPort.py` are in the **same folder**, since `EMG_Scroll.py` imports `find_responsive_port` from `DetectComPort.py`.

---

## Hardware Setup

### Step 1: Stack the Shield
Stack the **Muscle BioAmp Shield** on top of the **Arduino Uno R4 Minima**, aligning the pin headers properly.

### Step 2: Connect the BioAmp EXG Pill
Connect the **BioAmp EXG Pill** to the **A2 port** of the Muscle BioAmp Shield using the **STEMMA cable** (JST PH 2.0mm connector on one end, 3 female jumpers on the other).

### Step 3: Connect Electrode Cables
- Connect one **BioAmp cable** to the **BioAmp EXG Pill**.
- Connect another **BioAmp cable** to the **Muscle BioAmp Shield**.
- Insert both cable ends into their respective **JST PH connectors**.

---

## Skin Preparation & Electrode Placement

This is a **2-channel system**:
- **Channel 1** → recorded from the **right forearm**
- **Channel 2** → recorded from the **left forearm**

### Skin Prep
1. Apply **NuPrep Skin Preparation Gel** to the area where electrodes will be placed (removes dead skin and dirt).
2. Rub thoroughly, then wipe clean with an alcohol wipe or wet wipe.

### Electrode Placement

| Channel | Positive (+) Electrode | Negative (−) Electrode | Reference Electrode |
|---|---|---|---|
| CH1 | Right Forearm | Right Forearm | Behind the Right Palm |
| CH2 | Left Forearm | Left Forearm | Behind the Left Palm |

---

## Uploading the Arduino Code

1. Copy the `08_EMG_Scrolling.ino` code into the Arduino IDE.
2. Install the **Arduino UNO R4 board package**:
   `Tools → Board → Boards Manager → Search "Arduino UNO R4 Boards" → Install latest`
   - Accept any installation/permission prompts.
3. Connect the Arduino to your laptop via USB.
4. Select the board: `Tools → Board → Arduino UNO R4 Minima` (or your board).
5. Select the correct port: `Tools → Port` (disconnect/reconnect the Arduino if the port isn't listed).
6. Click **Upload** (→) and wait for it to complete.

> ⚠️ **Important:** For best signal quality, **unplug your laptop charger** and sit **at least 5 meters away from AC appliances** during use - this minimizes electrical interference (50/60 Hz noise) on the EMG signal.

---

## Running the Python Script

1. Make sure the Arduino is uploaded with the firmware and connected via USB.
2. From a terminal, navigate to the project folder (where `EMG_Scroll.py` and `DetectComPort.py` live) and run:

`python EMG_Scroll.py`
3. The script automatically detects the correct COM port via `DetectComPort.py` (it scans all available serial ports and sends a test handshake to find the Arduino).
> 🔴 **Critical step:** , **press the red SW1 button on the Muscle BioAmp Shield** to start the serial transaction. The onboard status LED (pin 8) will turn on, confirming the board is now actively sending `1`/`2`/`0` values over serial. If you skip this step, the Python script will appear to do nothing.
5. Once connected, click into any scrollable window (browser, PDF reader, document, slides, etc.) and start flexing:
   - **Right forearm** → scrolls **Up**
   - **Left forearm** → scrolls **Down**

---


## Calibration

Muscle strength varies between people, so threshold values need tuning.

1. In the Arduino code, find:

   `// #define Calibrate`
2. Uncomment it:
  
   `#define Calibrate`
3. Upload the modified code.
4. Open the **Serial Monitor**in the Arduino IDE:`Tools → Serial Moniter` .
5. Flex your **right forearm** several times at full strength - note the **maximum peak value** shown for envelope1.
6. Flex your **left forearm** several times - note its **maximum peak value** for envelope2.
7. Calculate new thresholds as **60–70% of each peak value**.
8. Update the threshold values in code:

   `int threshold1 = 70;   // Right forearm channel (A0)`

   `int threshold2 = 50;   // Left forearm channel (A2)`
9. Re-comment the calibration line:
   
   `// #define Calibrate`
10. Upload the updated code, press the **red SWD button** on the shield again, and test with the Python script. Repeat calibration if detection is still unreliable.

---

## Controls

| Gesture | Serial Signal | Key Emulated | Function |
|---|---|---|---|
| Flex right forearm | `1` | `Up Arrow` | Scroll Up |
| Flex left forearm | `2` | `Down Arrow` | Scroll Down |
| Neither flexed | `0` | - | No action |

---


## Troubleshooting

| Issue | Possible Fix |
|---|---|
| Python script does nothing / no scrolling | **Press the red SW1 button on the Muscle BioAmp Shield** to start the serial transaction - this is the most common cause |
| `find_responsive_port()` returns `None` / script can't find Arduino | Check USB connection, confirm Arduino is uploaded with the firmware, close Arduino Serial Monitor (it locks the port), re-run the script |
| Keys not registering at all | Confirm `pyserial` and `pyautogui` are installed; check that `EMG_Scroll.py` and `DetectComPort.py` are in the same folder |
| Noisy / inconsistent flex detection | Re-do skin prep; check electrode placement and contact; sit away from AC appliances/chargers. See the [Document](https://docs.upsidedownlabs.tech/guides/troubleshoot/tips/index.html) for more tips |
| Thresholds always triggering or never triggering | Re-run calibration (see Calibration section) and adjust `threshold1`/`threshold2` |
| Scrolling too fast / too sensitive | Increase the `latency` value in `EMG_Scroll.py` |
| Scrolling feels laggy / unresponsive | Decrease the `latency` value in `EMG_Scroll.py`, or decrease `BUFFER_SIZE` in the Arduino code for faster envelope response (trade-off: less smoothing) |
| Scrolling feels jittery/noisy | Increase `BUFFER_SIZE` in the Arduino code for more smoothing (trade-off: less responsive) |