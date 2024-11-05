//
// Created by 20495 on 24-10-9.
//

#include "mqtt.h"
#include <wifi/config.h>
#include <ArduinoJson.h>


#include "MqttView.h"
#include "service/service.h"

extern "C" {
#include "freertos/timers.h"
#include "freertos/FreeRTOS.h"
}

namespace mqtt {
    auto MQTT_PUBLISH_STATUS =  "/status";
    auto MQTT_PUBLISH_EVENTS =  "/events";
    const String ID = String(ESP.getEfuseMac());
    const String MQTT_PUBLISH_SELF_EVENTS = String("/device/") + ID + "/events";
    const String MQTT_SUBSCRIBE_SERVICE   = String("/device/") + ID + "/service";
    static StaticJsonDocument<128> doc;

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
        Serial.print("Session present: ");
        Serial.println(sessionPresent);

        // 不重不漏
        uint16_t packetIdSub = mqttClient.subscribe(MQTT_SUBSCRIBE_SERVICE.c_str(), 2);
        Serial.print("Subscribing at QoS 2, packetId: ");
        Serial.println(packetIdSub);

        // status message
        doc["efuse_mac"] = ID;
        doc["chip_model"] = ESP.getChipModel();
        doc["type_id"] = 1;
        Service::getInstance().callback("status", doc);

        String msg;
        serializeJson(doc, msg);
        Serial.println(msg);
        mqttClient.publish(MQTT_PUBLISH_STATUS, 1, false, msg.c_str());
        Serial.println("Publishing at QoS 1");
    }

    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
        Serial.println("Disconnected from MQTT.");

        if (WiFi.isConnected()) {
            xTimerStart(mqttReconnectTimer, 0);
        }
    }

    void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
        Serial.println("Subscribe acknowledged.");
        Serial.print("  packetId: ");
        Serial.println(packetId);
        Serial.print("  qos: ");
        Serial.println(qos);
    }

    void onMqttUnsubscribe(uint16_t packetId) {
        Serial.println("Unsubscribe acknowledged.");
        Serial.print("  packetId: ");
        Serial.println(packetId);
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
        Serial.println("  payload: ");
        Serial.println(String(payload, len));

        Message &msg = Message::current();
        msg.payload = String(payload, len);
        msg.qos = properties.qos;
        msg.topic = topic;
        Serial.println("store message");


        doc["type"] = "event";
        const bool ok = Service::getInstance().callback(msg.payload.c_str(), doc);
        if (!ok) {
            Serial.println("can't handle the error message");
            return;
        }

        String temp;
        serializeJson(doc, temp);
        Serial.println(temp);

        // event message
        mqttClient.publish(MQTT_PUBLISH_EVENTS, 2, false, temp.c_str());
        mqttClient.publish(MQTT_PUBLISH_SELF_EVENTS.c_str(), 2, false, temp.c_str());
    }

    void onMqttPublish(uint16_t packetId) {
        Serial.println("Publish acknowledged.");
        Serial.print("  packetId: ");
        Serial.println(packetId);
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
