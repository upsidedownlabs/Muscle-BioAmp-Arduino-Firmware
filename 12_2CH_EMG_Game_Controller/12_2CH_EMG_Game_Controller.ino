// EMGGAMECONTROLLER - BioAmp EXG Pill
// https://github.com/upsidedownlabs/BioAmp-EXG-Pill
// https://github.com/upsidedownlabs/Muscle-BioAmp-Arduino-Firmware

// Upside Down Labs invests time and resources providing this open source code,
// please support Upside Down Labs and open-source hardware by purchasing
// products from Upside Down Labs!

// Copyright (c) 2021 - 2024 Upside Down Labs - contact@upsidedownlabs.tech
// Copyright (c) 2021 - 2024 Aryan Prakhar - aryanprakhar1010@gmail.com

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

#define INPUT_PIN1 A0
#define INPUT_PIN2 A2

// CH1 & CH2 status LEDs
#define CH1_STATUS_LED 13
#define CH2_STATUS_LED 11

// Envelope buffer size
// High value -> smooth but less responsive
// Low value -> not smooth but responsive
#define BUFFER_SIZE 64

// Define different constant for different envelope
// Channel 1 variables
int circular_buffer1[BUFFER_SIZE];
int data_index1, sum1;
// Channel 2 varibles
int circular_buffer2[BUFFER_SIZE];
int data_index2, sum2;

// Button to start the serial transaction
const int buttonPin = 4;

// LED to show serial transaction status
const int ledPin = 8;

// Device working status varaiables
int ledState = LOW;
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Calibrate threshold values
// Uncomment the below line to view EMG envelope on serial plotter
// #define Calibrate

// Uncomment the line below to switch to advanced version of the code which provides better performance but may vary for different users
// #define Advanced

int threshold1 = 40;
int threshold2 = 60;

void setup() {
  // Serial connection begin
  Serial.begin(BAUD_RATE);
  
  pinMode(INPUT_PIN1, INPUT);
  pinMode(CH1_STATUS_LED, OUTPUT);
  
  pinMode(INPUT_PIN2, INPUT);
  pinMode(CH2_STATUS_LED, OUTPUT);

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, ledState);
}

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }
 
  // Calculate elapsed time
  static unsigned long past = 0;
  unsigned long present = micros();
  unsigned long interval = present - past;
  past = present;

  // Run timer
  static long timer = 0;
  timer -= interval;

  // Sample and get envelope
  if(timer < 0) {
    timer += 1000000 / SAMPLE_RATE;
    // RAW EMG
    int sensor_value1 = analogRead(INPUT_PIN1);
   
    // Filtered EMG
    int signal1 = EMGFilter1(sensor_value1);
   
    // EMG envelope
    int envelope1 = getEnvelope1(abs(signal1));
    int sensor_value2 = analogRead(INPUT_PIN2);
   
    // Filtered EMG
    int signal2 = EMGFilter2(sensor_value2);
   
    // EMG envelope
    int envelope2 = getEnvelope2(abs(signal2));

    // If set to calibrate show envelope data on serial monitor/plotter
    #ifdef Calibrate
    
      Serial.print(envelope1);
      Serial.print('\t');
      Serial.println(envelope2);
    
    #endif

    #ifdef Advanced

      // LED ON when one hand move otherwise off, same for other hand
      // envelope should be higher than the threshold and also the envelope of other hand
      // output as "1" for CH1 and "2" for CH2
      if(envelope1 > envelope2 and envelope1 > threshold1 and envelope2 < 20)
      { 
        if(envelope2>10)
        {
          Serial.println("4");
        }
        else
        {
        Serial.println("1");
        }
      }
      else if(envelope2 > envelope1 and envelope2 > threshold2)
      { 
        if(envelope1>20)
        {
          Serial.println("5");
        }
        else
        {
        Serial.println("2");
        }
      }
      else if(envelope1 > 20 and envelope1 < threshold1 and envelope2 < threshold2 and envelope2 > 10)
      { 
        Serial.println("3");
      }
      else
      { 
        Serial.println("0");
      }

    #else

      if(envelope1 > envelope2 and envelope1 > threshold1 and envelope2 < 20)
      { 
        Serial.println("1");
      }
      else if(envelope2 > envelope1 and envelope2 > threshold2)
      { 
        Serial.println("2");
      }
       else if(envelope1 > 20 and envelope1 < threshold1 and envelope2 < threshold2 and envelope2 > 10)
      { 
        Serial.println("3");
      }
      else
      { 
        Serial.println("0");
      }
      
    
   #endif
  }
  digitalWrite(ledPin, ledState);
 lastButtonState = reading;
}

// Envelope detection algorithm
// Get CH1 envelope
int getEnvelope1(int abs_emg){
  sum1 -= circular_buffer1[data_index1];
  sum1 += abs_emg;
  circular_buffer1[data_index1] = abs_emg;
  data_index1 = (data_index1 + 1) % BUFFER_SIZE;
  return (sum1/BUFFER_SIZE) * 2;
}
// Get CH2 envelope
int getEnvelope2(int abs_emg){
  sum2 -= circular_buffer2[data_index2];
  sum2 += abs_emg;
  circular_buffer2[data_index2] = abs_emg;
  data_index2 = (data_index2 + 1) % BUFFER_SIZE;
  return (sum2/BUFFER_SIZE) * 2;
}

// Band-Pass Butterworth IIR digital filter, generated using filter_gen.py.
// Sampling rate: 500.0 Hz, frequency: [74.5, 149.5] Hz.
// Filter is order 4, implemented as second-order sections (biquads).
// Reference: 
// https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.butter.html
// https://courses.ideate.cmu.edu/16-223/f2020/Arduino/FilterDemos/filter_gen.py

// CH1 EMG filter
float EMGFilter1(float input)
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

// CH2 EMG filter
float EMGFilter2(float input2)
{
  float output = input2;
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
