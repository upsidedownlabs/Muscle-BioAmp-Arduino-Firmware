import serial
import serial.tools.list_ports
import time

def find_responsive_port(baudrate=115200, timeout=1):
    ports = serial.tools.list_ports.comports()
    for port in ports:
        try:
            ser = serial.Serial(port.device, baudrate=baudrate, timeout=timeout)
            ser.write(b'TEST\n')  # Write a test string to the port
            time.sleep(1) # Give the device time to respond
            response = ser.readline().strip()
            if response:  # Check if the device responded
                ser.close()
                return port.device
            ser.close()
        except (OSError, serial.SerialException):
            pass
    return None