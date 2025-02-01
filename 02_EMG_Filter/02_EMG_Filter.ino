// EMG Filter - BioAmp EXG Pill
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


void setup() {
	// Serial connection begin
	Serial.begin(BAUD_RATE);
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

	// Sample
	if(timer < 0){
		timer += 1000000 / SAMPLE_RATE;
        // Get analog input value (Raw EMG)
		float sensor_value = analogRead(INPUT_PIN);
        // Apply the band-stop filter (48 Hz to 52 Hz)
		float bandstop_filtered = BandStopFilter(sensor_value);

		// Apply the high-pass filter (70 Hz)
		float highpass_filtered = HighPassFilter(bandstop_filtered);

		// Print the final filtered signal
		Serial.print(highpass_filtered);

    // Uncomment below 2 lines to compare with raw data
    // Serial.print(" ");
    // Serial.println(sensor_value);
	}
}

// High-Pass Butterworth IIR digital filter, generated using filter_gen.py.
// Sampling rate: 500.0 Hz, frequency: 70.0 Hz.
// Filter is order 2, implemented as second-order sections (biquads).
// Reference: https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.butter.html
float HighPassFilter(float input)
{
  float output = input;
  {
    static float z1, z2; // filter section state
    float x = output - -0.82523238*z1 - 0.29463653*z2;
    output = 0.52996723*x + -1.05993445*z1 + 0.52996723*z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}

// Band-Stop Butterworth IIR digital filter, generated using filter_gen.py.
// Sampling rate: 500.0 Hz, frequency: [48.0, 52.0] Hz.
// Filter is order 2, implemented as second-order sections (biquads).
// Reference: https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.butter.html
float BandStopFilter(float input)
{
  float output = input;
  {
    static float z1, z2; // filter section state
    float x = output - -1.56858163*z1 - 0.96424138*z2;
    output = 0.96508099*x + -1.56202714*z1 + 0.96508099*z2;
    z2 = z1;
    z1 = x;
  }
  {
    static float z1, z2; // filter section state
    float x = output - -1.61100358*z1 - 0.96592171*z2;
    output = 1.00000000*x + -1.61854514*z1 + 1.00000000*z2;
    z2 = z1;
    z1 = x;
  }
  return output;
}
