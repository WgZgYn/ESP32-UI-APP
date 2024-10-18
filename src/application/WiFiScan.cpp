//
// Created by 20495 on 24-10-11.
//

#include "./WiFiScan.h"
#include <WiFi.h>


const char *encryptionType(const wifi_auth_mode_t encryptionType) {
    switch (encryptionType) {
        case WIFI_AUTH_OPEN:
            return "OPEN";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA_PSK";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2_PSK";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA_WPA2_PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "WPA2_ENTERPRISE";
        default:
            return "UNKNOWN";
    }
}

void WiFiScanItem::onInit() {
    HAL::cleanInfo();
    WiFi.scanNetworks(true);
    HAL::printInfo("Scanning...");
}

bool WiFiScanItem::onOpen() {
    if (!inited) {
        onInit();
        inited = true;
        finish = false;
    }
    return true;
}

void WiFiScanItem::onLoop() {
    HAL::showInfo();

    if (finish) {
        HAL::delay(50);
        return;
    }

    const int state = WiFi.scanComplete();
    switch (state) {
        case WIFI_SCAN_RUNNING:
            static uint8_t time = 0;
            if (time % 4 == 0) {
                HAL::printInfo("|");
            } else if (time % 4 == 1) {
                HAL::printInfo("/");
            } else if (time % 4 == 2) {
                HAL::printInfo("-");
            } else if (time % 4 == 3) {
                HAL::printInfo("\\");
            }
            time = (time + 1) % 4;
            HAL::delay(50);
            HAL::cancelInfo();

            break;
        case WIFI_SCAN_FAILED:
            inited = false;
            break;
        default:
            HAL::printInfo("Scan Over");
            nums = state;
            inited = false;
            finish = true;
            break;
    }
}

void WiFiScanItem::onExit() {
    inited = false;
}

const char *WiFiInfoList::ConnectTo::get_password(const uint8_t index) {
    if (WiFi.encryptionType(index) == WIFI_AUTH_OPEN) {
        return EMPTY_PASSWORD;
    }
    for (const auto &i: wifiConfig) {
        if (strcmp(i.first, WiFi.SSID(index).c_str()) == 0) {
            return i.second;
        }
    }
    return nullptr;
}

void WiFiInfoList::ConnectTo::setup() {
    // Serial.println("ConnectTo Setup");
    // if (WiFi.SSID() == WiFi.SSID(index)) {
    //     connected = true;
    //     return;
    // }
    // if (get_password(index) != nullptr) {
    //     WiFi.disconnect();
    //     WiFi.mode(WIFI_MODE_STA);
    //     WiFi.begin(WiFi.SSID(index), get_password(index));
    // }


    WiFi.begin(WiFi.SSID(index), get_password(index));
}

void WiFiInfoList::ConnectTo::update() {
    HAL::showInfo();
    if (connected) {
        HAL::delay(50);
        return;
    }
    if (WiFi.status() != WL_CONNECTED) {
        static uint8_t time = 0;
        if (time % 4 == 0) {
            HAL::printInfo("|");
        } else if (time % 4 == 1) {
            HAL::printInfo("/");
        } else if (time % 4 == 2) {
            HAL::printInfo("-");
        } else if (time % 4 == 3) {
            HAL::printInfo("\\");
        }
        time = (time + 1) % 4;
        HAL::delay(50);
        HAL::cancelInfo();
    } else {
        connected = true;
        HAL::printInfo("Connected To");
        const String ssid = WiFi.SSID();
        HAL::printInfo(ssid.c_str());
        const String ip = WiFi.localIP().toString();
        HAL::printInfo(ip);
    }
}

void WiFiInfoList::add_network_info() {
    if (!WiFiScanItem::getInstance().finish) {
        // 没有经过扫描
        add("No WiFi");
        return;
    }

    const uint8_t n = WiFiScanItem::getInstance().nums;
    for (int i = 0; i < n; i++) {
        auto *connect = new ConnectTo(i);
        connects.push_back(connect);
        add(WiFi.SSID(i).c_str(), connect);
    }
}

void WiFiInfoList::onInit() {
    add_network_info();
}

bool WiFiInfoList::onOpen() {
    // if (!inited) {
    //     onInit();
    //     inited = true;
    // }
    add_network_info();
    ListMenu::onOpen();
    return true;
}

void WiFiInfoList::onExit() {
    for (const auto connect: connects) {
        delete connect;
    }
    connects.clear();

    items.clear();
    add("No WiFI"); // avoid empty error
    // inited = false;

    ListMenu::onExit();
}

WiFiInfoList::~WiFiInfoList() {
    for (const auto connect: connects) {
        delete connect;
    }
}

WiFiScan::WiFiScan() {
    add("wifi scan", &WiFiScanItem::getInstance());
    add("wifi connect", &WiFiInfoList::getInstance());
}

void WiFiScanService::setup() {

}

void WiFiScanService::loop() {
    if (WiFi.scanComplete()) {
        scan_ok = true;
    }
}

void WiFiScanService::scan_now() {
    scan_ok = false;
    last_scan = HAL::millis();
    WiFi.scanNetworks(true);
}

bool WiFiScanService::completed() const {
    return scan_ok;
}
