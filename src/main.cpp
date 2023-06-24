#include <Arduino.h>

#include "wifiSniffer/wifiSniffer.h" // Checks for wifi clients and saves them
#include "gatt/gatt.h" // Gatt server communiication
#include "resetDoubleClick/resetDoubleClick.h" // reset button double click
#include "configPortal/configPortal.h" //config portal for Name and service UUID
#include "config/config.h" // all the configuration data stored in EEprom

WifiSniffer& espWifiSniffer = WifiSniffer::getInstance(); // create an instance of the class globally
Gatt gatt;

void setup() {
  Serial.begin(115200);
  // if(reset.checkDoubleClick()){ // check if the reset button was double clicked
  //   ConfigPortal configPortal("ESP32_PeakFinder_Config");
  // }
  // gatt.init(config.sUuid().c_str(), "beb5483e-36e1-4688-b7f5-ea07361b26a8");
  espWifiSniffer.init(config.mountain().c_str());
}

void loop() {
}
