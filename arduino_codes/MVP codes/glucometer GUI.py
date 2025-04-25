import matplotlib.pyplot as plt
from matplotlib.widgets import Button
import time
import threading
from glucometer import run_glucometer, get_daily_average

# Text
def update_text(msg, color='black'):
    """Status text function."""
    status_text.set_text(msg)
    status_text.set_color(color)
    fig.canvas.draw_idle()

# Buttons
def on_new_test(event):
    """Display instruction and show OK button."""
    update_text("Insert test strip and press OK")
    ok_button_ax.set_visible(True)
    
    # Recreate the OK button and reconnect handler
    global ok_button  # So it doesn't get garbage-collected
    ok_button = Button(ok_button_ax, "OK")
    ok_button.on_clicked(on_ok)

    fig.canvas.draw_idle()
    #print("New test started. OK button should be visible and clickable.")



def on_ok(event):
    """Run the glucose test."""
    #print("OK button pressed")
    ok_button_ax.set_visible(False)
    fig.canvas.draw_idle()
    threading.Thread(target=run_glucose_test).start()  # Run test in background thread

def on_history(event):
    """Display today's average glucose."""
    avg = get_daily_average()
    update_text(f"Today's Avg: {avg} mg/dL")

def on_exit(event):
    """Close the GUI window."""
    plt.close(fig)

def run_glucose_test():
    """Run the test with countdown and update the result."""
    try:
        for i in range(5, 0, -1):
            update_text(f"Starting test in {i}...")
            time.sleep(1)
        
        update_text(f"Running test")
        result = run_glucometer()
        update_text(f"{result} mg/dL")

    except Exception as e:
        update_text(f"Error: {str(e)}")


# Figure window set up
fig, ax = plt.subplots(figsize=(5, 3))
plt.subplots_adjust(bottom=0.3)
ax.axis('off')

# Main message display
status_text = ax.text(0.5, 0.5, "Ready", fontsize=14, ha='center', va='center')

# Button: New Test
new_test_ax = plt.axes([0.1, 0.05, 0.2, 0.1])
new_test_btn = Button(new_test_ax, "New Test")
new_test_btn.on_clicked(on_new_test)

# Button: History
history_ax = plt.axes([0.4, 0.05, 0.2, 0.1])
history_btn = Button(history_ax, "History")
history_btn.on_clicked(on_history)

# Button: Exit
exit_ax = plt.axes([0.7, 0.05, 0.2, 0.1])
exit_btn = Button(exit_ax, "Exit")
exit_btn.on_clicked(on_exit)

# Button: OK (initially hidden)
ok_button_ax = plt.axes([0.35, 0.18, 0.3, 0.1])
ok_button = Button(ok_button_ax, "OK")
ok_button_ax.set_visible(False)
ok_button.on_clicked(on_ok)

plt.show()
