// LED Bar Graph - BioAmp EXG Pill
// https://github.com/upsidedownlabs/BioAmp-EXG-Pill

// Upside Down Labs invests time and resources providing this open source code,
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

// EMG Envelope baseline value
// Minimum value without flexing hand
#define EMG_ENVELOPE_BASELINE 4

// EMG Envelope divider
// Minimum value increase required to turn on a single LED
#define EMG_ENVELOPE_DIVIDER 4

int circular_buffer[BUFFER_SIZE];
int data_index, sum;

// Muscle BioAmp Shield v0.3 LED pin numbers in-order
int led_bar[] = {8, 9, 10, 11, 12, 13};

// Maker UNO LED pin numbers in-order
//int led_bar[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

int total_leds = sizeof(led_bar) / sizeof(led_bar[0]);

void setup() {
	// Serial connection begin
	Serial.begin(BAUD_RATE);
	// Initialize all the led_bar
  	for (int i = 0; i < total_leds; i++) {
    	pinMode(led_bar[i], OUTPUT);
  	}
}

void loop() {
	// Calculate elapsed time
	static unsigned long past = 0;
	unsigned long present = micros();
	unsigned long interval = present - past;
	past = present;

	// Run timer
	static long timer = 0;
	timer -= interval;

	// Sample and get envelop
	if(timer < 0) {
		timer += 1000000 / SAMPLE_RATE;
   
    // RAW EMG Values
    int sensor_value = analogRead(INPUT_PIN);
    
    // Filtered EMG
    int signal = EMGFilter(sensor_value);
    
    // EMG envelopee
    int envelope = getEnvelope(abs(signal));

    // Update LED bar graph
    for(int i = 0; i<=total_leds; i++){
      if(i>(envelope/EMG_ENVELOPE_DIVIDER - EMG_ENVELOPE_BASELINE)){
          digitalWrite(led_bar[i], LOW);
      } else {
          digitalWrite(led_bar[i], HIGH);
      }
    }

    // EMG Raw signal
    Serial.print(signal);
    // Data seprator
    Serial.print(",");
    // EMG envelopeee
    Serial.println(envelope);
	}
}

// Envelop detection algorithm
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
