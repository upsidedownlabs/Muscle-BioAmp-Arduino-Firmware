// BioArm Wrestling
// https://github.com/upsidedownlabs/Muscle-BioAmp-Patchy

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

Servo servo;
float armAngle = 50.0f;
uint32_t lastServo = 0;
uint32_t lastTelemetry = 0;

float strength1 = 0.0f;
float strength2 = 0.0f;
float active1 = 0.0f;
float active2 = 0.0f;
float diff = 0.0f;

unsigned long samplePast = 0;
long sampleTimer = 0;

// Game state machine
enum GameState { ST_IDLE,
                 ST_PLAYING,
                 ST_GAMEOVER };
GameState state = ST_IDLE;

// Incoming command line buffer
constexpr size_t MAX_CMD_LEN = 16;
String rxLine = "";

class EMGChannel {
private:
  float circular_buffer[BUFFER_SIZE] = { 0 };
  float sum = 0;
  int data_index = 0;

  // Filter state
  float s1_z1 = 0, s1_z2 = 0;
  float s2_z1 = 0, s2_z2 = 0;
  float s3_z1 = 0, s3_z2 = 0;
  float s4_z1 = 0, s4_z2 = 0;

public:
  void reset() {
    sum = 0;
    data_index = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
      circular_buffer[i] = 0;

    s1_z1 = s1_z2 = 0;
    s2_z1 = s2_z2 = 0;
    s3_z1 = s3_z2 = 0;
    s4_z1 = s4_z2 = 0;
  }

  // Band-Pass Butterworth IIR digital filter, generated using filter_gen.py.
  // Sampling rate: 500.0 Hz, frequency: [74.5, 149.5] Hz.
  // Filter is order 4, implemented as second-order sections (biquads).
  // Reference:
  // https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.butter.html
  // https://courses.ideate.cmu.edu/16-223/f2020/Arduino/FilterDemos/filter_gen.py
  float filter(float input) {
    float output = input;

    {
      float x = output - 0.05159732f * s1_z1 - 0.36347401f * s1_z2;
      output = 0.01856301f * x + 0.03712602f * s1_z1 + 0.01856301f * s1_z2;
      s1_z2 = s1_z1;
      s1_z1 = x;
    }

    {
      float x = output - -0.53945795f * s2_z1 - 0.39764934f * s2_z2;
      output = x - 2.0f * s2_z1 + s2_z2;
      s2_z2 = s2_z1;
      s2_z1 = x;
    }

    {
      float x = output - 0.47319594f * s3_z1 - 0.70744137f * s3_z2;
      output = x + 2.0f * s3_z1 + s3_z2;
      s3_z2 = s3_z1;
      s3_z1 = x;
    }

    {
      float x = output - -1.00211112f * s4_z1 - 0.74520226f * s4_z2;
      output = x - 2.0f * s4_z1 + s4_z2;
      s4_z2 = s4_z1;
      s4_z1 = x;
    }

    return output;
  }

  // Algorithm to get the envelope of EMG waves
  float envelope(float abs_emg) {
    sum -= circular_buffer[data_index];
    sum += abs_emg;
    circular_buffer[data_index] = abs_emg;
    data_index = (data_index + 1) % BUFFER_SIZE;

    return (sum / BUFFER_SIZE) * 2.0f;
  }
};
// Filter objects
EMGChannel player1;
EMGChannel player2;

void readCommands() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (rxLine.length() > 0) {
        handleCommand(rxLine);
        rxLine = "";
      }
    } else {
      if (rxLine.length() < MAX_CMD_LEN) {
        rxLine += c;
      }
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

  player1.reset();
  player2.reset();
  armAngle = 50.0f;
  servo.write(50);

  strength1 = strength2 = 0.0f;
  active1 = active2 = 0.0f;
  diff = 0.0f;
  lastServo = 0;
  samplePast = 0;
  sampleTimer = 0;

  state = ST_PLAYING;
  Serial.println("STARTED");
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

  unsigned long present = micros();
  unsigned long interval = present - samplePast;
  samplePast = present;

  sampleTimer -= interval;

  if (sampleTimer < 0) {
    sampleTimer += 1000000 / SAMPLE_RATE;

    float sensor_value1 = analogRead(CHANNEL_1);
    float sensor_value2 = analogRead(CHANNEL_2);

    float signal1 = player1.filter(sensor_value1);
    float signal2 = player2.filter(sensor_value2);

    float envelope1 = player1.envelope(fabsf(signal1));
    float envelope2 = player2.envelope(fabsf(signal2));

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