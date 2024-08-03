// EMG_Rehab_Game - Muscle Bioamp Shield
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

// Envelope buffer size
// High value -> smooth but less responsive
// Low value -> not smooth but responsive
#define BUFFER_SIZE 64

// EMG Threshold value, different for each user
// Check by plotting EMG envelope data on Serial plotter
#define EMG_THRESHOLD 70

// Game variables
uint16_t count = 0;
unsigned long lastGestureTime = 0;
uint16_t gestureDelay = 500;

// Circular buffer for envelope calculation
int circular_buffer[BUFFER_SIZE];
int data_index, sum;

// Define the buzzer pin
#define BUZZER_PIN 8

// Buzzer timer variables
unsigned long buzzerTimer = 0;
unsigned long BUZZER_INTERVAL = 2000000; // 1 second in microseconds
bool buzzerState = false;

// Level tracking variables
int level = 0;
const uint8_t MAX_COUNT = 31;
const uint8_t COUNTS_PER_LEVEL = 32;
const uint8_t MAX_LEVEL = 6;
const uint32_t BUZZER_INTERVALS[] = {1500000, 1300000, 1100000, 900000, 700000, 500000};

// Buzzer tone settings
const int BUZZER_FREQUENCY = 2000;
const int BUZZER_DURATION = 100;

// LED pins for count display
const int LED_PINS_Count[] = {9, 10, 11, 12, 13};
const int NUM_LEDS_Count = 5;

// LED pins for level display
const int LED_PINS_Level[] = {8, 9, 10, 11, 12, 13};
const int NUM_LEDS_Level = 6;

// Game state variables
bool countIncrementedSinceBuzzer = false;
unsigned long lastBuzzerTime = 0;

// Button definitions
// For reset there is already a pin in Maker Uno
#define BUTTON_PIN 4
#define DEBOUNCE_DELAY 50

// Game control variables
bool gameRunning = true; // Game starts automatically
bool buttonPressed = false;
unsigned long lastButtonPress = 0;
int buttonState;
int lastButtonState = HIGH;

// Level display button
// Change this pin to 2 if using Maker Uno
#define LEVEL_BUTTON_PIN 7

// Level display button variables
bool levelButtonPressed = false;
int levelButtonState;
int lastLevelButtonState = HIGH;
bool displayingLevel = false;

// Win condition
const int WIN_THRESHOLD = 191;
bool gameWon = false;

void setup() {
  // Initialize serial communication
  Serial.begin(BAUD_RATE);
  
  // Set pin modes
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LEVEL_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize LED pins
  for (int i = 0; i < NUM_LEDS_Count; i++) {
    pinMode(LED_PINS_Count[i], OUTPUT);
  }
  for (int i = 0; i < NUM_LEDS_Level; i++) {
    pinMode(LED_PINS_Level[i], OUTPUT);
  }
  
  // Initialize LED display
  displayBinaryCount(0);
}

void loop() {
  // Button debouncing for restart button
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastButtonPress = millis();
  }

  if ((millis() - lastButtonPress) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        buttonPressed = true;
      }
    }
  }
  lastButtonState = reading;

  // Button debouncing for level display button
  int levelReading = digitalRead(LEVEL_BUTTON_PIN);
  if (levelReading != lastLevelButtonState) {
    lastButtonPress = millis();
  }

  if ((millis() - lastButtonPress) > DEBOUNCE_DELAY) {
    if (levelReading != levelButtonState) {
      levelButtonState = levelReading;
      if (levelButtonState == LOW) {
        levelButtonPressed = true;
      }
    }
  }
  lastLevelButtonState = levelReading;

  // Game restart management
  if (buttonPressed) {
    buttonPressed = false;
    if (gameRunning || gameWon) {
      // Reset game
      gameRunning = true;
      gameWon = false;
      count = 0;
      level = 0;
      for (int i = 0; i < NUM_LEDS_Level; i++) {
        digitalWrite(LED_PINS_Level[i], LOW);
      }
      noTone(BUZZER_PIN);
      BUZZER_INTERVAL = BUZZER_INTERVALS[0];
      Serial.println("Game reset!");
    }
  }

  // Level display button logic
  if (levelButtonPressed) {
    levelButtonPressed = false;
    if (gameWon) {
      // Toggle between win display and level display
      if (!displayingLevel) {
        displayingLevel = true;
        displayLevelLEDs();
        Serial.println("Displaying final level!");
      } else {
        displayingLevel = false;
        displayWinState();
        Serial.println("Displaying win state!");
      }
    } else {
      // Toggle between level display and game
      if (!displayingLevel) {
        displayingLevel = true;
        gameRunning = false;
        displayLevelLEDs();
        Serial.println("Displaying level!");
      } else {
        displayingLevel = false;
        gameRunning = true;
        displayBinaryCount(count);
        Serial.println("Resuming game!");
      }
    }
  }

  if (gameRunning) {
    // Game timer logic
    static unsigned long past = 0;
    unsigned long present = micros();
    unsigned long interval = present - past;
    past = present;

    static long timer = 0;
    timer -= interval;
    
    // Buzzer timer
    buzzerTimer += interval;

    // EMG sampling and processing
    if(timer < 0) {
      timer += 1000000 / SAMPLE_RATE;
      
      int sensor_value = analogRead(INPUT_PIN);
      int signal = EMGFilter(sensor_value);
      int envelope = getEnvelope(abs(signal));
      
      if(envelope > EMG_THRESHOLD) {
        if((millis() - lastGestureTime) > gestureDelay && !countIncrementedSinceBuzzer){
          // Increment count and update level
          count++;
          level = min(6, count / COUNTS_PER_LEVEL);
          lastGestureTime = millis();
          displayBinaryCount(count);
          countIncrementedSinceBuzzer = true;
          
          adjustBuzzerInterval();
          
          Serial.print("Count: ");
          Serial.print(count);
          Serial.print(" Level: ");
          Serial.println(level);

          // Check for win condition
          if (count > WIN_THRESHOLD && !gameWon) {
            gameWon = true;
            winAnimation();
            gameRunning = false;
            noTone(BUZZER_PIN);
            displayWinState();
            Serial.println("Game won!");
          }
        }
      }
    }
  }
  else if (displayingLevel) {
    displayLevelLEDs();
  }

  // Buzzer control
  if (buzzerTimer >= BUZZER_INTERVAL) {
    buzzerTimer -= BUZZER_INTERVAL;
    buzzerState = !buzzerState;
    if (buzzerState) {
      tone(BUZZER_PIN, BUZZER_FREQUENCY, BUZZER_DURATION);
      countIncrementedSinceBuzzer = false;
      lastBuzzerTime = micros();
    }
  }
}

// Adjust buzzer interval based on current level
void adjustBuzzerInterval() {
  int index = min(level, MAX_LEVEL);
  if (index >= 0 && index < sizeof(BUZZER_INTERVALS)/sizeof(BUZZER_INTERVALS[0])) {
    BUZZER_INTERVAL = BUZZER_INTERVALS[index];
  } else {
    BUZZER_INTERVAL = BUZZER_INTERVALS[sizeof(BUZZER_INTERVALS)/sizeof(BUZZER_INTERVALS[0]) - 1];
  }
}

// Display binary count on LEDs
void displayBinaryCount(int count) {
  count = count % 32;
  
  for (int i = 0; i < NUM_LEDS_Count; i++) {
    if (count & (1 << i)) {
      digitalWrite(LED_PINS_Count[i], HIGH);
    } else {
      digitalWrite(LED_PINS_Count[i], LOW);
    }
  }
}

// Display current level on LEDs
void displayLevelLEDs() {
  int displayLevel = min(level, MAX_LEVEL);
  for (int i = 0; i < NUM_LEDS_Level; i++) {
    digitalWrite(LED_PINS_Level[i], i < displayLevel ? HIGH : LOW);
  }
}

// Display win state on LEDs
void displayWinState() {
  for (int i = 0; i < NUM_LEDS_Level; i++) {
    digitalWrite(LED_PINS_Level[i], HIGH);
  }
}

// Animate win display
void winAnimation() {
  const int ANIMATION_SPEED = 100;
  const int REPEAT_COUNT = 3;
  
  for (int repeat = 0; repeat < REPEAT_COUNT; repeat++) {
    // Light up LEDs from 8 to 13
    for (int i = 0; i < NUM_LEDS_Level; i++) {
      digitalWrite(LED_PINS_Level[i], HIGH);
      delay(ANIMATION_SPEED);
    }
    
    delay(ANIMATION_SPEED * 2);
    
    // Turn off LEDs from 13 to 8
    for (int i = NUM_LEDS_Level - 1; i >= 0; i--) {
      digitalWrite(LED_PINS_Level[i], LOW);
      delay(ANIMATION_SPEED);
    }
    
    delay(ANIMATION_SPEED * 2);
  }
  
  // Final state: all LEDs on
  for (int i = 0; i < NUM_LEDS_Level; i++) {
    digitalWrite(LED_PINS_Level[i], HIGH);
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
