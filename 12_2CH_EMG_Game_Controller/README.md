# EMG-Based Game Controller

This project transforms your muscle activity into keyboard keystrokes, enabling gesture-based control for gaming or other applications. By combining Arduino and Python, you can create a custom, gesture-controlled setup. Follow this guide to set up, test, and customize your EMG-based game controller.

---

## Table of Contents
1. [Setup](#setup)
2. [Uploading the Code](#uploading-the-code)
3. [Testing & Playing](#testing--playing)
4. [Customizing Keystrokes](#customizing-keystrokes)
5. [Tips for Best Results](#tips-for-best-results)

---

## Setup

Before you begin, ensure you have the following:
- **Hardware**: Arduino board, EMG sensor, connecting cables, and electrodes.
- **Software**: 
  - [Arduino IDE](https://www.arduino.cc/en/software)
  - [Python](https://www.python.org/downloads)
  - Necessary Python modules (detailed in the Python script or README in the repository).

---

## Uploading the Code

1. **Connect Your Arduino**:
   - Use a USB cable to connect the Arduino to your laptop.

2. **Download the Code**:
   - Visit the [GitHub repository](#) and download the provided files.
   - Open the `12_2CH_EMG_Game_Controller.ino` file in Arduino IDE.

3. **Configure Arduino IDE**:
   - Select the correct **COM port** and **board** under the **Tools** menu.
   - If you’re unsure about these settings, refer to the quick guide in the repository.

4. **Upload the Code**:
   - Click the **Upload** button. A progress bar will indicate the upload status.

5. **Optimize Signal Quality**:
   - Sit at least 5 meters away from AC appliances or electronic devices to avoid interference.
   - Unplug your laptop charger to reduce electrical noise.

---

## Testing & Playing

1. **Test with the Serial Monitor**:
   - Open the Serial Monitor in Arduino IDE.
   - Set the baud rate to **115200**.
   - Observe real-time outputs:
     - `0`: No muscle activity detected, no keystroke triggered.
     - `1`: Bend your hand to the left, triggering the **Left Arrow** key.
     - `2`: Bend your hand to the right, triggering the **Right Arrow** key.
     - `3`: Flex your fingers, triggering the **Up Arrow** key.
     - `4`: Flex your fingers while bending your hand to the left, triggering the **Left Arrow** key and **Up Arrow** key.
     - `5`: Flex your fingers while bending your hand to the right, triggering the **Right Arrow** key and **Up Arrow** key.

2. **Run the Python Script**:
   - Open the `12_2CH_EMG_Game_Controller.py` script in [Visual Studio Code](https://code.visualstudio.com/) or any Python IDE.
   - Ensure all required Python modules are installed (commands available in the repository README).
   - Run the script. It will automatically detect your Arduino's COM port.
   - If the port isn’t detected, manually edit the COM port in the script:
     ```python
     ser = serial.Serial("COM_PORT_HERE", 115200)
     ```

3. **Observe Keystroke Mapping**:
   - The terminal will display mapped keystrokes based on your gestures.

---

## Customizing Keystrokes

1. Open the `12_2CH_EMG_Game_Controller.py` script in a text editor.
2. Locate lines like:
   ```python
   keyboard.press('left')
   ```
3. Replace `'left'`, `'right'`, or `'up'` with your desired keys. For example:
   - Replace `'left'` with `'A'` for racing games.
   - Replace `'right'` with `'D'` for movement in RPGs.

4. Save your changes and rerun the script.

---

## Tips for Best Results

- **Positioning**: Ensure the EMG sensors are securely attached to your muscles.
- **Avoid Interference**: Keep a distance from electronics or AC devices.
- **Adjust Settings**: Modify thresholds or keystroke timings in the code to suit your muscle activity and gameplay.

---

This Game Controller transforms your muscle movements into gaming actions, enhancing both accessibility and interactivity. Customize it for various games and enjoy your new EMG-based game controller!
