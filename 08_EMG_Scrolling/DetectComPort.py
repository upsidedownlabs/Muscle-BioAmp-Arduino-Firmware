import serial
import serial.tools.list_ports
import time

def find_responsive_port(baudrate=115200, timeout=1):
    ports = serial.tools.list_ports.comports()

    for port in ports:
        description = (port.description or "").lower()
        hwid = (port.hwid or "").lower()
        # Skip Bluetooth ports
        if "bluetooth" in description or "bth" in hwid:
            continue
        # Skip virtual ports
        if ("virtual" in description or
            "vsp" in description or
            "com0com" in description or
            "emulator" in description):
            continue
        # Only allow USB/Serial devices
        if ("usb" not in description and
            "vid:pid" not in hwid and
            "usb" not in hwid):
            continue
        print(f"Checking {port.device} ({port.description})")
        try:
            ser = serial.Serial(port.device,baudrate=baudrate,timeout=timeout)
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            ser.write(b'TEST\n')
            time.sleep(1)
            response = ser.readline().strip()
            ser.close()
            if response:
                print(f"Found responsive device on {port.device}")
                return port.device
        except (OSError, serial.SerialException):
            pass

    return None