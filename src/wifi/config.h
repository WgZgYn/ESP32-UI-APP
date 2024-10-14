//
// Created by 20495 on 24-10-9.
//

#ifndef CONFIG_H
#define CONFIG_H

// #define AUTO_CONNECT

#ifdef AUTO_CONNECT
#define WIFI_SSID "550W主机服务器"
#define WIFI_PASSWORD "301a301a"
#endif

#include <vector>

#define MQTT_HOST IPAddress(47, 108, 27, 238)
#define MQTT_PORT 1883


#define EMPTY_PASSWORD ""
extern std::vector<std::pair<const char *, const char *> > wifiConfig;


#endif //CONFIG_H
