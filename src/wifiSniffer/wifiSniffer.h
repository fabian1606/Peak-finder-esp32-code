#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <DNSServer.h>
#include <vector>
#include <functional>
#include <iostream>
#include <cstring>


// inspiration from: https://github.com/DiJei/esp32-wifi-sniffer/tree/main


class WifiSniffer{
    private:  
        DNSServer dnsServer;
        const byte DNS_PORT = 53;

        WifiSniffer();
        WifiSniffer(uint8_t minChannel, uint8_t maxChannel);
        ~WifiSniffer();

        typedef struct { // wifi packet header (when promiscuous mode is enabled)
            unsigned frame_ctrl:16;
            unsigned duration_id:16;
            uint8_t addr1[6]; /* receiver address */
            uint8_t addr2[6]; /* sender address */
            uint8_t addr3[6]; /* filtering address */
            unsigned sequence_ctrl:16;
            uint8_t addr4[6]; /* optional */
        } wifi_ieee80211_mac_hdr_t;


        typedef struct { // wifi packet header (when promiscuous mode is enabled)
            wifi_ieee80211_mac_hdr_t hdr;
            uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
        } wifi_ieee80211_packet_t;



        uint8_t minWifiChannel;
        uint8_t maxWifiChannel;

        static void wifiSnifferPacketHandler (void* buff, wifi_promiscuous_pkt_type_t type); //set as static because it is called from a c function

        uint16_t macIdCounter;

    public:
        static WifiSniffer& getInstance() { // singleton pattern to ensure that only one instance of the class exists (i cant use the wifi instance in multiple objects)
            static WifiSniffer instance;
            return instance;
        }

        typedef struct { // the format the mac adress is saved in
            uint16_t id;
            uint32_t timestamp;
            char macAdress [35];
            uint16_t numMessages;
        }macAdress;

        std::vector<macAdress>macAdresses; // vector to save the mac adresses
        void init(const char* ssid);
        void setChannel(uint8_t & channel);

        void addMacAdress(const char * macAdress); // add a mac adress to the list
        void removeMacAdress(macAdress macAdress); // remove a mac adress from the list
};