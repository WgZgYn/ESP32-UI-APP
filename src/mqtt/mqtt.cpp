//
// Created by 20495 on 24-10-9.
//

#include "mqtt.h"
#include <wifi/config.h>


#include "MqttView.h"
#include "service/service.h"

extern "C" {
#include "freertos/timers.h"
#include "freertos/FreeRTOS.h"
}

namespace mqtt {
    const String MQTT_SERVICE = String(ESP.getEfuseMac()) + String("/service");
    const String MQTT_EVENTS = String(ESP.getEfuseMac()) + String("/events");

    auto DEVICE_BRO = "device";

    String DEVICE_INFO =
            String() +
            "efusemac=" + String(ESP.getEfuseMac()) +
            "&type:Light" +
            "&model:" + String(ESP.getChipModel())
    ;


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
        uint16_t packetIdSub = mqttClient.subscribe(MQTT_SERVICE.c_str(), 2);
        Serial.print("Subscribing at QoS 2, packetId: ");
        Serial.println(packetIdSub);

        mqttClient.publish(DEVICE_BRO, 0, true, String(ESP.getEfuseMac()).c_str());
        Serial.println("Publishing at QoS 0"); {
            mqttClient.publish("test/lol", 0, true, "test 1");
            Serial.println("Publishing at QoS 0");

            uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
            Serial.print("Publishing at QoS 1, packetId: ");
            Serial.println(packetIdPub1);

            uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
            Serial.print("Publishing at QoS 2, packetId: ");
            Serial.println(packetIdPub2);
        }
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

        if (msg.payload == "turnOn") {
            Service::run(Service::Func::Open);
        } else if (msg.payload == "turnOff") {
            Service::run(Service::Func::Close);
        } else if (msg.payload == "switch") {
            Service::run(Service::Func::Switch);
        }
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
