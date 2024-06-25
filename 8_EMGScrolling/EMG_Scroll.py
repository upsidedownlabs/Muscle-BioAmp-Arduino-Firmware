#  EMGScrolling - BioAmp EXG Pill
#  https://github.com/upsidedownlabs/BioAmp-EXG-Pill

#  Upside Down Labs invests time and resources providing this open source code,
#  please support Upside Down Labs and open-source hardware by purchasing
#  products from Upside Down Labs!

#  Copyright (c) 2021 - 2024 Upside Down Labs - contact@upsidedownlabs.tech
#  Copyright (c) 2021 - 2024 Aryan Prakhar - aryanprakhar1010@gmail.com

#  Permission is hereby granted, free of charge, to any person obtaining a copy
#  of this software and associated documentation files (the "Software"), to deal
#  in the Software without restriction, including without limitation the rights
#  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the Software is
#  furnished to do so, subject to the following conditions:

#  The above copyright notice and this permission notice shall be included in all
#  copies or substantial portions of the Software.

#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.

# Human Computer Interface (HCI) project example
# Emulate UP/Down keys to move through the screen

import serial
import time
import pyautogui

# return time in ms
def milis():
    return int(round(time.time() * 1000))

# Arduino serial port interface
# Change the COM port depending on the COM port selected
ser = serial.Serial('COM7', 115200, timeout=1)

# Timing variable
timer = milis()

# Update the latency value accordingly
# High latency value -> less responsive
# Low latency value -> more responsive
latency =  20

# Infinite loop
while True:
    try:
        # Process serial data
        data = ser.readline().decode('utf-8').strip()
        
        # Debounce
        if((milis() - timer) > latency):
            timer = milis()
            
            # Virtual up and down      
            if(int(data)==1):
                pyautogui.press('up')
                print("up")
               
            elif(int(data)==2):
                pyautogui.press('down')
                print("down")
                
    except Exception as e:
        print(e, "\nMove now...")
        continue
