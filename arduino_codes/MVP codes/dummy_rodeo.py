import serial
import time
import random
from serial.tools import list_ports

def get_giga_port():
    for port in list_ports.comports():
        desc = port.description.lower()
        hwid = port.hwid.lower()
        if "2341:0266" in hwid or "arduino" in desc or "giga" in desc:
            return port.device
    print("Arduino GIGA not detected.")
    return None

def send_fake_reading(port, value, baudrate=115200):
    try:
        with serial.Serial(port, baudrate, timeout=2) as ser:
            time.sleep(2)  # Allow Arduino time to reset
            ser.write((str(value) + '\n').encode())
            print(f"Sent fake glucose reading: {value} mg/dL to {port}")
    except serial.SerialException as e:
        print(f"Serial communication error: {e}")

if __name__ == '__main__':
    giga_port = get_giga_port()
    if not giga_port:
        print("Plug in your Arduino GIGA and try again.")
        exit()

    # Generate a fake reading between 50 and 500 mg/dL
    fake_reading = random.randint(50, 500)
    send_fake_reading(giga_port, fake_reading)
    print(fake_reading)