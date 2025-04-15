#ifndef CONFIG_H

#define CONFIG_H

//Parâmetros relacionados ao OTA
#define OTA_HOSTNAME_AR "ota-hostname-device1"
#define OTA_HOSTNAME_CONTROLE "ota-hostname-device2"

// Parâmetros relacionados ao Wifi
#define NUM_NETWORKS 2 // número de wifis na memória do ESP, trocar para redes onde ele atuará

static const char* ssids[NUM_NETWORKS] = {
    "SSID 1",
    "SSID 2"
""
};

static const char* passwords[NUM_NETWORKS] = {
    "password 1",
    "password 2"
};

#endif