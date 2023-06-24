#include "wifiSniffer.h"

WifiSniffer::WifiSniffer()
{
    minWifiChannel = 1;
    maxWifiChannel = 13;
    macIdCounter = 0;
}

void WifiSniffer::wifiSnifferPacketHandler(void *buff, wifi_promiscuous_pkt_type_t type)
{
    if (type != WIFI_PKT_MGMT) // check if packet is management packet -> only management packets contain SSID
        return;
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr; // get the mac address of the sender of the packet (the access point)

    const wifi_pkt_rx_ctrl_t rx_ctrl = ppkt->rx_ctrl;
    const int8_t rssi = rx_ctrl.rssi;

    char mac_add[35];
    sprintf(mac_add, "%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr2[0], hdr->addr2[1], hdr->addr2[2], hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);

    bool found = false;

    WifiSniffer &objectInstance = WifiSniffer::getInstance();
    // std::vector<WifiSniffer::macAdress>* macAdresses = &(objectInstance.macAdresses);

    bool foundMac = false;

    // for (auto mac = objectInstance.macAdresses.end(); mac != objectInstance.macAdresses.begin();)
    // { // go throu the list of mac adresses
    //     if (strcmp(mac->macAdress, mac_add) == 0)
    //     {                              // check if the mac adress is already in the list
    //         mac->timestamp = millis(); // update the timestamp
    //         found = true;
    //         mac->numMessages++;
    //         if (mac->numMessages == 0)
    //         {
    //             // mac->numMessages = 0;
    //             // Serial.printf("%s : %d", mac_add, mac->macAdress);
    //             // Serial.println();
    //         }
    //     }
    //     else if (mac->timestamp + 600000 < millis())
    //     {                                          // check if the mac adress is older than 10 minutes
    //         objectInstance.macAdresses.pop_back(); // remove the mac adress from the list
    //         mac--;
    //         Serial.println("deleted mac adress");
    //     }
    //     else
    //     {
    //         // Serial.println("No new mac adress found");
    //     }
    //     if (mac == objectInstance.macAdresses.begin())
    //     {
    //         break; // Break the loop when mac reaches the beginning of the vector
    //     }
    //     else
    //     {
    //         mac--;
    //     }
    // }
    // if (!found)
    // { // if the mac adress is not in the list
    //     WifiSniffer::macAdress newMacAdress;
    //     newMacAdress.id = objectInstance.macIdCounter++;
    //     newMacAdress.timestamp = millis();
    //     newMacAdress.numMessages = 0;
    //     Serial.printf("New mac adress found: %s with Strength %d", mac_add, rssi);
    //     Serial.println();
    //     strcpy(newMacAdress.macAdress, mac_add);
    //     objectInstance.macAdresses.push_back(newMacAdress); // add the mac adress to the list
    // }
}

WifiSniffer::WifiSniffer(uint8_t minChannel, uint8_t maxChannel)
{
    minWifiChannel = minChannel;
    maxWifiChannel = maxChannel;
}

WifiSniffer::~WifiSniffer()
{
    // Destructor
}

void WifiSniffer::init(const char* ssid)
{
    wifi_country_t wifi_country = {
        // To use powerlimits in Germany
        .cc = "DE",
        .schan = minWifiChannel,
        .nchan = maxWifiChannel,
        .max_tx_power = 100,
    };
    // Init
    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_country(&wifi_country); /* set country for channel range [1, 13] */
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t ap_config;
    memset(&ap_config, 0, sizeof(ap_config));
    strcpy((char*)ap_config.ap.ssid, ssid); // Replace "YourSSID" with your desired SSID
    ap_config.ap.ssid_len = strlen(ssid);
    ap_config.ap.channel = 1; // Set the Wi-Fi channel to use
    ap_config.ap.authmode = WIFI_AUTH_OPEN; // Set the authentication mode

    esp_wifi_set_config(WIFI_IF_AP, &ap_config);

    esp_wifi_start();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifiSnifferPacketHandler);
}

void WifiSniffer::setChannel(uint8_t &channel)
{
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    channel += 1;
    if (channel > maxWifiChannel)
        channel = minWifiChannel;
}
