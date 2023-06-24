#include "gattCallback.h"   

class Gatt{
    public:
        void init(const char* serviceUuid, const char* characteristicUuid);
        void notify(String message);
    private:
        GattCallback* gattCallback;
        BLEUUID _serviceUuid;
        BLEUUID _characteristicUuid;
        BLEServer* pServer;
        BLEService* pService;
        BLECharacteristic* pCharacteristic;
    
};