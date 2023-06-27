#include <Arduino.h>

#include "wifiSniffer/wifiSniffer.h" // Checks for wifi clients and saves them
#include "gatt/gatt.h" // Gatt server communiication
#include "resetDoubleClick/resetDoubleClick.h" // reset button double click
#include "configPortal/configPortal.h" //config portal for Name and service UUID
#include "config/config.h" // all the configuration data stored in EEprom
#include "saveData/saveData.h" // saves the data to EEprom
#include "webserver/webserver.h" // webserver for the website

WifiSniffer& espWifiSniffer = WifiSniffer::getInstance(); // create an instance of the class globally
Webserver server; //define a webserver 
Gatt gatt;
SaveData saveData;

void setup() {
  Serial.begin(115200);
  if(reset.checkDoubleClick()){ // check if the reset button was double clicked
    ConfigPortal configPortal("ESP32_PeakFinder_Config");
  }
  // gatt.init(config.sUuid().c_str(), "beb5483e-36e1-4688-b7f5-ea07361b26a8");
  saveData.init();

  SaveData* localSaveData = &saveData;
  auto newMac = [&localSaveData]() {localSaveData->addMac();};
  espWifiSniffer.setMacCallback(&newMac);

  IPAddress ip = espWifiSniffer.init(config.mountain().c_str()); // initialize the wifi and the webserver
  server.init(&ip); // initialize the wifi and the webserver
}

void loop() {
  server.update();
}
