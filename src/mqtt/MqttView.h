//
// Created by 20495 on 24-10-9.
//

#ifndef MQTTVIEW_H
#define MQTTVIEW_H
#include <Arduino.h>
#include <astra/ui/element/page/item.h>
#include <astra/ui/element/widget/widget.h>


namespace mqtt {
    class Message {
    public:
        String topic;
        String payload;
        uint8_t qos{};

        static Message &current() {
            static Message m;
            return m;
        }
    };

    class MqttView final : public astra::ui::Widget {
    public:
        MqttView() = default;

        ~MqttView() override = default;

        void update() override;

        static MqttView &getInstance() {
            static MqttView instance;
            return instance;
        }
    };
}


#endif //MQTTVIEW_H
