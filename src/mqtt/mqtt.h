//
// Created by 20495 on 24-10-9.
//

#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <AsyncMqttClient.h>
#include <app/app.h>

#include <wifi/config.h>


namespace mqtt {
#ifdef AUTO_CONNECT
    void connectToWifi();
#endif
    void connectToMqtt();

    void WiFiEvent(WiFiEvent_t event);

    void onMqttConnect(bool sessionPresent);

    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);

    void onMqttSubscribe(uint16_t packetId, uint8_t qos);

    void onMqttUnsubscribe(uint16_t packetId);

    void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len,
                       size_t index, size_t total);

    void onMqttPublish(uint16_t packetId);


    class MqttClient : public app::Service {
    private:
        MqttClient() = default;

    public:
        ~MqttClient() override = default;

        void setup() override;

        void loop() override;

        static MqttClient &getInstance();
    };
}


#endif //MQTT_H
