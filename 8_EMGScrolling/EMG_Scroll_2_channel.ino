// Samples per second
#define SAMPLE_RATE 500

// Make sure to set the same baud rate on your Serial Monitor/Plotter
#define BAUD_RATE 115200

// Change if not using A0 analog pin
#define INPUT_PIN1 A0
#define INPUT_PIN2 A2
#define OUTPUT_PIN1 13
#define OUTPUT_PIN2 11
// envelope buffer size
// High value -> smooth but less responsive
// Low value -> not smooth but responsive
#define BUFFER_SIZE 64

int circular_buffer1[BUFFER_SIZE];
int data_index1, sum1;
int circular_buffer2[BUFFER_SIZE];
int data_index2, sum2;

int threshold1 = 70;
int threshold2 = 70;
const int buttonPin = 4;
const int ledPin = 8;
int ledState = LOW;
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
void setup() {
  // Serial connection begin
  Serial.begin(BAUD_RATE);
  pinMode(INPUT_PIN1, INPUT);
  pinMode(OUTPUT_PIN1, OUTPUT);
  pinMode(INPUT_PIN1, INPUT);
  pinMode(OUTPUT_PIN1, OUTPUT);

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

 

      digitalWrite(OUTPUT_PIN1,envelope1 > envelope2 and envelope1 > threshold1);
      digitalWrite(OUTPUT_PIN2, envelope2 > envelope1 and envelope2 > threshold2 );
      if(envelope1 and ledState){
      if(envelope1 > envelope2 and envelope1 > threshold1) Serial.println("1");}
      if(envelope2 and ledState){
      if(envelope2 > envelope1 and envelope2 > threshold2) Serial.println("2");}
      if(envelope2 > threshold1 and envelope2 > threshold2) Serial.println("0");
      else Serial.println("0");
  }
  digitalWrite(ledPin, ledState);
 lastButtonState = reading;
}

// envelope detection algorithm
int getEnvelope1(int abs_emg){
  sum1 -= circular_buffer1[data_index1];
  sum1 += abs_emg;
  circular_buffer1[data_index1] = abs_emg;
  data_index1 = (data_index1 + 1) % BUFFER_SIZE;
  return (sum1/BUFFER_SIZE) * 2;
}
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
