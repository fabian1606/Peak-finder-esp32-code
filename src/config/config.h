#include <Preferences.h>

class  Config{
    private:
        Preferences preferences;
    public:
        String sUuid();
        String mountain();
};

Config config;

String Config::sUuid(){
    preferences.begin("config", false);
    String uuid = preferences.getString("uuid");
    preferences.end();
    return(uuid);
}

String Config::mountain(){
    preferences.begin("config", false);
    String mountain = preferences.getString("mountain");
    preferences.end();
    return(mountain);
}
    

