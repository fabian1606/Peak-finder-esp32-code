#include "gatt.h"

void Gatt::init(const char* serviceUuid, const char* characteristicUuid){
    _serviceUuid =BLEUUID(serviceUuid);
    _characteristicUuid = BLEUUID(characteristicUuid);

    NimBLEDevice::init("ESP32_PeakFinder");
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new GattCallback());

    // Create a BLE service
    pService = pServer->createService(_serviceUuid);

    // Create a BLE characteristic and set the properties
    pCharacteristic = pService->createCharacteristic(
        _characteristicUuid,
        NIMBLE_PROPERTY::READ|
        NIMBLE_PROPERTY::WRITE
    );

    // Set the initial value of the characteristic
    pCharacteristic->setValue("Hello, BLE!");

    // Start the service
    pService->start();
    // Start advertising the service
    pServer->getAdvertising()->start();
    Serial.println("Waiting for connections...");
}
