#include "NimBLEDevice.h"

#define CONFIG_BT_NIMBLE_EXT_ADV 1

#define _serviceUuid "07c0e33c-6cc2-4a68-a7df-478e334e0ed6"
#define _characteristicUuid "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool bleConnected = false;

NimBLECharacteristic *pCharacteristic;
NimBLEServer *pServer;
uint32_t lastMsgSend = 0;
uint16_t indexX = 0;

void gattSendChunk(uint16_t index, String message)
{
    // pCharacteristic->setValue();
    if (bleConnected)
    {
        const char *charMsg = message.c_str();
        uint16_t stringLength = strlen(charMsg)+2;
        uint8_t val[stringLength];
        // set identifier
        val[0] = index >> 8;
        val[1] = index & 0xFF;

        for (uint16_t i = 2; i < stringLength; i++)
        {
            val[i] = static_cast<uint8_t>(charMsg[i-2]);
        }
        Serial.println("Sending: " + message + String(index));

        

        pCharacteristic->setValue((uint8_t *)&val, stringLength);
        pCharacteristic->notify();
        delay(3);
    }
}
void gattSendMessage(uint16_t index)
{
    Preferences preferences;
    preferences.begin("web", false);
    String users = preferences.getString("users");
    String clientsPerHour = preferences.getString("clientsHour");
    preferences.end();
    preferences.begin("config", false);
    int MountainID = (preferences.getString("sUuid")).toInt();
    String MountainName = preferences.getString("mountain");
    preferences.end();

    String message = String(String(MountainID)[0])+ "{\"a\":\""+MountainName+"\",\"arr\":["+users + String(static_cast<char>(0xFF)) + clientsPerHour;
    Serial.println(message);
    const char *charmessage = message.c_str();
    uint16_t messageLength = strlen(charmessage);
    // Serial.println("Sending: "+message+String(messageLength));
    uint8_t chunkSize = 18;
    if (index * chunkSize < messageLength)
    {
        gattSendChunk(index, message.substring(index * chunkSize, index * chunkSize + chunkSize));
    }
    else if ((index)*chunkSize > messageLength && (index-1)*chunkSize < messageLength){
        gattSendChunk(256*256-1,"");
    }
   
}

class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
    {
        Serial.printf("Client connected:: %s\n", NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        // gattSendMessage();
        bleConnected = true;
    };

    void onDisconnect(NimBLEServer *pServer)
    {
        Serial.printf("Client disconnected");
        bleConnected = false;
        indexX = 0;

    };
};

void gattInit(const char *serviceUuid, const char *characteristicUuid)
{
    NimBLEDevice::init("ESP32_PeakFinder");
    pServer = NimBLEDevice::createServer();

    // GattCallback *pServerCallbacks = new GattCallback();
    // pServerCallbacks->setCallback(new ServerCallbacks());

    pServer->setCallbacks(new ServerCallbacks());

    // Create a BLE service
    NimBLEService *pService = pServer->createService(_serviceUuid);

    // Create a BLE characteristic and set the properties BLE2902 is automaticly added to the characteristic
    pCharacteristic = pService->createCharacteristic(
        _characteristicUuid,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::NOTIFY |
            NIMBLE_PROPERTY::INDICATE);

    // Set the initial value of the characteristic
    // sendMessage("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium.Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium.");
    pCharacteristic->setValue("Hello World");
    // Start the service
    pService->start();

    // pCharacteristic->setNotifyProperty(true);
    // Start advertising the service
    pServer->getAdvertising()->start();
    Serial.println("Waiting for connections...");
}

void gattUpdate()
{

    if (bleConnected)
    {
        if (millis() > lastMsgSend+5 && indexX >0 && bleConnected){
            gattSendMessage(indexX-1);
            indexX++;
            lastMsgSend = millis();
        }
        else if (indexX == 0){
            indexX++;
            lastMsgSend = millis()+1500;
        }
    }
}
