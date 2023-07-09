#include <Arduino.h>

#include "wifiSniffer/wifiSniffer.h"           // Checks for wifi clients and saves them
#include "gatt/gatt.h"                         // Gatt server communiication
#include "resetDoubleClick/resetDoubleClick.h" // reset button double click
#include "configPortal/configPortal.h"         //config portal for Name and service UUID
#include "config/config.h"                     // all the configuration data stored in EEprom
#include "saveData/saveData.h"                 // saves the data to EEprom
#include "webserver/webserver.h"               // webserver for the website
#include "userData/userData.h"                 // stores the data from the website

WifiSniffer &espWifiSniffer = WifiSniffer::getInstance(); // create an instance of the class globally
Webserver server;                                         // define a webserver
SaveData saveData;
// UserData userData;

void setup()
{
  Serial.begin(115200);
  if (reset.checkDoubleClick())
  { // check if the reset button was double clicked
    ConfigPortal configPortal("ESP32_PeakFinder_Config");
  }
  // gatt.init(config.sUuid().c_str(), "beb5483e-36e1-4688-b7f5-ea07361b26a8");
  saveData.init();

  SaveData *localSaveData = &saveData;
  auto newMac = [&localSaveData]()
  { localSaveData->addMac(); };
  espWifiSniffer.setMacCallback(&newMac);

  IPAddress ip = espWifiSniffer.init(config.mountain().c_str()); // initialize the wifi and the webserver
  server.init(&ip);                                              // initialize the wifi and the webserver
  gattInit("07c0e33c-6cc2-4a68-a7df-478e334e0ed6", "beb5483e-36e1-4688-b7f5-ea07361b26a8");

  // server.setAddMessageCallback(reinterpret_cast<Webserver::addMessageCallback_t*>(&newMessage));
}
void newMessage (char* info ,char* email, char* userName, uint32_t timestamp ){
    // userData.addUser(info , email, userName, timestamp );
}

void loop()
{
  server.update();
  gattUpdate();
}
