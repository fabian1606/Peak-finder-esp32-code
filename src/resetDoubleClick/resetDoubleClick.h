#include <Arduino.h>
#include <Preferences.h>

class ResetDoubleClick{
    public:
        ResetDoubleClick();
        bool checkDoubleClick();
    private:
        bool _reset;
        Preferences preferences;
};

ResetDoubleClick reset;

ResetDoubleClick::ResetDoubleClick(){}

bool ResetDoubleClick::checkDoubleClick(){
    preferences.begin("resetData", false);
    if(preferences.getBool("reset")){
        Serial.println("Doubleclick");
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN,HIGH);
        preferences.putBool("reset", false);
        return(true);
    }
    preferences.putBool("reset", true);
    preferences.end();

    delay(1000);
    preferences.begin("resetData", false);
    preferences.putBool("reset", false);
    preferences.end();
    return(false);
}

