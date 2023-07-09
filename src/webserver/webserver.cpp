#include "webserver.h"

Webserver::Webserver() : server(80), dnsServer() {}
void Webserver::init(IPAddress *ip)
{
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    IPAddress apIP = *ip;
    dnsServer.start(53, "*", apIP);
    server.on("/loadData",[this](){
        Preferences preferences;
        preferences.begin("web", false);
        String users = preferences.getString("users");
        preferences.end();
        server.send(200, "text/json", "{\"arr\":["+users+"]}"); 
    });
    server.on("/addData", [this](){ 
        String requestBody = server.arg("plain");
        Serial.println(requestBody);
        Preferences preferences;
        preferences.begin("web", false);
        String users = preferences.getString("users");
        if(users != "") users += ",";
        users += requestBody;
        preferences.putString("users",users);
        preferences.end();
        server.send(200, "text/plain", "Request Sucessful"); 
    });
    server.on("/", [this]()
        { server.send(200, "text/html", configHtml); });

    server.onNotFound([this](){
        server.send(200, "text/html", configHtml); // Http Status 200 => Workaround to get captive portal working on android
    });
    server.begin();
}

void Webserver::update()
{
    server.handleClient();
    dnsServer.processNextRequest();
}
void Webserver::setAddMessageCallback(addMessageCallback_t *callback)
{
    addMessageCallback = callback;
}