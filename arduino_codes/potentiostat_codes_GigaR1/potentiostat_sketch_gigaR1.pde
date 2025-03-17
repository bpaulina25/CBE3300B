import processing.serial.*;
import java.util.ArrayList;
import java.io.PrintWriter;
import java.util.Collections;

// Serial connection
Serial myPort;

// Data storage
ArrayList<Float> VRE = new ArrayList<Float>(); // Voltage values
ArrayList<Float> IWE = new ArrayList<Float>(); // Current values

// CSV data storage
ArrayList<Float> storedValues1 = new ArrayList<Float>(); // Store all voltage data
ArrayList<Float> storedValues2 = new ArrayList<Float>(); // Store all current data

void setup() {
  size(600, 400);
  
  // Print available serial ports to the console
  // IMPORTANT: The index may change depending on your computer setup
  println(Serial.list()); // List all available ports
  
  // Select the correct port for Arduino Giga R1 WiFi
  // CHANGE: Ensure the correct port is selected before running
  String portName = Serial.list()[2]; // Adjust index if necessary
  
  // CHANGE: Reduced baud rate from 2000000 to 115200 for stability with Giga R1 WiFi
  myPort = new Serial(this, portName, 115200); 
  myPort.clear(); // Clear serial buffer at startup
  myPort.bufferUntil('\n'); // Read until newline

  background(255);
  println("Serial port connected: " + portName);
}

void draw() {
  background(255);
  drawPlot(); // Refresh plot with new data
}

void serialEvent(Serial myPort) {
  String inString = myPort.readStringUntil('\n'); // Read data from serial

  if (inString != null) {
    inString = trim(inString); // Remove spaces and newlines

    try {
      String[] parts = split(inString, ',');
      if (parts.length == 2) {
        
        // Corrected ADC to voltage conversion for Giga R1 WiFi (0-3.3V, 12-bit)
        float fdat0 = (Long.parseLong(parts[0].trim()) * 3.3) / 4095.0; // Convert to volts
        float fdat1 = ((Long.parseLong(parts[1].trim()) - 2048) * 3.3) / 4095.0; // Adjusted for current

        VRE.add(fdat0); // Store voltage
        IWE.add(fdat1); // Store current
        
        storedValues1.add(fdat0); // Store for CSV
        storedValues2.add(fdat1); // Store for CSV

        // Limit buffer size for real-time plotting efficiency
        if (VRE.size() > 400) VRE.remove(0);
        if (IWE.size() > 400) IWE.remove(0);
      } else {
        println("Invalid data format: " + inString); // Debugging print for invalid data
      }
    } catch (Exception e) {
      println("Error parsing data: " + e); // Print error for debugging
    }
  }
}

void drawPlot() {
  if (VRE.size() < 2 || IWE.size() < 2) return; // Ensure enough data for plotting

  // Copy data to avoid concurrent modification issues
  ArrayList<Float> vreCopy = new ArrayList<Float>(VRE);
  ArrayList<Float> iweCopy = new ArrayList<Float>(IWE);

  if (vreCopy.isEmpty() || iweCopy.isEmpty()) return;

  // Determine dynamic min and max for axes scaling
  float minX = Collections.min(vreCopy);
  float maxX = Collections.max(vreCopy);
  float minY = Collections.min(iweCopy);
  float maxY = Collections.max(iweCopy);

  // Add a small margin to improve visibility
  float margin = 0.1;
  minX -= margin * abs(minX);
  maxX += margin * abs(maxX);
  minY -= margin * abs(minY);
  maxY += margin * abs(maxY);

  // Draw zero lines for reference
  stroke(150);
  float zeroX = map(0, minX, maxX, 0, width);
  float zeroY = map(0, minY, maxY, height, 0);
  line(zeroX, 0, zeroX, height);
  line(0, zeroY, width, zeroY);

  // Plot IWE vs. VRE
  stroke(0);
  noFill();
  beginShape();
  for (int i = 0; i < vreCopy.size(); i++) {
    float x = map(vreCopy.get(i), minX, maxX, 0, width);
    float y = map(iweCopy.get(i), minY, maxY, height, 0);
    vertex(x, y);
  }
  endShape();
}

// Save data to a CSV file when the program closes
void exit() {
  saveData();
  super.exit();
}

void saveData() {
  ArrayList<Float> csv1 = new ArrayList<Float>(storedValues1);
  ArrayList<Float> csv2 = new ArrayList<Float>(storedValues2);

  // Generate a timestamped filename
  String timestamp = year() + nf(month(), 2) + nf(day(), 2) + "-" + nf(hour(), 2) + nf(minute(), 2) + nf(second(), 2);
  String fileName = "data-" + timestamp + ".csv";
    
  int rowCount = min(csv1.size(), csv2.size());  // Ensure arrays have the same length
  String[] rows = new String[rowCount];

  // Create each row with voltage and current values
  for (int i = 0; i < rowCount; i++) {
    rows[i] = csv1.get(i) + "," + csv2.get(i);
  }

  // Save data to CSV file
  saveStrings(fileName, rows);
  println("Data saved to " + fileName);
}
