////
//// Created by 20495 on 24-10-9.
////
//
#ifndef SERVICE_H
#define SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson/Document/StaticJsonDocument.hpp>
#include <mqtt/mqtt.h>



class Service {
public:
    float temperature = 0;
    float humidity = 0;

    static constexpr uint8_t model_id = 8;
    static constexpr auto model_name = "ESP-Sensor-Temperature-Humidity";
    static const String ID;

    void reportStatus() const;

    static void init();

    void callback(const ArduinoJson::StaticJsonDocument<512> &message);

    static Service &getInstance() {
        static Service service;
        return service;
    }
};


class ServiceUI : public astra::ui::LifecycleItem {
    Service &service = Service::getInstance();

public:
    void onInit() override;

    bool onOpen() override;

    void onLoop() override;

    void onExit() override;

    static ServiceUI& getInstance() {
        static ServiceUI serviceUI;
        return serviceUI;
    }
};

#define SERVICE_H
#endif
