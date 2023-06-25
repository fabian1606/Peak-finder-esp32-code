#include <Arduino.h>
#include <vector>
#include <iostream>

class SaveMac
{
private:


    static void IRAM_ATTR timer_isr();

    std::vector<clientsPerHour> Hours;
    clientsPerHour* mac();
    hw_timer_t * timer = NULL;
    
public:
    typedef struct { 
        std::vector<uint16_t> wifiMacs;
        uint32_t averageTime;
    }clientsPerHour;
    
    void init(clientsPerHour* newMac());
    SaveMac();
    ~SaveMac();

};
