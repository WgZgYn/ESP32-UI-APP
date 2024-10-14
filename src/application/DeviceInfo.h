//
// Created by 20495 on 24-10-11.
//

#ifndef DEVICEINFO_H
#define DEVICEINFO_H


#include <app/app.h>
#include <Arduino.h>


class DeviceInfo : public app::Activity {
private:
    std::vector<String> info;

    DeviceInfo();

public:
    ~DeviceInfo() override = default;

    void setup() override;

    void update() override;

    static DeviceInfo &getInstance() {
        static DeviceInfo instance;
        return instance;
    }
};


#endif //DEVICEINFO_H
