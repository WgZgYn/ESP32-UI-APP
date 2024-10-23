#include "app.h"

#include <EEPROM.h>
#include <application/NetworkManager.h>
#include <application/WebAuthServer.h>
#include <astra/ui/UI_VIEW.h>
#include <mqtt/mqtt.h>
#include <service/service.h>

#include "hal/myhal.hpp"

#define RESET_BUTTON_PIN 0 // 按钮引脚，使用GPIO 0

void setup() {
    app::App::getInstance().setup();
}

void loop() {
    app::App::getInstance().loop();
}


namespace app {
    App &App::getInstance() {
        static App app;
        return app;
    }

    void App::add(Service *service) {
        _service_list.push_back(service);
    }

    void App::HAL_INIT() {
        HAL::inject(&MyHAL::getInstance());

        // Below is to test function
        // HAL::delay(200);
        // HAL::printInfo("loading...");
        // HAL::delay(200);
        // HAL::printInfo("astra UI modified by wzy.");
        // HAL::delay(300);
        // HAL::printInfo("If stuck on this page,");
        // HAL::delay(300);
        // HAL::printInfo("version 2.0.0");
        // HAL::delay(300);

        Serial.println("HAL ready");
    }

    void App::SERIAL_INIT() {
        Serial.begin(115200);
        Serial.println();
        Serial.println("Serial initialized");
    }

    extern void UIConfig();

    void App::UI_INIT() {
        add(&astraLauncher);
    }

    void App::STORE_INIT() {
        nvs_flash_init(); // Used to store the Wi-Fi connect state;
        if (!EEPROM.begin(EEPROM_SIZE)) {
            Serial.println("Failed to initialize EEPROM");
        }
    }

    void App::WIFI_MODE_INIT() {
        WiFi.mode(WIFI_MODE_APSTA);
    }

    void App::KEY_INIT() {

    }

    void App::MQTT_INIT() {
        add(&mqtt::MqttClient::getInstance());
    }

    void App::SERVICE_INIT() {
        pinMode(LED_BUILTIN, OUTPUT); // 使用蓝色LED管脚
        pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
        // pinMode(RESET_BUTTON_PIN, INPUT_PULLUP); // 按钮连接到GPIO0，低电平触发
    }

    void App::WEB_INIT() {
        add(&WebAuthService::getInstance());
    }

    void App::PAIR_INIT() {
        add(&WiFiManagerService::getInstance());
    }

    App::App() {
        SERIAL_INIT();
        WIFI_MODE_INIT();
        STORE_INIT();
        KEY_INIT();
        SERVICE_INIT();
        HAL_INIT();
        UI_INIT();
        MQTT_INIT();
        WEB_INIT();
        PAIR_INIT();
    }

    void App::setup() {
        for (Service *service: this->_service_list) {
            if (service != nullptr) {
                service->setup();
            }
        }
    }

    void App::loop() {
        for (Service *service: this->_service_list) {
            if (service != nullptr) {
                service->loop();
            }
        }
    }
}
