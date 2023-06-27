#include "saveData.h"

void SaveData::addMac(){
    avCurrentMac->numClients++;
}
void SaveData::timerCallback(SaveData* currentData){
    SaveData::mac newMac; 
    newMac.numClients = 0;
    newMac.timestamp = millis();
    newMac.temp = currentData->_temp;
    const uint8_t avNums = currentData-> avNums % 168;
    uint16_t avSize = currentData->averageMac.size();
    
    if(avSize < avNums){
        currentData->averageMac.push_back(newMac);
        currentData->avCurrentMac = &currentData->averageMac.at(-1);
    }
    else{
        currentData->avCurrentMac = &currentData->averageMac[avNums];
    }
}
void SaveData::setTemp(int8_t temp){
    _temp = temp;
}
void SaveData::init(){
    timer.attach(3600, timerCallback, this);
}
void SaveData::setTimeOffset(uint32_t offset){
    timeOffset = offset;
}
char* SaveData::getFormattedValues(){
    std::vector<uint8_t> values;
    for(auto it = averageMac.begin(); it != averageMac.end(); ++it){
        uint16_t numClients = it->numClients;
        values.push_back((numClients<<8)& 0xFF);
        values.push_back(numClients & 0xFF);
        values.push_back(it->temp);
    }
    values.push_back(123);
    return(reinterpret_cast<char*>(values.data()));
}
void SaveData::interpretFormattedValues(const char* values){
    uint16_t length = strlen(values);
    uint8_t* valuesInt = reinterpret_cast<uint8_t*>(const_cast<char*>(values));
    for (int i = 0; i< length; i+=3){
        uint16_t numClients = (valuesInt[i]<<8) | valuesInt[i+1];
        uint8_t temp = valuesInt[i+2];
        mac newMac;
        newMac.numClients = numClients;
        newMac.temp = temp;
        averageMac.push_back(newMac);
    }
}

void SaveData::saveToEEprom(){
    Preferences preferences;
    preferences.begin("data", false);
    preferences.putString("averageVal",getFormattedValues());
    preferences.putUInt("avNums", avNums);
    preferences.end();
}
void SaveData::readFromEEprom(){
    Preferences preferences;
    preferences.begin("data", false);
    String averageVal = preferences.getString("averageVal");
    avNums = preferences.getUInt("avNums");
    preferences.end();
    interpretFormattedValues(averageVal.c_str());
}
