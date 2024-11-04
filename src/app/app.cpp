#include "app.h"

#include <EEPROM.h>
#include <nvs_flash.h>
#include <application/NetworkManager.h>
#include <application/WebAuthServer.h>
#include <astra/ui/UI_VIEW.h>
#include <mqtt/mqtt.h>
#include <service/service.h>

#include "hal/myhal.hpp"


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

    void App::add(Service *service, uint8_t priority = 1) {
        _service_list.push_back(service);
        this->priority.push_back(priority);
    }

    void App::HAL_INIT() {
        HAL::inject(&MyHAL::getInstance());
        Serial.println("HAL ready");
    }

    void App::SERIAL_INIT() {
        Serial.begin(115200);
        Serial.println();
        Serial.println("Serial initialized");
    }

    extern void UIConfig();

    void App::UI_INIT() {
        add(&astraLauncher, 3);
        Serial.println("UI initialized");
    }

    void App::STORE_INIT() {
        nvs_flash_init(); // Used to store the Wi-Fi connect state;
        if (!EEPROM.begin(EEPROM_SIZE)) {
            Serial.println("Failed to initialize EEPROM");
        } else {
            Serial.println("EEPROM initialized");
        }
    }

    void App::WIFI_MODE_INIT() {
        WiFi.mode(WIFI_MODE_APSTA);
        Serial.println("WIFI_MODE initialized");
    }

    void App::KEY_INIT() {
        Serial.println("KEY initialized");
    }

    void App::MQTT_INIT() {
        add(&mqtt::MqttClient::getInstance());
        Serial.println("MQTT initialized");
    }

    void App::SERVICE_INIT() {
        ::Service::getInstance().init();
    }

    void App::WEB_INIT() {
        add(&WebAuthService::getInstance());
        Serial.println("WEB initialized");
    }

    void App::PAIR_INIT() {
        add(&WiFiManagerService::getInstance());
        Serial.println("PAIRING initialized");
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

    void App::loop() const {
        for (int i = 0; i < _service_list.size(); i++) {
            for(int _ = 0; _ < priority[i]; _ ++) {
                _service_list[i]->loop();
            }
        }
    }
}
