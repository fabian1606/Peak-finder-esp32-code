#include <NimBLEDevice.h>
#include <Arduino.h>


class GattCallback : public BLEServerCallbacks {
  //define a callback function that gets called when a client connects/disconnects
  public:
  typedef void (*CallbackFunction)();

  CallbackFunction callback; // 

  void setCallback(void* callback){
    callback = callback;
  }
   void onConnect(BLEServer* pServer) { 
    Serial.println("Client connected");
    callback();
  }
  void onDisconnect(BLEServer* pServer) {
    Serial.println("Client disconnected");
  }
};