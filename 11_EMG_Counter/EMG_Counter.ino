// EMG_Counter - Muscle Bioamp Patchy
// https://github.com/upsidedownlabs/Muscle-BioAmp-Arduino-Firmware

// Upside Down Labs invests time and resources providing this open source code,
// please support Upside Down Labs and open-source hardware by purchasing
// products from Upside Down Labs!

// Copyright (c) 2021 - 2024 Upside Down Labs - contact[at]upsidedownlabs.tech
// Copyright (c) 2021 - 2024 Aryan Prakhar - aryanprakhar1010[at]gmail.com

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

// Samples per second
#define SAMPLE_RATE 500

// Make sure to set the same baud rate on your Serial Monitor/Plotter
#define BAUD_RATE 115200

// Change if not using A0 analog pin
#define INPUT_PIN A0

// envelopeee buffer size
// High value -> smooth but less responsive
// Low value -> not smooth but responsive
#define BUFFER_SIZE 64

// EMG Threshold value, different for each user
// Check by plotting EMG envelopee data on Serial plotter
#define EMG_THRESHOLD 80

// Button to start the counter
#define BUTTON_PIN 2
bool buttonPressed = false;

// Servo toggle flag
int count = 0;
// Last gesture timestamp
unsigned long lastGestureTime = 0;
// Delay between two actions
unsigned long gestureDelay = 500;
int circular_buffer[BUFFER_SIZE];
int data_index, sum;

// Define the buzzer pin
#define BUZZER_PIN 8  // Change this to the actual pin you're using

// Buzzer timer variables
unsigned long buzzerTimer = 0;
const unsigned long BUZZER_INTERVAL = 1000000; // 1 second in microseconds
bool buzzerState = false;

// Buzzer tone frequency (in Hz)
const int BUZZER_FREQUENCY = 2000; // Adjust this value to change the tone
const int BUZZER_DURATION = 100; // Duration of the beep in milliseconds

// Timer for program termination
unsigned long programStartTime;
const unsigned long PROGRAM_DURATION = 60000000; // 1 minute in microseconds

// LED pins
const int LED_PINS[] = {8, 9, 10, 11, 12, 13};
const int NUM_LEDS = 6;

bool countedThisCycle = false;

// Calibrate threshold values
// Uncomment the below line to view EMG envelope on serial plotter
// #define Calibrate

void setup() {
  // Your existing setup code
  Serial.begin(BAUD_RATE);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use INPUT_PULLUP to enable the internal pull-up resistor
   programStartTime = micros(); // Record the start time
    for (int i = 0; i < NUM_LEDS; i++){
        pinMode(LED_PINS[i], OUTPUT);
        }
    }

  
void loop() {
  // Debouncing logic for the button
  static unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
  static int lastButtonState = HIGH;           // the previous reading from the input pin
  int buttonState;                             // the current reading from the input pin
  const unsigned long debounceDelay = 50;      // the debounce time; increase if the output flickers

  // Read the state of the button
  buttonState = digitalRead(BUTTON_PIN);

  // Check if the button state has changed
  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  // If the button state has not changed for the debounce delay, consider it stable
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState == LOW) { // Button is pressed (assuming active low with pull-up)
      buttonPressed = true;
    }
  }

  lastButtonState = buttonState;

  // Only run the main program logic if the button has been pressed
  if (buttonPressed) {
    unsigned long currentTime = micros();
    #ifndef Calibrate
    // Check if 1 minute has elapsed
    if (currentTime - programStartTime >= PROGRAM_DURATION) {
      // Program duration reached, print final count and halt
      displayLEDs(count, PROGRAM_DURATION);
      Serial.print("Final count: ");
      Serial.println(count);
      Serial.print("Program duration (microseconds): ");
      Serial.println(PROGRAM_DURATION);
      while(1) {} // Infinite loop to halt the program
    }
    #endif

    // Calculate elapsed time
    static unsigned long past = 0;
    unsigned long present = micros();
    unsigned long interval = present - past;
    past = present;

    // Run timer
    static long timer = 0;
    timer -= interval;

    #ifndef Calibrate
    // Buzzer timer
    buzzerTimer += interval;
    #endif

    // Sample and get envelope
    if(timer < 0) {
      timer += 1000000 / SAMPLE_RATE;

      // RAW EMG Values
      int sensor_value = analogRead(INPUT_PIN);

      // Filtered EMG
      int signal = EMGFilter(sensor_value);

      // EMG envelope
      int envelope = getEnvelope(abs(signal));
      // If set to calibrate show envelope data on serial monitor/plotter
      #ifdef Calibrate
      Serial.println(envelope);
      // If not set to calibrate do serial communication
      #else
      if(envelope > EMG_THRESHOLD && !countedThisCycle) {
        if((millis() - lastGestureTime) > gestureDelay){
          count = count + 1;
          lastGestureTime = millis();
          countedThisCycle = true;
        }
      }
      Serial.println(count);
      #endif
    }
    #ifndef Calibrate
    // Check if it's time to toggle the buzzer
    if (buzzerTimer >= BUZZER_INTERVAL) {
      buzzerTimer -= BUZZER_INTERVAL;
      buzzerState = !buzzerState;
      if (buzzerState) {
        tone(BUZZER_PIN, BUZZER_FREQUENCY, BUZZER_DURATION);
        countedThisCycle = false;  // Reset the flag when the buzzer goes high
      }
    }
    #endif
  }
}

void displayLEDs(int count, unsigned long duration) {
  // Calculate how many LEDs should be lit
  int ledsToLight = map(count, 0, duration / 2000000, 0, NUM_LEDS);
  ledsToLight = constrain(ledsToLight, 0, NUM_LEDS);

  // Light up the LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < ledsToLight) {
      digitalWrite(LED_PINS[i], HIGH);
    } else {
      digitalWrite(LED_PINS[i], LOW);
    }
  }
}

// envelope detection algorithm
int getEnvelope(int abs_emg){
  sum -= circular_buffer[data_index];
  sum += abs_emg;
  circular_buffer[data_index] = abs_emg;
  data_index = (data_index + 1) % BUFFER_SIZE;
  return (sum/BUFFER_SIZE) * 2;
}

// Band-Pass Butterworth IIR digital filter, generated using filter_gen.py.
// Sampling rate: 500.0 Hz, frequency: [74.5, 149.5] Hz.
// Filter is order 4, implemented as second-order sections (biquads).
// Reference: 
// https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.butter.html
// https://courses.ideate.cmu.edu/16-223/f2020/Arduino/FilterDemos/filter_gen.py
float EMGFilter(float input)
{
  float output = input;
  {
    static float z1, z2; // filter section state
    float x = output - 0.05159732*z1 - 0.36347401*z2;
    output = 0.01856301*x + 0.03712602*z1 + 0.01856301*z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2; // filter section state
    float x = output - -0.53945795*z1 - 0.39764934*z2;
    output = 1.00000000*x + -2.00000000*z1 + 1.00000000*z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2; // filter section state
    float x = output - 0.47319594*z1 - 0.70744137*z2;
    output = 1.00000000*x + 2.00000000*z1 + 1.00000000*z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2; // filter section state
    float x = output - -1.00211112*z1 - 0.74520226*z2;
    output = 1.00000000*x + -2.00000000*z1 + 1.00000000*z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}
