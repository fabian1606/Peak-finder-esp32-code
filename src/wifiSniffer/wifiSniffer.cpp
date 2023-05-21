#include "wifiSniffer.h"

WifiSniffer::WifiSniffer(){
    minWifiChannel = 1;
    maxWifiChannel =  13;



}
void WifiSniffer::wifiSnifferPacketHandler (void* buff, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT) //check if packet is management packet -> only management packets contain SSID
        return;
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr; //get the mac address of the sender of the packet (the access point)

    char mac_add[35];
    sprintf(mac_add,"%02x:%02x:%02x:%02x:%02x:%02x", hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],hdr->addr2[3],hdr->addr2[4],hdr->addr2[5]);

    bool found = false;

    WifiSniffer objectInstance = WifiSniffer::getInstance();

    for (auto mac = objectInstance.macAdresses.begin(); mac != objectInstance.macAdresses.end();){ // go throu the list of mac adresses
        if (mac->macAdress == mac_add){ // check if the mac adress is already in the list
            mac->timestamp = millis(); // update the timestamp
            found = true;
        }
        else if (mac->timestamp + 600000 < millis()){ // check if the mac adress is older than 10 minutes
            objectInstance.macAdresses.erase(objectInstance.macAdresses.begin()); // remove the mac adress from the list
            mac++;
        }
        else{
            mac++;
        }
    }
}


WifiSniffer::WifiSniffer(uint8_t minChannel, uint8_t maxChannel){
    minWifiChannel = minChannel;
    maxWifiChannel =  maxChannel;
}

WifiSniffer::~WifiSniffer(){
    //Destructor
}

void WifiSniffer::init(){
    wifi_country_t wifi_country = { //To use powerlimits in Germany
        .cc = "DE",
        .schan = minWifiChannel,
        .nchan = maxWifiChannel,
        .max_tx_power = 100,
    };
    //Init
    tcpip_adapter_init();
    // esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_country(&wifi_country); /* set country for channel range [1, 13] */
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(wifiSnifferPacketHandler);
}

void WifiSniffer::setChannel(uint8_t & channel){
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    channel += 1;
    if (channel > maxWifiChannel) 
        channel = minWifiChannel;
}
