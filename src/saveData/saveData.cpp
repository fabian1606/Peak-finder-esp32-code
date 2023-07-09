#include "saveData.h"

void SaveData::addMac(){
    avCurrentMac++;
    Serial.println(avCurrentMac);
}
void SaveData::timerCallback(SaveData* currentData){
    SaveData::mac newMac; 
    newMac.numClients = currentData->avCurrentMac;
    currentData->avCurrentMac = 0;
    newMac.timestamp = millis();
    currentData->averageMac.push_back(newMac);
    Serial.println("timerCallback");
    if(currentData->averageMac.size() > 168){
        currentData->averageMac.erase(currentData->averageMac.begin());
    }
}
void SaveData::setTemp(int8_t temp){
    _temp = temp;
}
void SaveData::init(){
    avCurrentMac = 0;
    timer.attach(120, timerCallback, this); //TODO change to 3600 seconds (1 hour)
}
void SaveData::setTimeOffset(uint32_t offset){
    timeOffset = offset;
}
String SaveData::getFormattedValues(){
    String values = "";
    for(int i = 0; i< averageMac.size(); i++){
        //convert number of clients to char array
        uint16_t numClients = averageMac.at(i).numClients;
        // Serial.println("numClients: "+String(numClients));
        values += String(numClients)+",";
        // values += static_cast<char> ((numClients<<8)& 0xFF);
        // values += static_cast<char>(numClients & 0xFF);
    }
    // Serial.println("values: "+values);
    return values;

}
void SaveData::interpretFormattedValues(const char* values){
    uint16_t length = strlen(values);
    uint8_t* valuesInt = reinterpret_cast<uint8_t*>(const_cast<char*>(values));
    for (int i = 0; i< length; i+=3){
        uint16_t numClients = (valuesInt[i]<<8) | valuesInt[i+1];
        mac newMac;
        newMac.numClients = numClients;
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
