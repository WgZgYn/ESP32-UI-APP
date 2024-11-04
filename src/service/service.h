////
//// Created by 20495 on 24-10-9.
////
//
#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson/Document/StaticJsonDocument.hpp>

#define LED_BUILTIN 2
#define RESET_BUTTON_PIN 0 // 按钮引脚，使用GPIO 0


class Service {
    bool light = false;
    uint8_t brightness = 0;

    const char* openLight();

    const char* closeLight();

    const char* switchLight();

    void setBrightness(uint8_t val);

public:
    static void init();

    bool callback(const char *cmd, ArduinoJson::StaticJsonDocument<128>& doc);

    static Service& getInstance() {
        static Service service;
        return service;
    }
};


#define SERVICE_H
#endif
