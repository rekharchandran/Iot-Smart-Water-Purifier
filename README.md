# Iot-Smart-Water-Purifier
Water Dispensing System with ESP32S3, Google Sheets Logging, and Telegram Integration 
This is a microcontroller-based project where a system for automatic water
dispensing using ESP32S3 microcontroller is designed. This system tracks the amount
of water dispensed and shows real-time updates on a Google Sheets
document. The system also maintains the health of the
filters and sends notifications via Telegram. The system has a predefined set of 250ml
of water dispensing capability with 50ml/sec. Also in case less water needed it can be
interrupted. 

## Key Features
1) Automated water dispensing based on touch sensor input.
2) Record log in Google Sheets for tracking water dispense time and ml and filter
health.
3) Transaction notifications Telegram


## Hardware Overview
The hardware used in this project is as follows:
 ESP32S3 Development Board: The core microcontroller that runs the system,WiFi,
EEPROM, and communicates with services.
Touch Sensor: It is used to initiate the water dispensing process when touched.
Buzzer: To signal the water dispensing action.
LED: To indicate whether water dispensing is in progress.


   
