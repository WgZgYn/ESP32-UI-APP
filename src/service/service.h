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

#define LED_BUILTIN 2
#define RESET_BUTTON_PIN 0 // 按钮引脚，使用GPIO 0


class Service {
public:
    enum WindSpeed {
        Low,
        Mid,
        High
    };

    enum Mode {
        Heating,
        Refrigeration,
        Dehumidify,
        AirSupply,
    };

    static constexpr uint8_t model_id = 9;
    static constexpr auto model_name = "ESP-air-condition";
    static const String ID;

    static constexpr float minTemp = 18;
    static constexpr float maxTemp = 30;

    float temperature = 26.0;
    int windSpeed = 0;
    int mode = 0;
    bool power = true;

    void reportStatus() const;

    void setPower(bool power);
    void setMode(int mode);
    void setWindSpeed(int speed);
    void setTemperature(float temp);

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

    static ServiceUI &getInstance() {
        static ServiceUI serviceUI;
        return serviceUI;
    }
};

#define SERVICE_H
#endif
