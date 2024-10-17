// //
// // Created by 20495 on 24-10-15.
// //
//
// #ifndef WEBSERVICE_H
// #define WEBSERVICE_H
//
// #include <DNSServer.h>
// #include <ESPAsyncWebServer.h>
//
// #include "app/app.h"
//
// class WebService final : public app::Service {
//     struct Network {
//         String ssid, rssi, type;
//
//         Network(const String &ssid, const String &rssi, const String &type): ssid(ssid), rssi(rssi), type(type) {
//         }
//     };
//
//     AsyncWebServer server{80};
//     std::vector<Network> scanResults; // 用于保存扫描到的WiFi网络列表
//
//
//     void scanNetworks();
//
//     static void connectToWiFi(const char *ssid, const char *password);
//
//     void handleWiFiConnection(AsyncWebServerRequest *request);
//
// public:
//     void setup() override;
//
//     void loop() override;
//
//     static WebService &getInstance() {
//         static WebService instance;
//         return instance;
//     }
// };
//
//
// #endif //WEBSERVICE_H
