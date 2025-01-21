import serial
import time
import keyboard
from DetectComPort import find_responsive_port

# Function to get the current time in milliseconds
def millis():
    return int(round(time.time() * 1000))

# Arduino serial port interface
ser = serial.Serial(find_responsive_port(), 115200, timeout=0.1)

# Timing variables for holding keys
keystroke_hold_time = {}  # Tracks the time of each keystroke press
key_release_time = {}  # Tracks when to release each key

# Variables for buffering
buffer_size = 20
buffer = []

while True:
    try:
        # Read and decode serial data
        data = ser.readline().decode('utf-8').strip()  # Strip newline and carriage return
        
        if data:
            # Add the data to the buffer
            buffer.append(data)
            
            # Process buffer when it reaches the required size
            if len(buffer) >= buffer_size:
                # Determine the most common keystroke in the buffer
                most_common = max(set(buffer), key=buffer.count)
                current_time = millis()
                
                # Handle keystroke 1 (left key press and release after 100ms)
                if most_common == "1":
                    if most_common not in keystroke_hold_time or current_time - keystroke_hold_time[most_common] >= 150:
                        keyboard.press('left')
                        print("left pressed")
                        keystroke_hold_time[most_common] = current_time
                        key_release_time[most_common] = current_time + 150  # Release after 100ms
                
                # Handle keystroke 2 (right key press and release after 100ms)
                elif most_common == "2":
                    if most_common not in keystroke_hold_time or current_time - keystroke_hold_time[most_common] >= 150:
                        keyboard.press('right')
                        print("right pressed")
                        keystroke_hold_time[most_common] = current_time
                        key_release_time[most_common] = current_time + 150  # Release after 100ms
                
                # Handle keystroke 3 (up key press and release after 100ms)
                elif most_common == "3":
                    if most_common not in keystroke_hold_time or current_time - keystroke_hold_time[most_common] >= 150:
                        keyboard.press('up')
                        print("up pressed")
                        keystroke_hold_time[most_common] = current_time
                        key_release_time[most_common] = current_time + 150  # Release after 100ms
                
                # Check if it's time to release any keys
                for key in list(key_release_time):
                    if current_time >= key_release_time[key]:
                        if key == "1":
                            keyboard.release('left')
                            print("left released")
                        elif key == "2":
                            keyboard.release('right')
                            print("right released")
                        elif key == "3":
                            keyboard.release('up')
                            print("up released")
                        
                        # Remove the key from tracking
                        del key_release_time[key]
                        del keystroke_hold_time[key]
                
                # Clear the buffer for the next set of samples
                buffer.clear()
    except Exception as e:
        print(e, "Move now...")
        continue
