#include "wifiSniffer.h"

WifiSniffer::WifiSniffer(){
    WifiSniffer(1, 13);
}


WifiSniffer::WifiSniffer(uint8_t minChannel, uint8_t maxChannel){
    minWifiChannel = minChannel;
    maxWifiChannel = maxChannel;
    macIdCounter = 0;

    httpServerInstance = NULL;

    testUri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = methodHandler,
            .user_ctx  = NULL,
    };
}

esp_err_t WifiSniffer:: methodHandler(httpd_req_t* httpRequest){
    ESP_LOGI("HANDLER","This is the handler for the <%s> URI", httpRequest->uri);
    return ESP_OK;
}

WifiSniffer::~WifiSniffer()
{
    // Destructor
}


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
        uint16_t numMessages = objectInstance.macAdresses[macAdressSearch - objectInstance.macAdresses.begin()].numMessages++;
        uint16_t interval = objectInstance.macAdresses[macAdressSearch - objectInstance.macAdresses.begin()].interval;
        interval = ((millis()-objectInstance.macAdresses[macAdressSearch - objectInstance.macAdresses.begin()].timestamp) + numMessages*interval)/(numMessages+1);
        // Serial.printf("Mac adress found: %s with interval %d", mac_add ,objectInstance.macAdresses[macAdressSearch - objectInstance.macAdresses.begin()].numMessages);
        // Serial.println();
        objectInstance.macAdresses[macAdressSearch - objectInstance.macAdresses.begin()].timestamp = millis();
    }
 
    else{
        WifiSniffer::macAdress newMacAdress;
        newMacAdress.id = objectInstance.macIdCounter++;
        newMacAdress.timestamp = millis();
        newMacAdress.numMessages = 0;
        newMacAdress.interval = 0;
        // std::string _possibleMac = scanPossibleMacAdress(newMacAdress.timestamp);
        //experiment to prevent wrong data when the mac is randomized
        scanPossibleMacAdress(millis(), newMacAdress.possibleMac);
        Serial.printf("New mac adress found: %s", mac_add);
        Serial.println();
        strcpy(newMacAdress.macAdress, mac_add);
        objectInstance.macAdresses.push_back(newMacAdress); // add the mac adress to the list
    }
}

void WifiSniffer::scanPossibleMacAdress(uint32_t time, const char* possibleMac){
    WifiSniffer &objectInstance = WifiSniffer::getInstance();
    // go through all mac adresses and check if the possible interval matches the given time interval sice the last timestamp of the mac adress
    for (auto it = objectInstance.macAdresses.begin(); it != objectInstance.macAdresses.end(); ++it){
        if (millis() - it->timestamp > time +120 && millis() - it->timestamp < time -120){
            possibleMac =  it->macAdress;
            Serial.printf("Possible mac adress: %s", possibleMac);
            Serial.println();
        }
    }
}



void WifiSniffer:: startHttpServer(void){
    // WifiSniffer &objectInstance = WifiSniffer::getInstance();
    // httpd_config_t httpServerConfiguration = HTTPD_DEFAULT_CONFIG();
    // httpServerConfiguration.server_port = objectInstance.serverPort;
    // if(httpd_start(&httpServerInstance, &httpServerConfiguration) == ESP_OK){
    //     ESP_ERROR_CHECK(httpd_register_uri_handler(httpServerInstance, &objectInstance.testUri));
    // }
}
     
void WifiSniffer::stopHttpServer(void){
    // if(httpServerInstance != NULL){
    //     ESP_ERROR_CHECK(httpd_stop(httpServerInstance));
    // }
}
//Handle 
void WifiSniffer::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
     if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        Serial.println("Client connected");
    } 
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        Serial.println("Client disconnected");
    }
}


IPAddress WifiSniffer::init(const char* ssid)
{
    // REG_WRITE(GPIO_OUT_W1TS_REG, BIT(GPIO_NUM_16));     // Guru Meditation Error Remediation set
    // delay(1);
    // REG_WRITE(GPIO_OUT_W1TC_REG, BIT(GPIO_NUM_16));     // Guru Meditation Error Remediation clear
    WiFi.mode(WIFI_AP); // set in Acces Point mode
    // WiFi.softAP(ssid);
    // WiFi.setHostname(ssid);

    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    tcpip_adapter_ip_info_t ipAddressInfo;
    memset(&ipAddressInfo, 0, sizeof(ipAddressInfo));
    // Set the IP address
    IP4_ADDR(&ipAddressInfo.ip,8,8,4,4); // public adress space --> to get the captive portal working with android (https://github.com/Aircoookie/WLED/issues/232)
    IP4_ADDR(&ipAddressInfo.gw,8,8,4,6);
    IP4_ADDR(&ipAddressInfo.netmask,255,255,255,0);
    ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ipAddressInfo));
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
    // Set the wifi country to a country with a legal ISM band

    wifi_country_t wifi_country = {
        // To use powerlimits in Germany
        .cc = "DE",
        .schan = minWifiChannel,
        .nchan = maxWifiChannel,
        .max_tx_power = 100,
    };
    // Init
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_country(&wifi_country); /* set country for channel range [1, 13] */
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    Serial.println("Starting Access Point...");
    esp_wifi_set_mode(WIFI_MODE_AP);

    //converting the ssid to a uint8_t array
    uint8_t ssidArray[32];
    memset(ssidArray, 0, sizeof(ssidArray));
    memcpy(ssidArray, ssid, strlen(ssid));

    //setting the access point config
    wifi_config_t ap_config;
    memset(&ap_config, 0, sizeof(ap_config));
    memcpy(ap_config.ap.ssid, ssidArray, sizeof(ap_config.ap.ssid));
    ap_config.ap.channel = 0;
    ap_config.ap.authmode = WIFI_AUTH_OPEN;
    ap_config.ap.ssid_hidden = 0;
    ap_config.ap.max_connection = 1;
    ap_config.ap.beacon_interval = 100;

    //start the acces point
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);

    // setting scan filter to only receive management packets --> better package filtering --> less packages --> less cpu usage

    wifi_promiscuous_filter_t filter;
    filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT;
    esp_wifi_set_promiscuous_filter(&filter);

    // starting packet sniffer

    esp_wifi_set_promiscuous(true); 
    esp_wifi_set_promiscuous_rx_cb(&wifiSnifferPacketHandler);

    esp_event_loop_create_default();
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

    // Start Wi-Fi
    esp_wifi_start();
    // print the soft-AP ip address
    tcpip_adapter_ip_info_t ip_info;
    ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info));
    return(ip_info.ip.addr);
}

void WifiSniffer::update(){
}

void WifiSniffer::setChannel(uint8_t &channel)
{
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    channel += 1;
    if (channel > maxWifiChannel)
        channel = minWifiChannel;
}
