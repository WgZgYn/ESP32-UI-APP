//
// Created by 20495 on 24-10-24.
//

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

struct DeviceConfig {
private:
    static constexpr int AIR_CONDITION_OFFSET = 1;

    DeviceConfig(): ID(ESP.getEfuseMac() + AIR_CONDITION_OFFSET) {
    }

public:
    static constexpr uint8_t model_id = 9;
    static constexpr auto model_name = "ESP-air-condition";
    const String ID;

    static DeviceConfig &getInstance() {
        static DeviceConfig instance;
        return instance;
    }
};


#endif //DEVICE_H
