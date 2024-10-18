//
// Created by 20495 on 24-10-11.
//

#ifndef WIFISCAN_H
#define WIFISCAN_H
#include <WiFi.h>
#include <astra/ui/element/page/Menu.h>
#include <wifi/config.h>

#include <app/app.h>

class WiFiScanItem final : public astra::ui::LifecycleItem {
    WiFiScanItem() = default;

public:
    bool inited = false;
    bool finish = false;
    uint8_t nums = 0;

    static WiFiScanItem &getInstance() {
        static WiFiScanItem instance;
        return instance;
    }

    void onInit() override;

    bool onOpen() override;

    void onLoop() override;

    void onExit() override;
};


class WiFiInfoList final : public astra::ui::ListMenu {
    class ConnectTo final : public app::Activity {
        const uint8_t index;
        bool connected = false;

        static const char *get_password(uint8_t index);

    public:
        explicit ConnectTo(const uint8_t index) : index(index) {
        }

        void setup() override;

        void update() override;
    };

    bool inited;
    std::vector<ConnectTo *> connects;

    WiFiInfoList(): inited(false), connects(std::vector<ConnectTo *>()) {
    }

    void add_network_info();

public:
    void onInit() override;

    bool onOpen() override;

    void onExit() override;

    ~WiFiInfoList() override;

    static WiFiInfoList &getInstance() {
        static WiFiInfoList instance;
        return instance;
    }
};

class WiFiScan final : public astra::ui::ListMenu {
    WiFiScan();

    ~WiFiScan() override = default;

public:
    static WiFiScan &getInstance() {
        static WiFiScan instance;
        return instance;
    }
};

class WiFiScanService final : public app::Service {
private:
    unsigned long last_scan = 0;
    int scan_start = 0;
    int scan_time = 0;
    int scan_times = 0;

    bool scan_ok = false;

    WiFiScanService() {
    }

public:
    static WiFiScanService &getInstance() {
        static WiFiScanService instance;
        return instance;
    }


    void setup() override;

    void loop() override;

    void scan_now();
    bool completed() const;
};


#endif //WIFISCAN_H
