#include <Arduino.h>
#include <vector>
#include <Preferences.h>
#include <Ticker.h>

class SaveData{
public:

    typedef struct { 
        uint16_t numClients;
        uint32_t timestamp;
        int8_t temp;
    }mac;


    void setTimeOffset(uint32_t offset);
    void addMac();
    void setTemp(int8_t temp);
    void setTemperatureCallback(void* callback);
    char* getFormattedValues();
    void init();
    static void timerCallback( SaveData* instance);
    int8_t _temp;
    std::vector<mac> averageMac;
    mac* avCurrentMac;

private:
    Ticker timer;
    void saveToEEprom ();
    void readFromEEprom();
    void interpretFormattedValues(const char* values);
    uint32_t timeOffset;
    uint16_t avNums;
};
