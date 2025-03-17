// User settable constants
const long period = 21333;       // Total period for the triangle wave, in milliseconds
const float Vneg = -0.025;       // Minimum voltage during scan
const float Vpos = +1;           // Maximum voltage during scan
const int numReadings = 10;      // Number of analog readings, summed together
const int pwmPin = D3;           // PWM output pin (Giga R1 uses D3)
const int VREPin = A0;           // VRE input pin
const int IWEPin = A2;           // IWE input pin

// Compute internal variables
const int Rmin = 127 + (int) round(Vneg * 56);   // Minimum PWM value (0 to 255)
const int Rmax = 127 + (int) round(Vpos * 56);   // Maximum PWM value (0 to 255)
const int steps = Rmax - Rmin;                   // Total steps from Rmin to Rmax
const long halfPeriod = period / 2;              // Time for each rise or fall, in milliseconds 
const long delayTime = halfPeriod / steps;       // Delay per step, in milliseconds

void setup() {
  Serial.begin(115200);      // Start Serial Communication at a stable baud rate
  pinMode(pwmPin, OUTPUT);   // Set PWM pin as output
  pinMode(VREPin, INPUT);    // Set analog input pins
  pinMode(IWEPin, INPUT);

  // Adjust PWM resolution if needed (Giga R1 supports higher resolutions)
  analogWriteResolution(8);  // Ensure 8-bit resolution (0-255 range)
}

void loop() {
  // Increase PWM from Vmin to Vmax
  for (int pwmValue = Rmin; pwmValue <= Rmax; pwmValue++) {
    analogWrite(pwmPin, pwmValue);  // Set PWM output
    delay(delayTime);               // Control scan rate

    long VRE = 0, IWE = 0;
    for (int i = 0; i < numReadings; i++) {
       VRE += analogRead(VREPin);  
       IWE += analogRead(IWEPin);
    }

    Serial.print(VRE);
    Serial.print(", ");
    Serial.println(IWE);
  }

  // Decrease PWM from Vmax to Vmin
  for (int pwmValue = Rmax; pwmValue >= Rmin; pwmValue--) {
    analogWrite(pwmPin, pwmValue);
    delay(delayTime);

    long VRE = 0, IWE = 0;
    for (int i = 0; i < numReadings; i++) {
       VRE += analogRead(VREPin);  
       IWE += analogRead(IWEPin);
    }

    Serial.print(VRE);
    Serial.print(", ");
    Serial.println(IWE);
  }
}
