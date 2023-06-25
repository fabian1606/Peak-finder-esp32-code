#include <Arduino.h>
#include <vector>
#include <iostream>

class SaveMac
{
public:
    typedef struct { 
        std::vector<uint16_t> wifiMacs;
        uint32_t averageTime;
    }clientsPerHour;
    
    void init(clientsPerHour* newMac());
    SaveMac();
    ~SaveMac();
private:
    std::vector<clientsPerHour> Hours;
    clientsPerHour* mac();
    hw_timer_t * timer = NULL;
    

};
