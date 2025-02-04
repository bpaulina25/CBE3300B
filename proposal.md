This proposal outlines the foundational aspects of the glucometer development project, emphasizing both the engineering design considerations and future improvements necessary to refine the device for real-world applications.

1\. Introduction

The increasing prevalence of diabetes necessitates the development of reliable, accessible, and accurate glucometers. Traditional glucometers utilize enzyme-modified electrodes for electrochemical glucose detection. This project aims to develop a low-cost, potentiostat-based glucometer prototype that employs cyclic voltammetry for glucose quantification. The system will be validated against standard glucose solutions to ensure accuracy and reproducibility, paving the way for further refinements in electrochemical biosensing technology.

2\. Objectives

\- Construct a potentiostat capable of performing cyclic voltammetry.

\- Adapt the potentiostat for glucose detection using enzyme-modified electrodes.

\- Validate the prototype by calibrating it with standard glucose solutions.

\- Identify and address limitations to improve future iterations.

 3\. Materials and Components

Hardware

- IO Rodeostat   
  - Already bought  
    - $240   
  - [Adapters](https://blog.iorodeo.com/zp_hypervalue_rodeostat_adapter/) for screen printed electrodes.   
    - We are not screen printing electrode but we might use if we need the a better adapter for the AimsStrips  
- Power supply  
  - Compatible with Adafruit ItsyBitsy M4 ([Adafruit board guide](https://www.adafruit.com/product/2124))  
    - Battery backpack ([Adafruit LiIon/LiPoly Backpack](https://www.adafruit.com/product/2124)) can debug w/ USB connection switch to LiIon/LiPoly when disconnected   
- Display screen  
  - ARDUINO Display  
    - [Arduino store](https://store.arduino.cc/products/giga-display-shield) (from EUrope) \\ US but sold out ([store](https://store-usa.arduino.cc/products/giga-display-shield?srsltid=AfmBOora69Mw4g2RHMiKeSAv3hRBOK-h7JVJqsESe7WxWg0iiusILrex))  
    - [Ebay $5](https://www.ebay.com/itm/156329309170?chn=ps&norover=1&mkevt=1&mkrid=711-117182-37290-0&mkcid=2&mkscid=101&itemid=156329309170&targetid=2299003535995&device=c&mktype=pla&googleloc=9007284&poi=&campaignid=21666443208&mkgroupid=167896472878&rlsatarget=pla-2299003535995&abcId=10012311&merchantid=6296724&gad_source=1&gbraid=0AAAAA-Cg_1zVCoLleR-aEY0VYqsR-j85v&gclid=CjwKCAiA74G9BhAEEiwA8kNfpcKfLxpF-kY5LGDC0xjP4tNpqQWRsTEz6RpthrytIaQBDSKrlGc-1BoCURgQAvD_BwE)5  
    - [How to program](https://github.com/upiir/arduino_giga_display_shield)  
  - [Display compatible](https://www.adafruit.com/product/4200?ref=blog.iorodeo.com) with IO Rodeostat (used in their colorimetry project) $35  
    - [https://blog.iorodeo.com/open-colorimeter-electronics/](https://blog.iorodeo.com/open-colorimeter-electronics/)  
    - [https://blog.iorodeo.com/open-colorimeter-product-guide/](https://blog.iorodeo.com/open-colorimeter-product-guide/)  
    - I think we can use this to model our push buttons, display and such  
        
  - [Display](https://www.digikey.com/en/products/detail/adafruit-industries-llc/3531/7364307?gclsrc=aw.ds&&utm_adgroup=&utm_source=google&utm_medium=cpc&utm_campaign=PMax%20Shopping_Product_Low%20ROAS%20Categories&utm_term=&utm_content=&utm_id=go_cmp-20243063506_adg-_ad-__dev-c_ext-_prd-7364307_sig-CjwKCAiA74G9BhAEEiwA8kNfpSa4Qji7JQv933020VsnIYbtfgIzBMVqvubuIxEIZ7mGGz5tpyO1EhoC94gQAvD_BwE&gad_source=1&gbraid=0AAAAADrbLljI22TKZO2za_hkYnyMsOVRN&gclid=CjwKCAiA74G9BhAEEiwA8kNfpSa4Qji7JQv933020VsnIYbtfgIzBMVqvubuIxEIZ7mGGz5tpyO1EhoC94gQAvD_BwE) Adafruit  
    -  $24   
      - Uses Raspberry Pi, is this what we will be using?

- Adafruit ItsyBitsy M4 board   (MICROCONTROLLER)  
  - [$15 store](https://www.adafruit.com/product/3800) board   
    - Consider getting multiple (3) in case of failure

Calibration and Testing

\- Glucose test strips from 

- [AimsStrip](https://www.germainelabs.com/product/aimstrip-plus-blood-glucose-test-strips-50-box/)   
  - [Amazon](https://www.amazon.com/AimStrip-37350-Blood-Glucose-Strips/dp/B07J5P9GXG/ref=sr_1_2?crid=35QOW45TUZMUJ&dib=eyJ2IjoiMSJ9.nngM-nAf7sRlZFrCfUlZoMifXLu3TE26eGM0AsQGB6HGjHj071QN20LucGBJIEps.sA6gZoKjp6aymXk3Ys0SuC450B8lfsJ3KeheNEtO138&dib_tag=se&keywords=AimsStrip&qid=1738610982&s=industrial&sprefix=aimsstrip+%2Cindustrial%2C95&sr=1-2) $15.15  
  - 1 box contains 50 strips  
  - Contains **glucose oxidase**

\- d-glucose stock solution (250 g/L in PBS)

\- Phosphate-buffered saline (PBS) for dilutions

\- Calibration concentrations: 10–200 mg/dL

\- Benchmark fasting blood glucose ranges:

  \- Normal (70–100 mg/dL)

  \- Low (\<70 mg/dL)

  \- High (\>125 mg/dL)

Software and Tools: 

\- Arduino IDE for waveform generation and data acquisition  (???)

\- Processing script for real-time data visualization

\- Excel/Matlab for calibration curve fitting and data analysis

\---

 4\. Methodology

4.1 Calibration

\- Prepare glucose solutions (10–200 mg/dL) for calibration.

\- Conduct cyclic voltammetry scans, analyzing oxidation current peaks for correlation with glucose concentration.

\- Discard initial unstable scans and average subsequent scans for consistency.

4.2 Testing and Validation

\- Evaluate performance using glucose standards within physiologically relevant ranges.

\- Compare prototype readings to known glucose concentrations to assess accuracy and linearity. Also compare to commercial glucose meter AimsStrip 

\- Identify potential anomalies such as saturation at high concentrations and address underlying causes (e.g., mixing issues, electrode kinetics limitations).

4.3 Data Analysis

\- Generate calibration curves based on the Randles-Sevcik equation. (???/ changing equation???)

\- Analyze deviations at high glucose concentrations and adjust scan parameters accordingly.

5\. Engineering Design Considerations

\- Signal Stability:Noise reduction through optimized resistor selection and scan rates.

\- User Interface: Develop an intuitive interface for user-friendly data retrieval and analysis.

\- Power Supply: Assess battery-operated vs. external power options for portable use.

6\. Concepts for Future Improvements

\- Error Analysis

\- Data Storage and User Profiles: Implement onboard memory to track readings over time.

\- Integration with Display Modules: Consider LCD or LED screens for real-time data visualization.

7\. Minimum Viable Product (MVP)

\- Functional potentiostat using IO Rodeostat.

\- Basic glucose detection with standard calibration.

\- Python-based data output (no external display required).

\- Single-user operation without long-term data storage.

\- Acceptable margin of error (15%) per industry standards.

8\. Main Goals

\- Develop a user-friendly glucose meter.

\- Achieve industry-standard error margins.

\- Optimize calibration to meet ISO specifications.

\- Store and analyze user data for long-term tracking.

\- Incorporate date/time functionality for better monitoring.

