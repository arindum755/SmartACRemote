<p align="center">
<img src="https://github.com/arindum755/SmartACRemote/assets/137756163/caf8f55b-1694-4f73-85cc-bb788482b454.png" alt="favicon" width="100">
</p>
<h1 align="center">Smart AC Remote</h1>

Tiny ESP8266 based remote to control your home Air Conditioner when your physical remote run out of battery.

This repository contains the code for an AC remote control using an ESP8266 microcontroller. It allows you to control an AC unit remotely through a web interface.

### Hardware Requirements

To use this code, you will need the following hardware:
-	ESP8266 microcontroller (Node MCU or Wemos D1 Mini)
-	220 Ohm Resistor
-	Transistor - NPN BC337
- IR LED (950nm)
-	AC unit compatible with the ir_Coolix library 

### Libraries Used

The following Arduino libraries are used in this project:
-	Arduino
- ArduinoJson
-	ESP8266WiFi
-	WiFiClient
-	ESP8266WebServer
-	ESP8266mDNS
-	IRremoteESP8266
-	ir_Coolix

**Make sure to install these libraries in your Arduino IDE before compiling and uploading the code.**

### Setup
1.	Connect the IR LED to the appropriate GPIO pins on the ESP8266 microcontroller.
2.	Modify the ssid and password variables in the code to match your Wi-Fi network credentials.
3.	Set the remotename variable to the desired name for your AC remote.
4.	Upload the code to the ESP8266 microcontroller.

<p align="center">
<img src="https://github.com/arindum755/SmartACRemote/assets/137756163/993bae8b-1846-436d-a0ae-360cb11e69b8.png" alt="circuit" width="600">
</p>

### Usage
1.	Once the code is uploaded, the ESP8266 will connect to the Wi-Fi network and obtain an IP address.
2.	Access the remote control interface by navigating to http://<IP_ADDRESS>/index.html in a web browser, where <IP_ADDRESS> is the IP address assigned to the ESP8266.
3.	The web interface provides buttons and controls for various AC functions, including power on/off, mode selection, fan speed, temperature adjustment, and extra features like swing, step, turbo, and LED control.
4.	Clicking on the buttons will send the corresponding commands to the AC unit via IR signals.
5.	The current status of the AC (power, mode, fan speed, temperature) is displayed on the web interface.
6.	You can also retrieve the current AC data in JSON format by accessing http://<IP_ADDRESS>/getdata.

### API Endpoints
The following API endpoints are available:
-	/getdata: Returns the current AC data in JSON format.
-	/togglePower?power=<true/false>: Toggles the power state of the AC unit. Pass power=true to turn on and power=false to turn off.
-	/toggleMode?mode=<mode_value>: Changes the mode of the AC unit. Available mode values are Auto, Cool, Dry, and Fan.
-	/toggleFan?fan=<fan_value>: Changes the fan speed of the AC unit. Available fan values are 0 (Auto), 1 (Min), 2 (Medium), and 3 (Max).
-	/toggleTemp?temp=<temperature_value>: Changes the temperature of the AC unit. Provide the desired temperature value within the range of 17 to 30 degrees Celsius.
-	/toggleExtra?extra=<extra_value>: Toggles extra features of the AC unit. Available extra values are Led, Turbo, Swing, and Step.
Note: Replace <IP_ADDRESS> in the API endpoints with the actual IP address assigned to the ESP8266.
### Credits
#### This project uses the following libraries:
-	ArduinoJson: https://arduinojson.org/
-	IRremoteESP8266: https://github.com/crankyoldgit/IRremoteESP8266
-	ir_Coolix: https://github.com/z3t0/Arduino-IRremote
  
#### License
This code is released under the MIT License.
Feel free to modify and use this code according to your needs. Contributions and improvements are always welcome!
For more information and updates, visit the project repository.

