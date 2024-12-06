//
// Created by 20495 on 24-10-24.
//

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

struct DeviceConfig {
private:
    DeviceConfig(): ID(ESP.getEfuseMac()) {

    }

public:
    static constexpr uint8_t model_id = 2;
    static constexpr auto model_name = "ESP-Light-Dimmable";
    const String ID;

    static DeviceConfig& getInstance() {
        static DeviceConfig instance;
        return instance;
    }
};


#endif //DEVICE_H
