This proposal outlines the foundational aspects of the glucometer development project, emphasizing both the engineering design considerations and future improvements necessary to refine the device for real-world applications.
1. Introduction
The increasing prevalence of diabetes necessitates the development of reliable, accessible, and accurate glucometers. Traditional glucometers utilize enzyme-modified electrodes for electrochemical glucose detection. This project aims to develop a low-cost, potentiostat-based glucometer prototype that employs cyclic voltammetry for glucose quantification. The system will be validated against standard glucose solutions to ensure accuracy and reproducibility, paving the way for further refinements in electrochemical biosensing technology.


2. Objectives
- Construct a potentiostat capable of performing cyclic voltammetry.
- Adapt the potentiostat for glucose detection using enzyme-modified electrodes.
- Validate the prototype by calibrating it with standard glucose solutions.
- Identify and address limitations to improve future iterations.

 3. Materials and Components
Hardware
IO Rodeostat 
Already bought
$240 
Adapters for screen printed electrodes. 
We are not screen printing electrode but we might use if we need the a better adapter for the AimsStrips
Power supply
Compatible with Adafruit ItsyBitsy M4 (Adafruit board guide)
Battery backpack (Adafruit LiIon/LiPoly Backpack) can debug w/ USB connection switch to LiIon/LiPoly when disconnected 
Display screen
ARDUINO Display
Arduino store (from EUrope) \ US but sold out (store)
Ebay $55
How to program
Display compatible with IO Rodeostat (used in their colorimetry project) $35
https://blog.iorodeo.com/open-colorimeter-electronics/
https://blog.iorodeo.com/open-colorimeter-product-guide/
I think we can use this to model our push buttons, display and such

Display Adafruit
 $24 
Uses Raspberry Pi, is this what we will be using?

Adafruit ItsyBitsy M4 board   (MICROCONTROLLER)
$15 store board 
Consider getting multiple (3) in case of failure

Calibration and Testing
- Glucose test strips from 
AimsStrip 
Amazon $15.15
1 box contains 50 strips
Contains glucose oxidase
- d-glucose stock solution (250 g/L in PBS)
- Phosphate-buffered saline (PBS) for dilutions
- Calibration concentrations: 10–200 mg/dL
- Benchmark fasting blood glucose ranges:
  - Normal (70–100 mg/dL)
  - Low (<70 mg/dL)
  - High (>125 mg/dL)

Software and Tools: 
- Arduino IDE for waveform generation and data acquisition  (???)
- Processing script for real-time data visualization
- Excel/Matlab for calibration curve fitting and data analysis

---
 4. Methodology
4.1 Calibration
- Prepare glucose solutions (10–200 mg/dL) for calibration.
- Conduct cyclic voltammetry scans, analyzing oxidation current peaks for correlation with glucose concentration.
- Discard initial unstable scans and average subsequent scans for consistency.

4.2 Testing and Validation
- Evaluate performance using glucose standards within physiologically relevant ranges.
- Compare prototype readings to known glucose concentrations to assess accuracy and linearity. Also compare to commercial glucose meter AimsStrip 
- Identify potential anomalies such as saturation at high concentrations and address underlying causes (e.g., mixing issues, electrode kinetics limitations).

4.3 Data Analysis
- Generate calibration curves based on the Randles-Sevcik equation. (???/ changing equation???)
- Analyze deviations at high glucose concentrations and adjust scan parameters accordingly.
5. Engineering Design Considerations
- Signal Stability:Noise reduction through optimized resistor selection and scan rates.
- User Interface: Develop an intuitive interface for user-friendly data retrieval and analysis.
- Power Supply: Assess battery-operated vs. external power options for portable use.

6. Concepts for Future Improvements
- Error Analysis
- Data Storage and User Profiles: Implement onboard memory to track readings over time.
- Integration with Display Modules: Consider LCD or LED screens for real-time data visualization.

7. Minimum Viable Product (MVP)
- Functional potentiostat using IO Rodeostat.
- Basic glucose detection with standard calibration.
- Python-based data output (no external display required).
- Single-user operation without long-term data storage.
- Acceptable margin of error (15%) per industry standards.

8. Main Goals
- Develop a user-friendly glucose meter.
- Achieve industry-standard error margins.
- Optimize calibration to meet ISO specifications.
- Store and analyze user data for long-term tracking.
- Incorporate date/time functionality for better monitoring.
