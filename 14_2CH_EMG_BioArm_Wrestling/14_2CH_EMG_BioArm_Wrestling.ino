// EMG envelope - BioAmp EXG Pill
// https://github.com/upsidedownlabs/BioAmp-EXG-Pill

// Upside Down Labs invests time and resources providing this open source code,a
// please support Upside Down Labs and open-source hardware by purchasing
// products from Upside Down Labs!

// Copyright (c) 2021 - 2024 Upside Down Labs - contact@upsidedownlabs.tech

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

#include <Servo.h>

#define SAMPLE_RATE 500
#define BAUD_RATE 115200

#define CHANNEL_1 A0  // closes the claw
#define CHANNEL_2 A1  // opens the claw

#define BUFFER_SIZE 64
#define DEADZONE_DIFF 5.0f
#define SERVO_PIN 2
#define SERVO_OPEN 10.0f
#define SERVO_CLOSE 90.0f
#define SERVO_MIN 10.0f
#define SERVO_MAX 90.0f
#define ANGLE_SENSNTIVITY 0.03f
#define LED_PIN LED_BUILTIN

// Channel 1 envelope state
float circular_buffer1[BUFFER_SIZE] = { 0 };
float sum1 = 0;
int data_index1 = 0;

// Channel 2 envelope state
float circular_buffer2[BUFFER_SIZE] = { 0 };
float sum2 = 0;
int data_index2 = 0;

Servo servo;
float armAngle = 45.0f;
uint32_t lastServo = 0;
uint32_t lastTelemetry = 0;

float strength1 = 0.0f;
float strength2 = 0.0f;
float active1 = 0.0f;
float active2 = 0.0f;
float diff = 0.0f;

// Game state machine
enum GameState { ST_IDLE,
                 ST_PLAYING,
                 ST_GAMEOVER };
GameState state = ST_IDLE;

// Incoming command line buffer
String rxLine = "";

void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial)
    delay(10);

  servo.attach(SERVO_PIN);
  servo.write(50);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("READY");
}

void loop() {
  readCommands();

  if (state == ST_IDLE || state == ST_GAMEOVER) {
    return;
  }

  static unsigned long past = 0;
  unsigned long present = micros();
  unsigned long interval = present - past;
  past = present;

  static long timer = 0;
  timer -= interval;

  if (timer < 0) {
    timer += 1000000 / SAMPLE_RATE;

    float sensor_value1 = analogRead(CHANNEL_1);
    float sensor_value2 = analogRead(CHANNEL_2);

    float signal1 = EMGFilter1(sensor_value1);
    float signal2 = EMGFilter2(sensor_value2);

    float envelope1 = min(getEnvelope1(fabsf(signal1)), 150.0f);
    float envelope2 = min(getEnvelope2(fabsf(signal2)), 150.0f);

    active1 = max(0.0f, envelope1);
    active2 = max(0.0f, envelope2);

    strength1 = 0.9f * strength1 + 0.1f * active1;
    strength2 = 0.9f * strength2 + 0.1f * active2;

    diff = active1 - active2;
    diff = constrain(diff, -10.0f, 10.0f);
    if (fabsf(diff) < DEADZONE_DIFF) diff = 0.0f;

    updateServo();
    sendTelemetry();
  }
}

void readCommands() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (rxLine.length() > 0) {
        handleCommand(rxLine);
        rxLine = "";
      }
    } else {
      rxLine += c;
    }
  }
}

void handleCommand(String cmd) {
  cmd.trim();
  if (cmd == "START" || cmd == "REMATCH") {
    startGame();
  }
}

void startGame() {
  digitalWrite(LED_PIN, LOW);

  resetEnvelope();
  armAngle = 45.0f;
  servo.write(45);

  strength1 = strength2 = 0.0f;
  active1 = active2 = 0.0f;
  diff = 0.0f;
  lastServo = 0;

  state = ST_PLAYING;
  Serial.println("STARTED");
}

void resetEnvelope() {
  sum1 = 0;
  sum2 = 0;
  data_index1 = 0;
  data_index2 = 0;

  for (int i = 0; i < BUFFER_SIZE; i++) {
    circular_buffer1[i] = 0;
    circular_buffer2[i] = 0;
  }
}

void updateServo() {
  if (millis() - lastServo >= 20) {
    lastServo = millis();

    armAngle += diff * ANGLE_SENSNTIVITY;
    armAngle = constrain(armAngle, SERVO_OPEN, SERVO_CLOSE);

    servo.write(armAngle);

    checkGameOver();
  }
}

void checkGameOver() {
  if (armAngle <= SERVO_MIN) {
    servo.write(SERVO_MIN);
    state = ST_GAMEOVER;
    Serial.println("WIN,2");
  } else if (armAngle >= SERVO_MAX) {
    servo.write(SERVO_MAX);
    state = ST_GAMEOVER;
    Serial.println("WIN,1");
  }
}

void sendTelemetry() {
  if (millis() - lastTelemetry >= 20) {  // ~50 Hz
    lastTelemetry = millis();
    Serial.print("EMG,");
    Serial.print(active1);
    Serial.print(",");
    Serial.print(active2);
    Serial.print(",");
    Serial.println(armAngle);
  }
}

// ---- Envelope detection (unchanged) ----
float getEnvelope1(float abs_emg) {
  sum1 -= circular_buffer1[data_index1];
  sum1 += abs_emg;
  circular_buffer1[data_index1] = abs_emg;
  data_index1 = (data_index1 + 1) % BUFFER_SIZE;
  return (sum1 / BUFFER_SIZE) * 2;
}

float getEnvelope2(float abs_emg) {
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
float EMGFilter1(float input) {
  float output = input;
  {
    static float z1, z2;
    float x = output - 0.05159732 * z1 - 0.36347401 * z2;
    output = 0.01856301 * x + 0.03712602 * z1 + 0.01856301 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;
    float x = output - -0.53945795 * z1 - 0.39764934 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;
    float x = output - 0.47319594 * z1 - 0.70744137 * z2;
    output = 1.00000000 * x + 2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;
    float x = output - -1.00211112 * z1 - 0.74520226 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}

float EMGFilter2(float input) {
  float output = input;
  {
    static float z1, z2;
    float x = output - 0.05159732 * z1 - 0.36347401 * z2;
    output = 0.01856301 * x + 0.03712602 * z1 + 0.01856301 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;
    float x = output - -0.53945795 * z1 - 0.39764934 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;
    float x = output - 0.47319594 * z1 - 0.70744137 * z2;
    output = 1.00000000 * x + 2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;
    float x = output - -1.00211112 * z1 - 0.74520226 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}