////
//// Created by 20495 on 24-10-9.
////
//
#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <ArduinoJson/Document/StaticJsonDocument.hpp>
#include <mqtt/mqtt.h>


class Service {
    uint8_t brightness = 0;

    const char *openLight();

    const char *closeLight();

    const char *switchLight();

    void setBrightness(uint8_t val);

    void lightService(const ArduinoJson::StaticJsonDocument<512> &message);

public:

    void reportStatus() const;

    static void init();

    void callback(const ArduinoJson::StaticJsonDocument<512> &message);

    static Service &getInstance() {
        static Service service;
        return service;
    }
};


#define SERVICE_H
#endif
