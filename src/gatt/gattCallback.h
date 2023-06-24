#include <NimBLEDevice.h>
#include <Arduino.h>


class GattCallback : public BLEServerCallbacks {
   void onConnect(BLEServer* pServer) {
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client disconnected");
  }
};