#include "webserver.h"

Webserver::Webserver() : server(80), dnsServer(){}
void Webserver::init(IPAddress *ip)
{
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    IPAddress apIP = *ip;
    dnsServer.start(53, "*",apIP);
    server.on("/", [this]()
              { server.send(200, "text/plain", "Hello World"); });
    server.onNotFound([this]()
                      {
                          server.send(200, "text/plain", "Not found"); // Http Status 200 => Workaround to get captive portal working on android
                      });
    server.begin();
}

void Webserver::update()
{
    server.handleClient();
    dnsServer.processNextRequest();
}