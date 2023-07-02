#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <ir_Coolix.h>
#include "index.h"            //Our HTML webpage contents
#include "favicon.h"          //Our SVG ICON contents

const uint16_t kIrLed = 4;    // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRCoolixAC ac(kIrLed);        // Set the GPIO used for sending messages.

// Your Wifi Username & Password!
const char* ssid = "********";
const char* password = "*******";

// Change this to set your Remote Name.
String remotename = "Living Room AC";

ESP8266WebServer server(80);

// Global variables for the AC
unsigned int isOn = false;        // Default AC Off
uint8_t isMode = kCoolixAuto;     // Default Mode Auto
uint8_t isFan = kCoolixFanAuto;   // Default Fan Mode Auto
uint8_t isTemp = 24;              // Default Temp 24C

// Global variables for the Remote
String rMode = "Auto";            //Auto, Cool, Dry, Fan
String rFan = "0";                //0: Auto, 1:Min, 2:Medium, 3:High

void handleRoot() {
  // Redirect to the root page with the hostname query parameter
  String redirectUrl = "/index.html?remote=" + remotename;
  server.sendHeader("Location", redirectUrl, true);
  server.send(302, "text/plain", "Redirecting...");
}


void handleIndex() {
  String s = MAIN_page;             //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

void handleIcon() {
  String i = ICON_page;                 //Read SVG ICON contents
  server.send(200, "image/svg+xml", i); //Send ICON
}

// Data sent to AC Remote
void getData() {
  DynamicJsonDocument jsonDoc(256);
  JsonObject root = jsonDoc.to<JsonObject>();

  root["isOn"] = isOn;
  root["isMode"] = String(rMode);
  root["isFan"] = String(rFan);
  root["isTemp"] = String(isTemp);

  char buffer[256];
  size_t bufferSize = serializeJson(root, buffer, sizeof(buffer));
  server.send(200, "application/json", buffer, bufferSize);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(115200);
  //Setup IR Lib
  ac.begin();                           // Initialize AC Remote
  WiFi.hostname(remotename.c_str());    // Remote Hostname
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connect to: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(remotename)) {
    Serial.println("ESP responder started");
  }

  server.on("/", handleRoot);               // Root Handler  
  server.on("/favicon.svg", handleIcon);    // Favicon  
  server.on("/index.html", handleIndex);    // Register the handleIndex function for the index.html path
  server.on("/getdata", getData);           // Get Current AC data
  server.on("/togglePower", togglePower);   // Power On/Off
  server.on("/toggleMode", toggleMode);     // Change Modes
  server.on("/toggleFan", toggleFan);       // Change Modes
  server.on("/toggleTemp", toggleTemp);     // Change Temperature
  server.on("/toggleExtra", toggleExtra);   // Change Swing, Step, Turbo, Led

  server.onNotFound(handleNotFound);
  //server.enableCORS(true);
  server.begin();
  Serial.println("HTTP server started");
}

void togglePower() {

  String message = "";
  String powerValue = server.arg("power");

  if (powerValue == "true") {
    isOn = true;
    ac.on();                             // Turn ON AC
    ac.setMode(isMode);                  // Default Auto Mode
    ac.setTemp(isTemp);                  // Default Temp 24 C
#if SEND_COOLIX
    ac.send();                           // Send the signal
#endif
  } else if (powerValue == "false") {
    isOn = false;
    ac.off();                             // Turn OFF AC
#if SEND_COOLIX
    ac.send();                            // Send the signal
#endif
  } else if (powerValue == "") {
    message = "Invalid";
  } else {
    message = "Invalid";
  }
  server.send(200, "text/plain", message);          //Returns the HTTP response
}

void toggleMode() {

  String message = "";
  String modeValue = server.arg("mode");
  // Change AC Mode
  if (modeValue == "Auto") {
    isMode = kCoolixAuto;
    rMode = "Auto";
    ac.setMode(kCoolixAuto);
#if SEND_COOLIX
    ac.send();                // Send the signal
#endif
  } else if (modeValue == "Cool") {
    isMode = kCoolixCool;
    rMode = "Cool";
    ac.setMode(kCoolixCool);
#if SEND_COOLIX
    ac.send();                // Send the signal
#endif
  }
  else if (modeValue == "Dry") {
    isMode = kCoolixDry;
    rMode = "Dry";
    ac.setMode(kCoolixDry);
#if SEND_COOLIX
    ac.send();                // Send the signal
#endif
  }
  else if (modeValue == "Fan") {
    isMode = kCoolixFan;
    rMode = "Fan";
    ac.setMode(kCoolixFan);
#if SEND_COOLIX
    ac.send();                // Send the signal
#endif
  }
  else if (modeValue == "") {
    message = "Invalid";
  } else {
    message = "Invalid";
  }
  server.send(200, "text/plain", message);          //Returns the HTTP response
}

void toggleFan() {

  String message = "";
  int fanValue = server.arg("fan").toInt();

  if (fanValue == 0 || fanValue == 1 || fanValue == 2 || fanValue == 3) {
    if (isMode != kCoolixAuto)
    {
      switch (fanValue)                // Change Fan Speed
      {
        case (0):
          isFan = kCoolixFanAuto;
          rFan = "0";
          ac.setFan(kCoolixFanAuto);
          break;
        case (1):
          isFan = kCoolixFanMin;
          rFan = "1";
          ac.setFan(kCoolixFanMin);
          break;
        case (2):
          isFan = kCoolixFanMed;
          rFan = 2;
          ac.setFan(kCoolixFanMed);
          break;
        case (3):
          isFan = kCoolixFanMax;
          rFan = "3";
          ac.setFan(kCoolixFanMax);
          break;
      }
#if SEND_COOLIX
      ac.send();                // Send the signal
#endif
    }
  } else {
    message = "Invalid";
  }
  server.send(200, "text/plain", message);          //Returns the HTTP response
}

void toggleTemp() {

  String message = "";
  int tempValue = server.arg("temp").toInt();

  if (tempValue >= 17 || tempValue <= 30) {
    isTemp = tempValue;         // Change Tepperature
    ac.setTemp(tempValue);
#if SEND_COOLIX
    ac.send();                  // Send the signal
#endif
  }
  else if (tempValue < 17) {
    message = "Invalid";
  } else {
    message = "Invalid";
  }
  server.send(200, "text/plain", message);          //Returns the HTTP response
}

void toggleExtra() {

  String message = "";
  String extraValue = server.arg("extra");
  // Change Swing, Step, Turbo, Led
  if (extraValue == "Led") {
    ac.setLed();              // Toogle LED
#if SEND_COOLIX
    ac.send();                // Send the signal
#endif
  } else if (extraValue == "Turbo") { 
    if(rMode != "Auto")
    {
      //ac.setTurbo();               // Toogle Turbo
      #if SEND_COOLIX
          ac.send(kCoolixTurbo);     // Send the signal
      #endif
    }    
  }
  else if (extraValue == "Swing") {
    ac.setSwing();            // Toogle Swing
#if SEND_COOLIX
    ac.send();                // Send the signal
#endif
  }
  else if (extraValue == "Step") {        
    ac.setSwingVStep();       // Toogle Step
#if SEND_COOLIX
    ac.send();                // Send the signal
#endif
  }
  else if (extraValue == "") {
    message = "Invalid";
  } else {
    message = "Invalid";
  }
  server.send(200, "text/plain", message);          //Returns the HTTP response
}

void loop(void) {
  server.handleClient();
}
