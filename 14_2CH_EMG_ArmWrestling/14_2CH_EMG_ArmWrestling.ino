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

// Samples per second
#define SAMPLE_RATE 500

// Make sure to set the same baud rate on your Serial Monitor/Plotter
#define BAUD_RATE 115200

// Change if your sensor is connected to a different analog pin
#define CHANNEL_1 A0  //  Has to close the claw
#define CHANNEL_2 A1  //  Has to open the claw

// Envelope buffer size
// High value -> smooth but less responsive
// Low value -> not smooth but responsive
#define BUFFER_SIZE 64
#define DEADZONE 30.0f
#define DEADZONE_DIFF 5.0f
#define SERVO_PIN 2
#define SERVO_OPEN 15.0f
#define SERVO_CLOSE 90.0f
#define SERVO_MIN 15.0f
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

// Servo control globals
Servo servo;
float armAngle = 45.0f;
uint32_t lastServo = 0;

// Max values
float maxValue1 = 0;
float maxValue2 = 0;

float strength1 = 0.0f;
float strength2 = 0.0f;

float active1 = 0.0f;
float active2 = 0.0f;

float diff = 0.0f;

// Player names
String player1Name;
String player2Name;
String winner;
char endChoice;

// Filter
struct BiquadState {
  float z1 = 0, z2 = 0;
};
BiquadState state1, state2;

// Calibration variables
bool calibrated = false;
float baseline1 = 0.0f;
float baseline2 = 0.0f;
uint32_t calibrationStart = 0;
uint32_t calibrationSamples = 0;

// Game over flag
bool gameOver = false;

void setup() {
  // Initialize serial communication
  Serial.begin(BAUD_RATE);
  while (!Serial)
    delay(10);

  servo.attach(SERVO_PIN);
  servo.write(45);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  startMenu();

  calibrationStart = millis();
}

void loop() {
  if (gameOver) {
    return;
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
  if (timer < 0) {
    timer += 1000000 / SAMPLE_RATE;

    // RAW EMG
    float sensor_value1 = analogRead(CHANNEL_1);
    float sensor_value2 = analogRead(CHANNEL_2);

    // EMG Filter
    float signal1 = EMGFilter1(sensor_value1);
    float signal2 = EMGFilter2(sensor_value2);

    // Detect envelope of filtered signal
    float envelope1 = min(getEnvelope1(fabsf(signal1)), 150.0f);
    float envelope2 = min(getEnvelope2(fabsf(signal2)), 150.0f);

    if (updateCalibration(envelope1, envelope2))
      return;

    active1 = max(0.0f, envelope1 - baseline1);
    active2 = max(0.0f, envelope2 - baseline2);

    strength1 = 0.9f * strength1 + 0.1f * active1;
    strength2 = 0.9f * strength2 + 0.1f * active2;
    active2 = max(0.0f, strength2 - baseline2);

    Serial.print(active1);
    Serial.print(" , ");
    Serial.println(active2);

    diff = active1 - active2;
    diff = constrain(diff, -10.0f, 10.0f);

    if (fabsf(diff) < DEADZONE_DIFF)
      diff = 0.0f;
    updateServo();
  }
}

bool updateCalibration(float s1, float s2) {
  uint32_t elapsed = millis() - calibrationStart;

  if (!calibrated && elapsed >= 3000) {
    baseline1 /= calibrationSamples;
    baseline2 /= calibrationSamples;
    calibrated = true;

    Serial.println("Calibration Complete");
    Serial.print("Baseline1: ");
    Serial.println(baseline1);

    Serial.print("Baseline2: ");
    Serial.println(baseline2);
    return false;
  }

  if (!calibrated) {
    baseline1 += s1;
    baseline2 += s2;
    calibrationSamples++;

    digitalWrite(LED_PIN, (millis() / 500) % 2);
    return true;
  }
  return false;
}

void updateServo() {
  if (millis() - lastServo >= 20) {
    lastServo = millis();

    armAngle += diff * ANGLE_SENSNTIVITY;
    armAngle = constrain(armAngle, SERVO_OPEN, SERVO_CLOSE);

    servo.write(armAngle);

    gameResult();
  }
}

void startMenu() {

  Serial.println();
  Serial.println("███████╗███╗   ███╗ ██████╗");
  Serial.println("██╔════╝████╗ ████║██╔════╝");
  Serial.println("█████╗  ██╔████╔██║██║  ███╗");
  Serial.println("██╔══╝  ██║╚██╔╝██║██║   ██║");
  Serial.println("███████╗██║ ╚═╝ ██║╚██████╔╝");
  Serial.println("╚══════╝╚═╝     ╚═╝ ╚═════╝");

  Serial.println();

  Serial.println(" █████╗ ██████╗ ███╗   ███╗");
  Serial.println("██╔══██╗██╔══██╗████╗ ████║");
  Serial.println("███████║██████╔╝██╔████╔██║");
  Serial.println("██╔══██║██╔══██╗██║╚██╔╝██║");
  Serial.println("██║  ██║██║  ██║██║ ╚═╝ ██║");
  Serial.println("╚═╝  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝");

  Serial.println();

  Serial.println("██╗    ██╗██████╗ ███████╗███████╗████████╗██╗     ██╗███╗   ██╗ ██████╗");
  Serial.println("██║    ██║██╔══██╗██╔════╝██╔════╝╚══██╔══╝██║     ██║████╗  ██║██╔════╝");
  Serial.println("██║ █╗ ██║██████╔╝█████╗  ███████╗   ██║   ██║     ██║██╔██╗ ██║██║  ███╗");
  Serial.println("██║███╗██║██╔══██╗██╔══╝  ╚════██║   ██║   ██║     ██║██║╚██╗██║██║   ██║");
  Serial.println("╚███╔███╔╝██║  ██║███████╗███████║   ██║   ███████╗██║██║ ╚████║╚██████╔╝");
  Serial.println(" ╚══╝╚══╝ ╚═╝  ╚═╝╚══════╝╚══════╝   ╚═╝   ╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝");

  Serial.println();
  Serial.println("===============================================================");
  Serial.println("            Muscle BioAmp Patchy Competition");
  Serial.println("===============================================================");
  Serial.println();

  Serial.print("Enter Player 1 Name (Close): ");
  while (!Serial.available())
    ;
  player1Name = Serial.readStringUntil('\n');
  player1Name.trim();

  Serial.print("Enter Player 2 Name (Open) : ");
  while (!Serial.available())
    ;
  player2Name = Serial.readStringUntil('\n');
  player2Name.trim();

  Serial.println();
  Serial.println("===============================================================");
  Serial.print("   ");
  Serial.print(player1Name);
  Serial.print("   VS   ");
  Serial.println(player2Name);
  Serial.println("===============================================================");
  Serial.println();
  Serial.println("Relax your muscles...");
  Serial.println("Calibration starts for 3 seconds.");
  Serial.println();
}

void gameResult() {
  if (armAngle <= SERVO_MIN) {
    gameOver = true;
    servo.write(SERVO_MIN);

    winner = player2Name;
  }

  if (armAngle >= SERVO_MAX) {
    gameOver = true;
    servo.write(SERVO_MAX);

    winner = player1Name;
  }

  if (gameOver) {
    Serial.println();
    Serial.println("███╗   ███╗ █████╗ ████████╗ ██████╗██╗  ██╗");
    Serial.println("████╗ ████║██╔══██╗╚══██╔══╝██╔════╝██║  ██║");
    Serial.println("██╔████╔██║███████║   ██║   ██║     ███████║");
    Serial.println("██║╚██╔╝██║██╔══██║   ██║   ██║     ██╔══██║");
    Serial.println("██║ ╚═╝ ██║██║  ██║   ██║   ╚██████╗██║  ██║");
    Serial.println("╚═╝     ╚═╝╚═╝  ╚═╝   ╚═╝    ╚═════╝╚═╝  ╚═╝");

    Serial.println();

    Serial.println(" ██████╗ ██╗   ██╗███████╗██████╗ ");
    Serial.println("██╔═══██╗██║   ██║██╔════╝██╔══██╗");
    Serial.println("██║   ██║██║   ██║█████╗  ██████╔╝");
    Serial.println("██║   ██║╚██╗ ██╔╝██╔══╝  ██╔══██╗");
    Serial.println("╚██████╔╝ ╚████╔╝ ███████╗██║  ██║");
    Serial.println(" ╚═════╝   ╚═══╝  ╚══════╝╚═╝  ╚═╝");

    Serial.println();
    Serial.println("===============================================================");
    Serial.println("                 Muscle BioAmp Patchy Competition");
    Serial.println("===============================================================");
    Serial.println();

    Serial.print("                 WINNER : ");
    Serial.println(winner);

    Serial.println();
    Serial.println("===============================================================");
    Serial.println("                 WHAT WOULD YOU LIKE TO DO?");
    Serial.println("===============================================================");
    Serial.println();
    Serial.println("  [1] REMATCH");
    Serial.println("      Same players. Recalibrate and play again.");
    Serial.println();
    Serial.println("  [2] NEW OPPONENTS");
    Serial.println("      Enter two new player names.");
    Serial.println();
    Serial.println("  [3] EXIT");
    Serial.println("      End the competition.");
    Serial.println();
    Serial.print("Enter your choice (1-3): ");

    while (!Serial.available())
      digitalWrite(LED_PIN, (millis() / 200) % 2);

    String choiceStr = Serial.readStringUntil('\n');
    choiceStr.trim();
    endChoice = choiceStr.length() > 0 ? choiceStr[0] : '\0';

    handleEndChoice();
  }
}

void handleEndChoice() {

  switch (endChoice) {

    case '1':  // Rematch
      Serial.println("\nStarting Rematch...\n");

      digitalWrite(LED_PIN, LOW);

      armAngle = 45.0f;
      servo.write(45);

      strength1 = strength2 = 0.0f;
      active1 = active2 = 0.0f;
      diff = 0.0f;
      lastServo = 0;
      winner = "";

      baseline1 = baseline2 = 0.0f;
      calibrationSamples = 0;
      calibrated = false;
      calibrationStart = millis();

      gameOver = false;
      break;

    case '2':  // New opponents
      Serial.println("\nNew Opponents Selected\n");

      digitalWrite(LED_PIN, LOW);

      player1Name = "";
      player2Name = "";

      armAngle = 45.0f;
      servo.write(45);

      strength1 = strength2 = 0.0f;
      active1 = active2 = 0.0f;
      diff = 0.0f;
      lastServo = 0;
      winner = "";
      baseline1 = baseline2 = 0.0f;
      calibrationSamples = 0;
      calibrated = false;
      gameOver = false;

      startMenu();
      calibrationStart = millis();
      break;

    case '3':  // Exit
      Serial.println("\nThanks for playing!");
      while (1)
        ;

    default:
      Serial.println("\nInvalid choice! Starting new opponent selection by default.\n");

      digitalWrite(LED_PIN, LOW);

      player1Name = "";
      player2Name = "";

      armAngle = 45.0f;
      servo.write(45);

      strength1 = strength2 = 0.0f;
      active1 = active2 = 0.0f;
      diff = 0.0f;
      lastServo = 0;
      winner = "";

      baseline1 = baseline2 = 0.0f;
      calibrationSamples = 0;
      calibrated = false;
      gameOver = false;

      startMenu();
      calibrationStart = millis();
      break;
  }
}

// Envelope detection algorithm
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
    static float z1, z2;  // filter section state
    float x = output - 0.05159732 * z1 - 0.36347401 * z2;
    output = 0.01856301 * x + 0.03712602 * z1 + 0.01856301 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  // filter section state
    float x = output - -0.53945795 * z1 - 0.39764934 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  // filter section state
    float x = output - 0.47319594 * z1 - 0.70744137 * z2;
    output = 1.00000000 * x + 2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  // filter section state
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
    static float z1, z2;  // filter section state
    float x = output - 0.05159732 * z1 - 0.36347401 * z2;
    output = 0.01856301 * x + 0.03712602 * z1 + 0.01856301 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  // filter section state
    float x = output - -0.53945795 * z1 - 0.39764934 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  // filter section state
    float x = output - 0.47319594 * z1 - 0.70744137 * z2;
    output = 1.00000000 * x + 2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2;  // filter section state
    float x = output - -1.00211112 * z1 - 0.74520226 * z2;
    output = 1.00000000 * x + -2.00000000 * z1 + 1.00000000 * z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}