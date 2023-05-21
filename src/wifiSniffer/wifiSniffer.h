// #include <Arduino.h>
// #include <WiFi.h>
// #include <esp_wifi.h>
// #include <vector>
// #include <functional>
// #include <iostream>

// // inspiration from: https://github.com/DiJei/esp32-wifi-sniffer/tree/main

// class WifiSniffer{
//     private:  

//         typedef struct { // wifi packet header (when promiscuous mode is enabled)
//             unsigned frame_ctrl:16;
//             unsigned duration_id:16;
//             uint8_t addr1[6]; /* receiver address */
//             uint8_t addr2[6]; /* sender address */
//             uint8_t addr3[6]; /* filtering address */
//             unsigned sequence_ctrl:16;
//             uint8_t addr4[6]; /* optional */
//         } wifi_ieee80211_mac_hdr_t;


//         typedef struct { // wifi packet header (when promiscuous mode is enabled)
//             wifi_ieee80211_mac_hdr_t hdr;
//             uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
//         } wifi_ieee80211_packet_t;

//         typedef struct { // the format the mac adress is saved in
//             uint32_t timestamp;
//             const char macAdress [35];
//         }macAdress;

//         std::vector<macAdress> macAdresses; // vector to save the mac adresses

//         uint8_t minWifiChannel;
//         uint8_t maxWifiChannel;

//         std::function<void(void*, wifi_promiscuous_pkt_type_t)> wifiSnifferPacketHandler;

//         // static void wifiSnifferPacketHandler(void* buff, wifi_promiscuous_pkt_type_t type); // set as static to be able to use it as callback
//     public:
//         WifiSniffer();
//         WifiSniffer(uint8_t minChannel, uint8_t maxChannel);
//         ~WifiSniffer();
//         void init();
//         void setChannel(uint8_t & channel);
// };