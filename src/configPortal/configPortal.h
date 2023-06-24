#include <Arduino.h>
#include <WiFi.h> 
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>


class ConfigPortal{
    public:
        ConfigPortal(char* name);
        ConfigPortal();
    private:
        WebServer server;
        DNSServer dnsServer;
        Preferences preferences;
        const byte DNS_PORT = 53;
        char* _name;
        bool configChanged = false;
        char* getConfigPortalHtml();
        void handleApi(String mountain, String uuid);
        void startWifi(char* ssid);
};