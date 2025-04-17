import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime
from potentiostat import Potentiostat
from scipy.signal import savgol_filter
import scipy.stats as stats
import serial
import time

def run_amperometry(dev, name):
    # Run chronoamperometry test
    t_all, volt_all, curr_all = dev.run_test(name, display='pbar')

    # Save full data to CSV
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    filename = f"glucose_data_{timestamp}.csv"
    df = pd.DataFrame({'time_s': t_all, 'voltage_V': volt_all, 'current_uA': curr_all})
    df.to_csv(filename, index=False)

    return t_all, volt_all, curr_all

def run_calibration(t_all, volt_all, curr_all):
    window = 50
    m = 27.5
    b = -31.2
    smooth_window = 7
    poly = 2

    volt_all = np.array(volt_all)
    curr_all = np.array(curr_all)
    t_all = np.array(t_all)

    mask = volt_all != 0
    t = t_all[15:15+window]
    curr = curr_all[mask][15:15+window]

    curr_smooth = savgol_filter(curr, window_length=smooth_window, polyorder=poly)
    t_axis = 1 / np.sqrt(t)

    slope, _, _, _, _ = stats.linregress(t_axis, curr_smooth)
    glucose_conc = m * slope + b

    return glucose_conc, slope

def show_result_gui(reading, actual_conc):
    fig_result, ax_result = plt.subplots()
    fig_result.canvas.manager.set_window_title("Glucose Reading")
    ax_result.axis('off')
    ax_result.text(0.5, 0.5,
                   f"Actual: {actual_conc:.2f} mg/dL\nEstimated: {reading:.2f} mg/dL",
                   ha='center', va='center', fontsize=16, fontweight='bold',
                   transform=ax_result.transAxes)
    plt.show()

def send_to_arduino(value, port="/dev/tty.usbmodem1101", baudrate=115200):
    try:
        with serial.Serial(port, baudrate, timeout=2) as ser:
            time.sleep(2)  # Wait for Arduino to reset
            ser.write((str(value) + '\n').encode())
            print(f"✔️ Sent to Arduino: {value}")
    except serial.SerialException as e:
        print(f"⚠️ Serial error: {e}")

if __name__ == '__main__':
    # ✅ Use correct port for your potentiostat
    dev = Potentiostat('/dev/tty.usbmodem101')
    dev.set_curr_range('1000uA')
    dev.set_sample_period(10)
    dev.set_auto_connect(True)

    name = 'chronoamp'
    test_param = {
        'quietValue': 0.0,
        'quietTime': 8000,
        'step': [(2000, 0.200)],
    }
    dev.set_param(name, test_param)

    # Run test
    t_all, volt_all, curr_all = run_amperometry(dev, name)

    # Calibrate and estimate concentration
    reading, slope = run_calibration(t_all, volt_all, curr_all)

    # Enter actual concentration for reference
    actual_conc = float(input("Enter the actual glucose concentration (mg/dL): "))
    #show_result_gui(reading, actual_conc)

    # Save values
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    df1 = pd.DataFrame({'slope': [slope], 'concentration': [reading], 'actual': [actual_conc]})
    df1.to_csv(f"slope_and_reading_data_{timestamp}.csv", index=False)

    df2 = pd.DataFrame([[reading]])
    df2.to_csv("Reading_to_GIGA.csv", index=False, header=False)

    # ✅ Send final reading to Arduino GIGA
    send_to_arduino(reading, port="/dev/tty.usbmodem1101")
