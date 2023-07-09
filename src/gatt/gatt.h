#include "NimBLEDevice.h"

#define CONFIG_BT_NIMBLE_EXT_ADV 1

#define _serviceUuid "07c0e33c-6cc2-4a68-a7df-478e334e0ed6"
#define _characteristicUuid "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool bleConnected = false;

NimBLECharacteristic *pCharacteristic;

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
    String message = "Once upon a time in the quaint village of Willowbrook, there lived a young girl named Emily. Emily was known for her wild imagination and adventurous spirit. She spent most of her days exploring the nearby forest, seeking hidden treasures and uncovering the secrets of the enchanted land. One sunny afternoon, as Emily ventured deeper into the heart of the forest, she stumbled upon a hidden pathway. It was a narrow trail, covered in vibrant flowers and shimmering with a magical aura. Curiosity sparked within Emily, and she couldn't resist the urge to follow the trail and see where it led. As she walked further along the path, the surroundings began to transform. The trees grew taller and more majestic, their branches intertwining to form intricate archways. The air filled with the sweet scent of blooming flowers, and tiny fairies flitted through the sunlight. Emily's heart danced with joy as she marveled at the breathtaking beauty around her. But little did she know that this was only the beginning of her extraordinary journey. After walking for what felt like hours, Emily reached a clearing bathed in golden light. In the center stood a magnificent, ancient oak tree, its trunk covered in symbols and intricate carvings. At the base of the tree, a small key glistened, beckoning Emily to take hold of it. With a mix of excitement and trepidation, Emily grasped the key and turned it in the lock of the oak tree. As she did, the ground trembled, and the tree split open to reveal a hidden doorway. It was an entrance to a magical realm beyond her wildest dreams. Hesitant but filled with curiosity, Emily stepped through the doorway and found herself in a land filled with wonders. Talking animals greeted her, and mystical creatures of all shapes and sizes wandered the colorful landscapes. There were towering castles made of candy, rivers of rainbow-hued water, and floating islands adorned with vibrant flowers. Amidst the enchantment, Emily discovered a forgotten village inhabited by creatures known as the Whimsicals. They were tiny, mischievous beings with brightly colored hair and laughter that echoed through the air. The Whimsicals were known for their love of games and riddles, and they welcomed Emily with open arms. In the village square, Emily encountered their leader, a wise and whimsical gnome named Ollie. Ollie explained that the magical realm had been hidden away for centuries, waiting for a special visitor like Emily to bring joy and laughter back to their lives. Eager to help, Emily embraced her role as the bringer of joy. She organized grand feasts, entertaining performances, and exciting scavenger hunts that brought laughter and unity to the Whimsical village. The once quiet village was now filled with the sounds of merriment and jubilation. Months turned into years, and Emily became an integral part of the Whimsical community. She learned their songs, listened to their stories, and shared her own tales of the human world. Together, they discovered the true power of friendship, love, and imagination. One fateful day, as Emily prepared to leave the magical realm and return home, the Whimsicals gathered to bid her farewell. Tears filled their eyes as they thanked her for the happiness she had brought into their lives. Ollie handed Emily a small golden locket, a symbol of their eternal bond. With a heavy heart, Emily stepped back through the hidden doorway, leaving behind the world of whimsy and wonder. As she emerged back into the familiar forest, she couldn't help but smile, knowing that a piece of the magical realm would forever reside in her heart. From that day forward, Emily carried the lessons she learned from the Whimsicals—about the power of imagination, the importance of laughter, and the beauty of friendship. And whenever she felt the need for a touch of magic, she would simply open the golden locket, allowing the memories of her extraordinary journey to fill her soul and inspire her adventures in the human world.";
    const char *charmessage = message.c_str();
    uint16_t messageLength = strlen(charmessage);
    // Serial.println("Sending: "+message+String(messageLength));
    uint8_t chunkSize = 18;
    if (index * chunkSize < messageLength)
    {
        if((index+1)*chunkSize > messageLength)index = 256*256-1;
        gattSendChunk(index, message.substring(index * chunkSize, index * chunkSize + chunkSize));
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
    };
};

void gattInit(const char *serviceUuid, const char *characteristicUuid)
{
    NimBLEDevice::init("ESP32_PeakFinder");
    NimBLEServer *pServer = NimBLEDevice::createServer();

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

uint32_t lastMsgSend = 0;
uint16_t indexX = 0;

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
