#include "configPortal.h"

ConfigPortal::ConfigPortal(){
    ConfigPortal("ESP_PEAKFINDER");
}
ConfigPortal::ConfigPortal(char* name):server(80){
    _name = name;
    startWifi(_name);

}

void ConfigPortal::startWifi(char* ssid){
    Serial.println("starting config portal...");
    REG_WRITE(GPIO_OUT_W1TS_REG, BIT(GPIO_NUM_16));     // Guru Meditation Error Remediation set
    delay(1);
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(GPIO_NUM_16));     // Guru Meditation Error Remediation clear
    WiFi.mode(WIFI_AP); // set in Acces Point mode
    WiFi.softAP(ssid);
    WiFi.setHostname(ssid);
    delay(500);
    const IPAddress ip = WiFi.softAPIP();
    Serial.println("Starting Wifi with ip: "+String(ip));
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", ip);
    server.on("/",[this](){
        const char* configHtml = R"pogremm(<!DOCTYPE html> <html lang="en"> <head> <meta charset="UTF-8"> <meta name="viewport" content="width=device-width, initial-scale=1.0"> <title>Configure Peak Finder</title> <style> html, body{ background-color: rgb(29, 29, 34); color: aliceblue; margin-top: 100px; font-family: 'Courier New', Courier, monospace; } header{ background-color: rgb(30, 30, 54); position: fixed; top: 0; left: 0; width: 100%; padding: 10px; } form{ margin:auto; display: flex; justify-content: space-between; align-items: left; flex-direction: column; width: 350px; } form div{ padding: 10px; display: flex; justify-content: space-between; } form button{ margin: 10px; width: 50%; } </style> </head> <body> <header> <h1>Peak Finder Config portal</h1> </header> <form action="/api/setConfig" method="get"> <div> <label for="mountain-name">Berg-Name</label> <input type="text" name="mountain" id ="mountain-name" value="%s"> </div> <div> <label for="service-uuid"> Service-UUID</label> <input type="text" name ="uuid" id="service-uuid" value="%s"> </div> <button type="submit">Ändern</button> </form> </body> </html>)pogremm";
        server.send(200, "text/html", configHtml);
    });
    server.on("/api/setConfig", [this](){
        server.send(202,"text/html","confiigurtion succesfull: closing config portal...");
        if(server.hasArg("mountain") && server.hasArg("uuid"))
            handleApi(server.arg("mountain"),server.arg("uuid"));
    });
    server.onNotFound([this](){
        server.sendHeader("Location", String("http://") + server.client().localIP().toString(), true);
        server.send(302, "text/plain", "");
    });
    server.begin();
    while(! configChanged){
        server.handleClient();
        dnsServer.processNextRequest();
    }
}

void ConfigPortal:: handleApi(String mountain, String uuid){
    configChanged = true;
    preferences.begin("config",false);
    preferences.putString("mountain",mountain);
    preferences.putString("sUuid",uuid);
    preferences.end();
}