PROJECT FOLDER FOR ELECTRO PROJECT 2025 

Workflow:
- Download files
- Configure local arduino IDE
- Load sketches onto board for testing

Documentation for sensors and other components:

https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189

https://lastminuteengineers.com/sim800l-gsm-module-arduino-tutorial/

https://github.com/m5stack/M5Stack/blob/master/examples/Modules/SIM800L/send_short_message/send_short_message.ino

https://docs.arduino.cc/hardware/nano/

FOR SIM800L:
- TxD (green) mapped to nano digitalPin 2 (w resistor in between)
- RxD (blue) mapped to digitalPin 9
- DTR (brown) (sleep mode activate) mapped to digitalPin 4
- RST (grey) to digitalPin 7