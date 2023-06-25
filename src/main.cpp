#include <Arduino.h>

#include "wifiSniffer/wifiSniffer.h" // Checks for wifi clients and saves them
#include "gatt/gatt.h" // Gatt server communiication
#include "resetDoubleClick/resetDoubleClick.h" // reset button double click
#include "configPortal/configPortal.h" //config portal for Name and service UUID
#include "config/config.h" // all the configuration data stored in EEprom
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFi.h>

WebServer server;
DNSServer dnsServer;

WifiSniffer& espWifiSniffer = WifiSniffer::getInstance(); // create an instance of the class globally
Gatt gatt;

void setup() {
  Serial.begin(115200);
  if(reset.checkDoubleClick()){ // check if the reset button was double clicked
    ConfigPortal configPortal("ESP32_PeakFinder_Config");
  }
  Serial.println(config.mountain().c_str());
  // gatt.init(config.sUuid().c_str(), "beb5483e-36e1-4688-b7f5-ea07361b26a8");
  IPAddress ip = espWifiSniffer.init(config.mountain().c_str());
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", ip);
  server.on("/", [](){
    server.send(200, "text/plain", "Hello World");
  });
  server.onNotFound([](){
    server.send(200, "text/plain", "Not found"); // Http Status 200 => Workaround to get captive portal working on android
  });
  server.begin();
}

void loop() {
  server.handleClient();
        dnsServer.processNextRequest();
}
