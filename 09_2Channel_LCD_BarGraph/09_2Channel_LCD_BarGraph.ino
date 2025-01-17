// 2Channel_LCDBarGraph- Muscle Bioamp Patchy
// https://github.com/upsidedownlabs/Muscle-BioAmp-Arduino-Firmware

// Upside Down Labs invests time and resources providing this open source code,
// please support Upside Down Labs and open-source hardware by purchasing
// products from Upside Down Labs!

// Copyright (c) 2021 - 2024 Upside Down Labs - contact [at] upsidedownlabs.tech
// Copyright (c) 2021 - 2024 Aryan Prakhar - aryanprakhar1010 [at] gmail.com

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

// Libraries required
// By Marco Schwartz  
//https://github.com/johnrickman/LiquidCrystal_I2C/blob/master/LiquidCrystal_I2C.h
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 

// Initialize LCD object with I2C address 0x27, 16 columns and 2 rows
// Serial LCD I2C module- PCF8574
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Custom character definition
byte fullBlock[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

// Sampling rate in Hz
#define SAMPLE_RATE 500

// Serial communication baud rate
#define BAUD_RATE 115200
//Connected directly to Analog Electrodes port of Muscle Bioamp Shield
#define INPUT_PIN1 A0
//Connected to A2 Analog port of Muscle Bioamp Shield through Muscle Bioamp Patchy
#define INPUT_PIN2 A2  

// Buffer size for envelope detection
// Higher value: smoother but less responsive
// Lower value: less smooth but more responsive
#define BUFFER_SIZE 16

// Channel 1 envelope variables
int circular_buffer1[BUFFER_SIZE];
int data_index1, sum1;

// Channel 2 envelope variables
int circular_buffer2[BUFFER_SIZE];
int data_index2, sum2;

// Minimum EMG value without flexing hand
#define EMG_ENVELOPE_BASELINE 4

// EMG Envelope divider for LED bar graph scaling
#define EMG_ENVELOPE_DIVIDER 4

// LCD column indices for bar graph
int lcd_bar[] = {4,5,6,7,8, 9, 10, 11, 12, 13,14,15};

// Total number of blocks in LCD bar graph
int total_blocks = sizeof(lcd_bar) / sizeof(lcd_bar[0]);


void setup() {
  // Initialize serial communication
  Serial.begin(BAUD_RATE);

  // Initialize LCD
  lcd.begin(16,2);
  lcd.createChar(0, fullBlock);
  lcd.backlight();

  // Set EMG input pins
  pinMode(INPUT_PIN1, INPUT);
  pinMode(INPUT_PIN2, INPUT);
  }

void loop() {
  // Calculate elapsed time for precise timing
  static unsigned long past = 0;
  unsigned long present = micros();
  unsigned long interval = present - past;
  past = present;

  // Timer for maintaining sample rate
  static long timer = 0;
  timer -= interval;

  // Sample and process EMG data at the specified sample rate
  if(timer < 0) {
    timer += 1000000 / SAMPLE_RATE;
    // Process Channel 1
    int sensor_value1 = analogRead(INPUT_PIN1);
    int signal1 = EMGFilter1(sensor_value1);
    int envelope1 = getEnvelope1(abs(signal1));

    // Process Channel 2
    int sensor_value2 = analogRead(INPUT_PIN2);
    int signal2 = EMGFilter2(sensor_value2);
    int envelope2 = getEnvelope2(abs(signal2));

    // Update LCD bar graph for Channel 1
    for(int i = 0; i<=total_blocks; i++){
      if(i>(envelope1/EMG_ENVELOPE_DIVIDER - EMG_ENVELOPE_BASELINE)){
          lcd.setCursor(lcd_bar[i],0);
          lcd.print(" ");
      } else {
          lcd.setCursor(lcd_bar[i],0);
          lcd.write(byte(0));
      }
    }
    // Update LCD bar graph for Channel 2
    for(int j = 0; j<=total_blocks; j++){
      if(j>(envelope2/EMG_ENVELOPE_DIVIDER - EMG_ENVELOPE_BASELINE)){
           lcd.setCursor(lcd_bar[j],1);
          lcd.print((" "));
      } else {
          lcd.setCursor(lcd_bar[j],1);
          lcd.write(byte(0));
      }
   }
    // Display Envelope value of Channel 1
    lcd.print("   ");
    lcd.setCursor(0,0);
    lcd.print(envelope1);

    // Display Envelope value of Channel 2
    lcd.print("   ");
    lcd.setCursor(0,1);
    lcd.print(envelope2);
  }
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

// Get CH2 Envelope
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

//CH1 EMG Filter 
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

//CH2 EMG Filter 
float EMGFilter2(float input)
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
