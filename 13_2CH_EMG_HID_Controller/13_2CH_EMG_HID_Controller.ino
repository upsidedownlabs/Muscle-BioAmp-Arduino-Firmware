// EMG HID CONTROLLER - BioAmp EXG Pill
// https://github.com/upsidedownlabs/BioAmp-EXG-Pill
// https://github.com/upsidedownlabs/Muscle-BioAmp-Arduino-Firmware

// Upside Down Labs invests time and resources providing this open source code,
// please support Upside Down Labs and open-source hardware by purchasing
// products from Upside Down Labs!

// Copyright (c) 2021 - 2024 Upside Down Labs - contact@upsidedownlabs.tech
// Copyright (c) 2021 - 2024 Aryan Prakhar - aryanprakhar1010@gmail.com
// Copyright (c) 2026 Ankit - ankitmait64@gmail.com

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// At Upside Down Labs, we create open-source DIY neuroscience hardware and software.
// Our mission is to make neuroscience affordable and accessible for everyone.
// By supporting us with your purchase, you help spread innovation and open science.
// Thank you for being part of this journey with us!

//Keyboard library for Arduino
#include <Keyboard.h>

//Samples per second
#define SAMPLE_RATE 500

// Make sure to set the same baud rate on your Serial Monitor/Plotter
#define BAUD_RATE 115200


#define INPUT_PIN1 A0
#define INPUT_PIN2 A2

// Envelope buffer size
// High value -> smooth but less responsive
// Low value -> not smooth but responsive
#define BUFFER_SIZE 64

// Debounce time after a double-flex (both hands) toggle, in ms
#define DOUBLE_FLEX_DEBOUNCE_MS 300

// Key mappings
// Change these keys according  to your game
const char MODE_1_KEY = 'w';     // Key pressed for Forward mode
const char MODE_2_KEY = 's';     // Key pressed for Reverse mode
const char LEFT_ARM_KEY = 'a';   // Key pressed for left steering
const char RIGHT_ARM_KEY = 'd';  // Key pressed for right steering

// Tracks whether each keyboard key is currently held down
bool rightHeld = false;
bool leftHeld = false;
bool wHeld = false;
bool sHeld = false;

// Define different constant for different envelope
// Channel 1 variables
int circular_buffer1[BUFFER_SIZE];
int data_index1 = 0, sum1 = 0;
// Channel 2 variables
int circular_buffer2[BUFFER_SIZE];
int data_index2 = 0, sum2 = 0;

// Calibrate threshold values
// Uncomment the below line to view EMG envelope on serial plotter
// #define Calibrate


int threshold1 = 40;
int threshold2 = 60;

bool prevBothFlex = false;     // Stores previous loop's "both flexed" state
unsigned long comboCount = 0;  // count of double-flex
int currentMode = -1;          // -1 = rest, no key pressed until first double flex

// Tracks when the last double-flex toggle happened, for debounce
unsigned long lastComboTime = 0;

void setup() {
  //Initialize serial communication
  Serial.begin(BAUD_RATE);
  Keyboard.begin();

  // Start in idle mode with no movement keys pressed.
  // First accepted double-flex switches to forward mode ('w').
  pinMode(INPUT_PIN1, INPUT);
  pinMode(INPUT_PIN2, INPUT);

  // Start in idle mode with no movement keys pressed.
  // First accepted double-flex switches to forward mode ('w').
  wHeld = false;
  sHeld = false;
}

void loop() {

  //Calculate elapsed time
  static unsigned long past = 0;
  unsigned long present = micros();
  unsigned long interval = present - past;
  past = present;

  //Run timer
  static long timer = 0;
  timer -= interval;

  //Sample and get envelope
  if (timer < 0) {
    timer += 1000000 / SAMPLE_RATE;

    //Raw EMG values CH1
    int sensor_value1 = analogRead(INPUT_PIN1);

    //Filtered EMG values CH1
    int signal1 = EMGFilter1(sensor_value1);

    //EMG envelope CH1
    int envelope1 = getEnvelope1(abs(signal1));

    //Raw EMG values CH2
    int sensor_value2 = analogRead(INPUT_PIN2);

    //Filtered EMG CH2
    int signal2 = EMGFilter2(sensor_value2);

    //EMG envelope CH2
    int envelope2 = getEnvelope2(abs(signal2));

// If set to calibrate show envelope data on serial monitor/plotter
#ifdef Calibrate

    Serial.print(envelope1);
    Serial.print('\t');
    Serial.println(envelope2);

#else
    //Channel 1 flex
    bool flex1 = envelope1 > threshold1;

    //Channel 2 flex
    bool flex2 = envelope2 > threshold2;

    //both flex
    bool bothFlex = flex1 && flex2;

    // Detect rising edge of double flex -> toggle Forward/Reverse mode
    // Only accept a new toggle if at least DOUBLE_FLEX_DEBOUNCE_MS
    // has passed since the previous accepted toggle.
    unsigned long now = millis();
    if (bothFlex && !prevBothFlex && (now - lastComboTime) >= DOUBLE_FLEX_DEBOUNCE_MS) {

      lastComboTime = now;  // start debounce window
      comboCount++;

      if (comboCount % 2 == 1) {
        // GO MODE (1st, 3rd, 5th... flex -> forward / 'w')
        currentMode = 0;
        if (sHeld) {
          Keyboard.release(MODE_2_KEY);
          sHeld = false;
        }
        if (!wHeld) {
          Keyboard.press(MODE_1_KEY);
          wHeld = true;
        }
      } else {
        // BRAKE/REVERSE MODE (2nd, 4th... flex -> 's')
        currentMode = 3;
        if (wHeld) {
          Keyboard.release(MODE_1_KEY);
          wHeld = false;
        }
        if (!sHeld) {
          Keyboard.press(MODE_2_KEY);
          sHeld = true;
        }
      }
    }
    prevBothFlex = bothFlex;
    bool steeringCooldown = (now - lastComboTime) < DOUBLE_FLEX_DEBOUNCE_MS;

    // STEERING - only a single key (a or d) pressed at a time
    if (!steeringCooldown && flex1 && !flex2) {
      // RIGHT
      if (!rightHeld) {
        Keyboard.press(RIGHT_ARM_KEY);  //change as per your game control
        rightHeld = true;
      }
      if (leftHeld) {
        Keyboard.release(LEFT_ARM_KEY);
        leftHeld = false;
      }
    } else if (!steeringCooldown && flex2 && !flex1) {
      // LEFT
      if (!leftHeld) {
        Keyboard.press(LEFT_ARM_KEY);  //change as per your game control
        leftHeld = true;
      }
      if (rightHeld) {
        Keyboard.release(RIGHT_ARM_KEY);
        rightHeld = false;
      }
    } else {
      // STRAIGHT - release both steering keys
      if (rightHeld) {
        Keyboard.release(RIGHT_ARM_KEY);
        rightHeld = false;
      }
      if (leftHeld) {
        Keyboard.release(LEFT_ARM_KEY);
        leftHeld = false;
      }
    }
#endif
  }
}

// Envelope detection algorithm
// Get CH1 envelope
int getEnvelope1(int abs_emg) {
  sum1 -= circular_buffer1[data_index1];
  sum1 += abs_emg;
  circular_buffer1[data_index1] = abs_emg;
  data_index1 = (data_index1 + 1) % BUFFER_SIZE;
  return (sum1 / BUFFER_SIZE) * 2;
}

// Get CH2 envelope
int getEnvelope2(int abs_emg) {
  sum2 -= circular_buffer2[data_index2];
  sum2 += abs_emg;
  circular_buffer2[data_index2] = abs_emg;
  data_index2 = (data_index2 + 1) % BUFFER_SIZE;
  return (sum2 / BUFFER_SIZE) * 2;
}

// Band-Pass Butterworth IIR digital filter, generated using filter_gen.py.
// Sampling rate: 500.0 Hz, frequency: [74.5, 149.5] Hz.
// Filter is order 4, implemented as second-order sections (biquads).
// Reference:
// https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.butter.html
// https://courses.ideate.cmu.edu/16-223/f2020/Arduino/FilterDemos/filter_gen.py

// CH1 EMG filter
float EMGFilter1(float input) {
  float output = input;
  {
    static float z1, z2;  //filter section state
    float x = output - 0.05159732 * z1 - 0.36347401 * z2;
    output = 0.01856301 * x + 0.03712602 * z1 + 0.01856301 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  //filter section state
    float x = output - -0.53945795 * z1 - 0.39764934 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  //filter section state
    float x = output - 0.47319594 * z1 - 0.70744137 * z2;
    output = 1.00000000 * x + 2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  //filter section state
    float x = output - -1.00211112 * z1 - 0.74520226 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}

// CH2 EMG filter
float EMGFilter2(float input2) {
  float output = input2;
  {
    static float z1, z2;  //filter section state
    float x = output - 0.05159732 * z1 - 0.36347401 * z2;
    output = 0.01856301 * x + 0.03712602 * z1 + 0.01856301 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  //filter section state
    float x = output - -0.53945795 * z1 - 0.39764934 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  //filter section state
    float x = output - 0.47319594 * z1 - 0.70744137 * z2;
    output = 1.00000000 * x + 2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  //filter section state
    float x = output - -1.00211112 * z1 - 0.74520226 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}