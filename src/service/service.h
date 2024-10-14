////
//// Created by 20495 on 24-10-9.
////
//
#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <nvs_flash.h>
//
//
//#define EEPROM_SIZE 512    // EEPROM大小 (根据需要设置)
//#define COUNTER_ADDR 0     // 数据存储的起始地址
//#define RESET_BUTTON_PIN 0 // 按钮引脚，使用GPIO 0
//
//// const char *name = "MECHREV 9674";
//// const char *pawd = "12345678";
//
//// const char *name = "550W主机服务器";
//// const char *pawd = "301a301a";
//
//
//constexpr static int host_port = 45677;
//constexpr static int serv_port = 45678;
//
//constexpr static unsigned long baud = 115200;
//const static uint64_t ID = ESP.getEfuseMac(); // 162756773954388 1.
//const String uuid = String(ESP.getEfuseMac());
//
//const char *name = "ESP32";
//const char *pasw = uuid.c_str();
//
//enum State {
//    Ok,
//    AskForConfig,
//    MessageError,
//    WiFiConnectError,
//    HostConnectError,
//    Success,
//};
//
//
//class Controller {
//    WiFiServer server{serv_port};
//
//    String host; // 16
//    String ssid; // 20
//    String token; // 20
//
//    WiFiClient client;
//    bool light = false; // TODO: store the light status
//
//public:
//    Controller();
//
//    static void handleInput(const String &s, WiFiClient &client);
//
//    void loadState();
//
//    void saveState() const;
//
//    static void clearState();
//
//    void printConfig() const;
//
//    void configAP();
//
//    // First use it will stuck here to make a bind with the center host
//    int parseMessage(String &msg);
//
//    int connectToWiFi() const;
//
//    int connectToHost() const;
//
//    void config();
//
//    void serve();
//};
//
//
class Service {
    static bool light;

    static void openLight() {
        digitalWrite(LED_BUILTIN, HIGH);
        light = true;
    }

    static void closeLight() {
        digitalWrite(LED_BUILTIN, LOW);
        light = false;
    }

    static void switchLight() {
        light ? closeLight() : openLight();
    }

public:
    enum class Func {
        Open,
        Close,
        Switch,
    };

    static void run(const Func &f) {
        switch (f) {
            case Func::Open:
                openLight();
                break;
            case Func::Close:
                closeLight();
                break;
            case Func::Switch:
                switchLight();
                break;
            default:
                break;
        }
    }
};


#define SERVICE_H
#endif
