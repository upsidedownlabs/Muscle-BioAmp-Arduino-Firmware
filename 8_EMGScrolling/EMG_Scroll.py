# Human Computer Interface (HCI) 
# scroll through the screen

import serial
import time
import pyautogui


# return time in ms
def milis():
    return int(round(time.time() * 1000))

# Arduino serial port interface
ser = serial.Serial('COM7', 115200, timeout=1)

# Timing variable
timer = milis()

latency =  20

# Infinite loop
while True:
    try:
        # Process serial data
        data = ser.readline().decode('utf-8').strip()
        # print(data)
        
        # Debounce
        if((milis() - timer) > latency):
            timer = milis()
            # Emulate Up navigation key
            if(int(data)==1):
                pyautogui.press('up')
                print("up")
                
            # Emulate Down navigation key
            elif(int(data)==2):
                pyautogui.press('down')
                print("down")
                
    except Exception as e:
        print(e, "\nMove now...")
        continue
