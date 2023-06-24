#include "wifiSniffer.h"

WifiSniffer::WifiSniffer()
{
    minWifiChannel = 1;
    maxWifiChannel = 13;
    macIdCounter = 0;
}

int count = 0;

void WifiSniffer::wifiSnifferPacketHandler(void *buff, wifi_promiscuous_pkt_type_t type){
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr; // get the mac address of the sender of the packet (the access point)

    char mac_add[35];
    sprintf(mac_add, "%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr2[0], hdr->addr2[1], hdr->addr2[2], hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);

    bool found = false;

    WifiSniffer &objectInstance = WifiSniffer::getInstance();

    // delete all mac adresses that have not been seen in the last 10 minutes
    for (auto it = objectInstance.macAdresses.begin(); it != objectInstance.macAdresses.end();){
        if (millis() - it->timestamp > 600000){ 
            Serial.printf("Deleted mac adress: %s", it->macAdress);
            Serial.println();
            objectInstance.macAdresses.erase(it);
        }
        else{
            ++it;
        }
    }

    // check if the mac adress is already in the list
    auto macAdressSearch = std::find_if(objectInstance.macAdresses.begin(), objectInstance.macAdresses.end(), [mac_add](const WifiSniffer::macAdress & obj) {
        return strcmp( obj.macAdress, mac_add) == 0;
    });
    if (macAdressSearch != objectInstance.macAdresses.end()){ // if the mac adress is in the list
        objectInstance.macAdresses[macAdressSearch - objectInstance.macAdresses.begin()].timestamp = millis();
    }
    else{
        WifiSniffer::macAdress newMacAdress;
        newMacAdress.id = objectInstance.macIdCounter++;
        newMacAdress.timestamp = millis();
        newMacAdress.numMessages = 0;
        Serial.printf("New mac adress found: %s", mac_add);
        Serial.println();
        strcpy(newMacAdress.macAdress, mac_add);
        objectInstance.macAdresses.push_back(newMacAdress); // add the mac adress to the list
    }
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
    Serial.println("Starting Access Point...");
    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t ap_config;
    memset(&ap_config, 0, sizeof(ap_config));
    strcpy((char*)ap_config.ap.ssid, ssid); // Replace "YourSSID" with your desired SSID
    ap_config.ap.ssid_len = strlen(ssid);
    ap_config.ap.channel = 1; // Set the Wi-Fi channel to use
    ap_config.ap.authmode = WIFI_AUTH_OPEN; // Set the authentication mode

    esp_wifi_set_config(WIFI_IF_AP, &ap_config);

    esp_wifi_start();

    // setting scan filter to only receive management packets --> better package filtering --> less packages --> less cpu usage

    wifi_promiscuous_filter_t filter;
    filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT;
    esp_wifi_set_promiscuous_filter(&filter);

    // starting packet sniffer

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
