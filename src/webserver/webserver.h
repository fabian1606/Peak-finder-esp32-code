#include <WebServer.h>
#include <DNSServer.h>
#include <WiFi.h>

class Webserver
{
public:
    Webserver();
    void init(IPAddress * ip);
    void update();
private:
    WebServer server;
    DNSServer dnsServer;
};
