import serial
import time

# Your local CSV file (should be in the same folder)
CSV_FILE = "Reading_to_GIGA.csv"

# Your Arduino GIGA R1 port
SERIAL_PORT = "/dev/tty.usbmodem1101"  # ← already confirmed by you

# Match the baud rate in your Arduino sketch
BAUD_RATE = 115200

def read_first_value_from_csv():
    try:
        with open(CSV_FILE, 'r') as f:
            for line in f:
                line = line.strip()
                if line:  # skip empty lines
                    return line
    except FileNotFoundError:
        print(f"❌ File not found: {CSV_FILE}")
    return None

def send_to_arduino(value):
    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2) as ser:
            time.sleep(2)  # wait for Arduino to reset if needed
            ser.write((value + '\n').encode())
            print(f"✔️ Sent to Arduino: {value}")
    except serial.SerialException as e:
        print(f"⚠️ Serial error: {e}")

def main():
    value = read_first_value_from_csv()
    if value:
        print(f"📄 CSV value: {value}")
        send_to_arduino(value)
    else:
        print("❌ No valid value found in CSV.")

if __name__ == "__main__":
    main()
