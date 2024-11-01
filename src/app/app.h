#pragma once
#ifndef ASTRA_CORE_SRC_ASTRA_APP_H_
#define ASTRA_CORE_SRC_ASTRA_APP_H_
#include <vector>
#include <astra/ui/element/widget/widget.h>


namespace app {
    class Service {
    public:
        virtual ~Service() = default;

        virtual void setup() = 0;

        virtual void loop() = 0;
    };

    class Activity : public astra::ui::LifecycleItem {
    public:
        ~Activity() override = default;

        void onInit() override {
            // setup();
        }

        bool onOpen() override {
            setup();
            return true;
        }

        void onLoop() override {
            update();
        }

        void onExit() override {
        }

        virtual void setup() = 0;

        virtual void update() = 0;

        astra::ui::ItemType getType() const override {
            return astra::ui::ItemType::Activity;
        }
    };

    class App {
        constexpr static size_t EEPROM_SIZE = 128;

        std::vector<Service *> _service_list;
        std::vector<uint8_t> priority;

        App();

        static void HAL_INIT();

        static void SERIAL_INIT();

        static void SERVICE_INIT();

        void UI_INIT();

        static void STORE_INIT();

        static void WIFI_MODE_INIT();

        void KEY_INIT();

        void MQTT_INIT();

        void WEB_INIT();

        void PAIR_INIT();


        void add(Service *activity, uint8_t priority);

    public:

        bool ui = false;

        void setup();

        void loop();

        static App &getInstance();
    };
}


#endif //ASTRA_CORE_SRC_ASTRA_APP_H_
