# Two way communication between ESP32 http server and client

![Screenshot](Screenshot.png)

Server which is the ESP32 can send data to webpage like contorl inputs and other data in XML format, while the webpage can send request and data back to server about potentially all page events.

The demo page has 3 input sliders which can be controlled on the page and by the joystick on the MCU. The submit button will send the sum of 3 inputs back to MCU to display, while this request also can be initiated by the MCU side green button. 

The functions are realized with all default Arduino libs. No additional libs are required. 

The code can be compiled by Platformio and run with Wokwi simulator (vscode ver.).