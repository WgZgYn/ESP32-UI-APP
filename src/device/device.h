//
// Created by 20495 on 24-10-24.
//

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

struct DeviceConfig {
private:
    constexpr int SENSOR_OFFSET = 2;

    DeviceConfig(): ID(ESP.getEfuseMac() + SENSOR_OFFSET) {

    }

public:
    static constexpr uint8_t model_id = 8;
    static constexpr auto model_name = "ESP-Sensor-Temperature-Humidity";
    const String ID;

    static DeviceConfig& getInstance() {
        static DeviceConfig instance;
        return instance;
    }
};


#endif //DEVICE_H
