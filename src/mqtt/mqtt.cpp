//
// Created by 20495 on 24-10-9.
//

#include "mqtt.h"
#include <wifi/config.h>
#include <ArduinoJson.h>
#include <device/device.h>


#include "MqttView.h"
#include "service/service.h"

extern "C" {
#include "freertos/timers.h"
#include "freertos/FreeRTOS.h"
}

namespace mqtt {
    const char *MQTT_PUBLISH_STATUS = "/status";
    const char *MQTT_PUBLISH_EVENTS = "/events";
    const char *MQTT_SUBSCRIBE_BROADCAST = "/device";

    const String MQTT_PUBLISH_SELF_EVENTS = String("/device/") + DeviceConfig::getInstance().ID + "/events";
    const String MQTT_SUBSCRIBE_SERVICE = String("/device/") + DeviceConfig::getInstance().ID + "/service";

    AsyncMqttClient mqttClient;

    TimerHandle_t mqttReconnectTimer;

#ifdef AUTO_CONNECT
    TimerHandle_t wifiReconnectTimer;

    void connectToWifi() {
        Serial.println("Connecting to Wi-Fi...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
#endif

    void connectToMqtt() {
        Serial.println("Connecting to MQTT...");
        mqttClient.connect();
    }

    void WiFiEvent(arduino_event_id_t event) {
        Serial.printf("[WiFi-event] event: %d\n", event);
        switch (event) {
            case SYSTEM_EVENT_STA_GOT_IP:
                Serial.println("WiFi connected");
                Serial.println("IP address: ");
                Serial.println(WiFi.localIP());
                connectToMqtt();
                break;
            case SYSTEM_EVENT_STA_DISCONNECTED:
                Serial.println("WiFi lost connection");
                xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
#ifdef AUTO_CONNECT
                xTimerStart(wifiReconnectTimer, 0);
#endif
                break;
            default:
                break;
        }
    }

    void onMqttConnect(bool sessionPresent) {
        Serial.println("Connected to MQTT.");

        Serial.println(MQTT_SUBSCRIBE_SERVICE);

        mqttClient.subscribe(MQTT_SUBSCRIBE_SERVICE.c_str(), 2);
        mqttClient.subscribe(MQTT_SUBSCRIBE_BROADCAST, 1);

        Service::getInstance().reportStatus();
    }

    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
        Serial.println("Disconnected from MQTT.");

        if (WiFi.isConnected()) {
            xTimerStart(mqttReconnectTimer, 0);
        }
    }

    void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
        Serial.println("Subscribe acknowledged.");
    }

    void onMqttUnsubscribe(uint16_t packetId) {
        Serial.println("Unsubscribe acknowledged.");
    }

    void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len,
                       size_t index, size_t total) {
        Serial.println("---------------------------");
        Serial.println("Publish received.");
        Serial.print("  topic: ");
        Serial.println(topic);
        Serial.print("  qos: ");
        Serial.println(properties.qos);
        Serial.print("  dup: ");
        Serial.println(properties.dup);
        Serial.print("  retain: ");
        Serial.println(properties.retain);
        Serial.print("  len: ");
        Serial.println(len);
        Serial.print("  index: ");
        Serial.println(index);
        Serial.print("  total: ");
        Serial.println(total);

        for (int i = 0; i < len; i++) {
            Serial.print(static_cast<uint8_t>(payload[i]));
        }

        const String s(payload, len);
        Message &msg = Message::current();
        msg.payload = s;
        msg.qos = properties.qos;
        msg.topic = topic;
        Serial.println("store message");


        StaticJsonDocument<512> doc;
        const DeserializationError error = deserializeJson(doc, s.c_str());
        if (error.code() == DeserializationError::Ok) {
            Service::getInstance().callback(doc);
        }
    }

    void onMqttPublish(uint16_t packetId) {
        Serial.println("Publish acknowledged.");
    }

    void publish(const char *topic, uint8_t qos, const char *payload) {
        mqttClient.publish(topic, qos, false, payload);
    }

    void MqttClient::setup() {
        Serial.begin(115200);
        Serial.println();
        Serial.println();

        mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *) 0,
                                          reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));

#ifdef AUTO_CONNECT
        wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *) 0,
                                          reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
#endif

        WiFi.onEvent(WiFiEvent);

        mqttClient
                .onConnect(onMqttConnect)
                .onDisconnect(onMqttDisconnect)
                .onSubscribe(onMqttSubscribe)
                .onUnsubscribe(onMqttUnsubscribe)
                .onMessage(onMqttMessage)
                .onPublish(onMqttPublish)
                .setServer(MQTT_HOST, MQTT_PORT);

#ifdef AUTO_CONNECT
        connectToWifi();
#endif
    }

    void MqttClient::loop() {
    }

    MqttClient &MqttClient::getInstance() {
        static MqttClient client;
        return client;
    }
}
