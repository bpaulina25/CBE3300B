#include <Arduino_USBHostMbed.h>

USBHost usb;
USBHostSerial rodeostat(usb);  // Create a USB Serial object for the ROdeostat

void setup() {
    Serial.begin(115200);  // Serial monitor for debugging
    while (!Serial);       // Wait for Serial Monitor to open

    Serial.println("Initializing USB Host...");
    usb.init();           // Start USB Host mode

    Serial.println("Waiting for ROdeostat...");
}

void loop() {
    usb.task();  // Maintain USB host communications

    if (rodeostat.connected()) {
        Serial.println("ROdeostat Detected!");
        rodeostat.println("{\"id\":1,\"method\":\"get_device_id\"}");
        delay(1000);
    }

    while (rodeostat.available()) {
        String response = rodeostat.readStringUntil('\n');
        Serial.println("ROdeostat Response: " + response);
    }
}
