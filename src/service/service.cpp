#include "service.h"
// #include <EEPROM.h>
//
bool Service::light = false;
//
//
// // Controller controller{};
//
// // void setup() {
// //     controller.config();
// // }
//
// // no-block
// void resetEEPROM() {
//     for (int i = 0; i < EEPROM_SIZE; i++) {
//         EEPROM.write(i, 0); // 将所有EEPROM内容置为0
//     }
//     EEPROM.commit(); // 保存更改
//     Serial.println("EEPROM已清除");
// }
//
// // void loop() {
// //     delay(100);
// //     Serial.print(".");
// //     controller.serve();
// //     if (digitalRead(RESET_BUTTON_PIN) == LOW) {
// //         Serial.println("重置按钮被按下, 清除EEPROM...");
// //         resetEEPROM();
// //         ESP.restart(); // 重启ESP32
// //     }
// // }
//
//
// // no-block
// Controller::Controller() {
//     pinMode(LED_BUILTIN, OUTPUT); // 使用蓝色LED管脚
//     pinMode(RESET_BUTTON_PIN, INPUT_PULLUP); // 按钮连接到GPIO0，低电平触发
//
//     // 串口调试
//     Serial.begin(baud);
//     Serial.println();
//
//     nvs_flash_init();
//
//     if (!EEPROM.begin(EEPROM_SIZE)) {
//         Serial.println("Failed to initialize EEPROM");
//         return;
//     }
//     loadState();
// }
//
// // write to tcpstream
// void Controller::handleInput(const String &s, WiFiClient &client) {
//     if (s.equals("open")) {
//         Service::run(Service::Func::Open);
//         client.write(State::Ok);
//     } else if (s.equals("close")) {
//         Service::run(Service::Func::Close);
//         client.write(State::Ok);
//     } else if (s.equals("switch")) {
//         Service::run(Service::Func::Switch);
//         client.write(State::Ok);
//     } else if (s.equals("ping")) {
//         client.write("pong");
//     } else if (s.equals("?deviceId")) {
//         client.write(pasw);
//     }
// }
//
// // no-block
// void Controller::loadState() {
//     host = EEPROM.readString(0);
//     ssid = EEPROM.readString(16);
//     token = EEPROM.readString(36);
//     Serial.println(host);
//     Serial.println(ssid);
//     Serial.println(token);
// }
//
// // no-block
// void Controller::saveState() const {
//     EEPROM.writeString(0, host);
//     EEPROM.writeString(16, ssid);
//     EEPROM.writeString(36, token);
//     EEPROM.commit();
// }
//
// // no-block
// void Controller::clearState() {
//     for (int i = 0; i < EEPROM_SIZE; i++) {
//         EEPROM.write(i, 0); // 将所有EEPROM内容置为0
//     }
//     EEPROM.commit(); // 保存更改
// }
//
// // no-block
// void Controller::printConfig() const {
//     Serial.print("host: ");
//     Serial.println(host);
//     Serial.print("ssid: ");
//     Serial.println(ssid);
//     Serial.print("token: ");
//     Serial.println(token);
// }
//
// // no-long-waiting
// void Controller::configAP() {
//     IPAddress local(192, 168, 4, 1);
//     IPAddress gateway(192, 168, 4, 1);
//     IPAddress subnet(255, 255, 255, 0);
//
//     WiFi.mode(WIFI_MODE_AP);
//     WiFi.softAPConfig(local, gateway, subnet);
//     WiFi.softAP(name, pasw);
//     Serial.print("password: ");
//     Serial.println(pasw);
//     server.begin();
// }
//
// // no-block
// int Controller::parseMessage(String &msg) {
//     int i = msg.indexOf('\r');
//     if (i == -1) {
//         return 0;
//     }
//     host = msg.substring(0, i);
//     msg = msg.substring(i + 1);
//     int d = msg.indexOf('\r');
//     if (d == -1) {
//         return -1;
//     }
//     ssid = msg.substring(0, d);
//     token = msg.substring(d + 1);
//
//     return 1;
// }
//
// // block until timeout
// int Controller::connectToWiFi() const {
//     constexpr static int MAX_TRY_TIMES = 10;
//     WiFi.mode(WIFI_MODE_APSTA);
//     WiFi.begin(ssid, token);
//     int count = 0;
//     while (WiFi.status() != WL_CONNECTED && count < MAX_TRY_TIMES) {
//         count += 1;
//         Serial.print(".");
//         delay(100);
//     }
//
//     // This prevents endless loop for connect to the wifi
//     if (count >= MAX_TRY_TIMES) {
//         Serial.println("WiFi connect timeout");
//         return 0;
//     }
//
//     Serial.println("Connected!");
//     Serial.println(WiFi.localIP());
//     return 1;
// }
//
// int Controller::connectToHost() const {
//     constexpr static int MAX_TRY_TIMES = 10;
//     int try_times = 0;
//     while (try_times < MAX_TRY_TIMES) {
//         try_times++;
//         WiFiClient client;
//         if (!client.connect(host.c_str(), host_port)) {
//             continue;
//         }
//         String s = client.readString();
//         if (s.equals("?deviceId")) {
//             client.write(pasw);
//             break;
//         }
//     }
//     if (try_times >= MAX_TRY_TIMES) {
//         return 0;
//     }
//     return 1;
// }
//
// void Controller::config() {
//     configAP();
//     Serial.println("open the AP, and start the server");
//     for (;;) {
//         while (host == "" || ssid == "") {
//             if (!client) Serial.println("waiting client");
//             while (!client) {
//                 Serial.print(".");
//                 delay(100);
//                 client = server.available();
//             }
//             Serial.println();
//
//
//             client.write(State::AskForConfig);
//             String msg = client.readStringUntil('\n');
//
//
//             if (parseMessage(msg) != 1) {
//                 host = "";
//                 client.write(State::MessageError);
//                 continue;
//             } else {
//                 client.write(State::Ok);
//             }
//
//
//             saveState();
//             Serial.println("host, ssid, token saved!");
//         }
//
//         printConfig();
//         Serial.println("host, ssid, token config finished!");
//
//         while (connectToWiFi() != 1) {
//             Serial.println("error on connect to wifi");
//             int code = 1;
//             if (client) {
//                 client.write(State::WiFiConnectError);
//                 code = client.read();
//             }
//             if (digitalRead(RESET_BUTTON_PIN) == LOW || code == 0) {
//                 goto Retry;
//             }
//         }
//
//         while (connectToHost() != 1) {
//             Serial.println("error on connect to host");
//             int code = 1;
//             if (client) {
//                 client.write(State::HostConnectError);
//                 code = client.read();
//             }
//             if (digitalRead(RESET_BUTTON_PIN) == LOW || code == 0) {
//                 goto Retry;
//             }
//         }
//
//         if (client) {
//             client.write(State::Success);
//         }
//         Serial.println("finish config");
//         break;
//
//     Retry:
//         clearState(); // 忘记网络
//         host = "";
//         ssid = "";
//         Serial.println("reset the wifi config manually");
//         continue;
//     }
//
//     WiFi.mode(WIFI_MODE_STA);
// }
//
// void Controller::serve() {
//     WiFiClient client = server.available();
//     if (!client)
//         return;
//
//     String s = client.readString();
//     Serial.print("[read]");
//     Serial.println(s);
//     handleInput(s, client);
// }
