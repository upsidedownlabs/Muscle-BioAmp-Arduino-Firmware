# EMG HID Controller – Play Racing Games with Your Muscles 🦾🎮

Control a car racing game using **Electromyography (EMG)** signals from your forearms! This project turns muscle flexes into keyboard inputs (`W`, `A`, `S`, `D`) using the **BioAmp EXG Pill** and **Muscle BioAmp Shield**, letting you steer and switch driving modes with nothing but your muscles.

- Flex your **right forearm** → steer right (`D`)
- Flex your **left forearm** → steer left (`A`)
- Flex **both forearms together** → toggle between Forward (`W`) and Reverse (`S`) mode

---

## Table of Contents

- [How It Works](#how-it-works)
- [Hardware Required](#hardware-required)
- [Software Required](#software-required)
- [Hardware Setup](#hardware-setup)
- [Skin Preparation & Electrode Placement](#skin-preparation--electrode-placement)
- [Uploading the Code](#uploading-the-code)
- [Calibration](#calibration)
- [Controls](#controls)
- [Customizing Key Mappings](#customizing-key-mappings)
- [Troubleshooting](#troubleshooting)


---

## How It Works

This project builds a **2-channel EMG acquisition system**:

- **Channel 1 (A0)** records EMG from the **right forearm**
- **Channel 2 (A2)** records EMG from the **left forearm**

The Arduino samples both channels at 500 Hz, remove noise, then computes a smoothed **envelope** of muscle activity using a circular buffer (moving average).

If the envelope crosses a calibrated **threshold**, that muscle is considered "flexed." Based on which muscle(s) are flexed, the Arduino acts as a **USB HID keyboard** (via the `Keyboard` library) and presses/releases keys accordingly:

| Gesture | Action |
|---|---|
| Right forearm flexed only | Press `D` (steer right) |
| Left forearm flexed only | Press `A` (steer left) |
| Neither flexed | Release `A` and `D` |
| Both flexed simultaneously (**double flex**) | Toggle driving mode: 1st double flex → hold `W` (Forward); 2nd double flex → release `W`, hold `S` (Reverse); and so on, alternating |

A **debounce window (300 ms)** prevents a single prolonged double-flex from being registered as multiple mode toggles.

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

> 🛒 Available from: [Upside Down Labs Store](https://store.upsidedownlabs.tech/product/diy-neuroscience-kit-pro/), Amazon India, [Robu India](https://robu.in/product/diy-neuroscience-kit-pro/), [Tindie](https://www.tindie.com/products/upsidedownlabs/diy-neuroscience-kit-pro-2025-edition/), or [DigiKey](https://www.digikey.in/en/products/detail/upside-down-labs/0049/19525709).

## Software Required

- [Arduino IDE](https://www.arduino.cc/en/software)
- `Keyboard` library (built into Arduino IDE for boards with native USB HID support)
- Arduino UNO R4 board package (if using UNO R4 Minima):
  `Tools → Board → Boards Manager → Search "Arduino UNO R4 Boards" → Install`

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

## Uploading the Code

1. Copy the code from this repository.
2. Open the **Arduino IDE**.
3. Install the **Arduino UNO R4 board package** (if applicable):
   `Tools → Board → Boards Manager → Search "Arduino UNO R4 Boards" → Install latest`
   - Accept any installation/permission prompts.
4. Create a new sketch: `File → New Sketch`.
5. Select all the default code (`Ctrl+A`) and paste in the project code (`Ctrl+V`).
6. Connect the Arduino to your laptop via USB.
7. Select the board: `Tools → Board → Arduino UNO R4 Minima` (or your board).
8. Select the correct port: `Tools → Port` (disconnect/reconnect the Arduino if the port isn't listed).
9. Click **Upload** (→) and wait for it to complete.

> ⚠️ **Important:** For best signal quality, **unplug your laptop charger** and sit **at least 5 meters away from AC appliances** during use - this minimizes electrical interference (50/60 Hz noise) on the EMG signal.

### Quick Functional Test
Open any text editor (e.g., Notepad) and try:
- Flex **right forearm** → `d` is typed once
- Flex **left forearm** → `a` is typed once
- Flex **both arms** (1st time) → `w` types continuously
- Flex **both arms** (2nd time) → `s` types continuously

If detection feels off, proceed to calibration below.

---

## Calibration

Muscle strength varies between people, so threshold values need tuning.

1. In the code, find:
   ```cpp
   // #define Calibrate
   ```
2. Uncomment it:
   ```cpp
   #define Calibrate
   ```
3. Upload the modified code.
4. Open the **Serial Monitor** in the Arduino IDE.
5. Flex your **left forearm** several times at full strength - note the **maximum peak value** shown.
6. Flex your **right forearm** several times - note its **maximum peak value**.
7. Calculate new thresholds as **60–70% of each peak value**.
8. Update the threshold values in code:
   ```cpp
   int threshold1 = 40;   // Right forearm channel (A0)
   int threshold2 = 60;   // Left forearm channel (A2)
   ```
9. Re-comment the calibration line:
   ```cpp
   // #define Calibrate
   ```
10. Upload the updated code and test again. Repeat calibration if detection is still unreliable.

---

## Controls

| Gesture | Key Pressed | Function |
|---|---|---|
| Flex right forearm | `D` | Steer Right |
| Flex left forearm | `A` | Steer Left |
| Double flex (1st) | `W` (held) | Forward Mode |
| Double flex (2nd) | `S` (held) | Reverse Mode |
| Double flex (3rd, 5th, ...) | `W` (held) | Back to Forward Mode |
| Double flex (4th, 6th, ...) | `S` (held) | Back to Reverse Mode |

Driving mode keeps toggling between Forward and Reverse with every subsequent double flex.

---

## Customizing Key Mappings

You can remap the keys to match any game's control scheme by editing these lines near the top of the code:

```cpp
const char MODE_1_KEY = 'w';      // Forward
const char MODE_2_KEY = 's';      // Reverse
const char LEFT_ARM_KEY    = 'a'; // Steer Left
const char RIGHT_ARM_KEY   = 'd'; // Steer Right
```
---

## Troubleshooting

| Issue | Possible Fix |
|---|---|
| Keys not registering at all | Check board/port selection; confirm `Keyboard.begin()` ran (re-upload); ensure board supports native USB HID (e.g., Leonardo, Micro, UNO R4) |
| Noisy / inconsistent flex detection | Re-do skin prep; check electrode placement and contact; sit away from AC appliances/chargers |
| Thresholds always triggering or never triggering | Re-run calibration (Step: Calibration) and adjust `threshold1`/`threshold2` |
| Mode toggles multiple times per flex | Increase `DOUBLE_FLEX_DEBOUNCE_MS` |
| Steering feels laggy | Decrease `BUFFER_SIZE` for faster response (trade-off: less smoothing) |
| Steering feels jittery/noisy | Increase `BUFFER_SIZE` for more smoothing (trade-off: less responsive) |

