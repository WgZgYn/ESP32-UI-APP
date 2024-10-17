// //
// // Created by 20495 on 24-10-15.
// //
//
// #include "WebService.h"
//
//
// const char *AP_SSID = "ESP32_AP"; // ESP32的AP模式SSID
// const char *AP_PASSWORD = "12345678"; // ESP32的AP模式密码
//
//
// void WebService::handleWiFiConnection(AsyncWebServerRequest *request) {
//     if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
//         const String& selectedSSID = request->getParam("ssid")->value();
//         const String& selectedPassword = request->getParam("password")->value();
//         connectToWiFi(selectedSSID.c_str(), selectedPassword.c_str());
//         request->send(200, "text/plain", "Connecting to WiFi...");
//     } else {
//         request->send(400, "text/plain", "Missing SSID or password.");
//     }
// }
//
// void WebService::connectToWiFi(const char *ssid, const char *password) {
//     WiFi.softAPdisconnect(true);
//     WiFi.mode(WIFI_MODE_STA);
//     WiFi.begin(ssid, password);
//     Serial.print("Connecting to WiFi: ");
//     Serial.println(ssid);
//
//     int attempts = 0;
//     while (WiFi.status() != WL_CONNECTED && attempts < 20) {
//         delay(500);
//         Serial.print(".");
//         attempts++;
//     }
//
//     if (WiFi.status() == WL_CONNECTED) {
//         Serial.println("Connected!");
//     } else {
//         Serial.println("Failed to connect.");
//     }
// }
//
// void WebService::scanNetworks() {
//     int n = WiFi.scanNetworks();
//     for (int i = 0; i < n; i++) {
//         scanResults.emplace_back(
//             WiFi.SSID(i),
//             String(WiFi.RSSI(i)),
//             WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? String("Open") : String("Secured")
//         );
//     }
// }
//
// void WebService::setup() {
//     WiFi.softAP(AP_SSID, AP_PASSWORD);
//
//     scanNetworks();
//     server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
//         AsyncResponseStream *response = request->beginResponseStream("text/html");
//         Serial.println("root page");
//
//         response->print("<html><head><meta charset='UTF-8'></head><body>");
//         response->print("<h1>Select WiFi</h1>");
//         response->print("<form action='/connect' method='POST'>");
//         response->print("<label for='ssid'>SSID:</label><select name='ssid'>");
//
//         // 将扫描到的WiFi网络添加到下拉菜单
//
//         for (const auto& network: scanResults) {
//             response->printf("<option value='%s'>", network.ssid.c_str());
//             response->printf(network.ssid.c_str());
//             response->printf(" (%s)", network.type.c_str());
//             response->printf("</option>");
//         }
//
//         response->print("</select><br>");
//         response->print("<label for='password'>Password:</label><input type='password' name='password'><br>");
//         response->print("<input type='submit' value='Connect'>");
//         response->print("</form></body></html>");
//
//         request->send(response);
//     });
//
//     server.on("/connect", HTTP_POST, [this](AsyncWebServerRequest *request) {
//         Serial.println("connect wifi");
//         handleWiFiConnection(request);
//     });
//
//     server.begin();
// }
//
// void WebService::loop() {
// }
