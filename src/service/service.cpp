#include "service.h"
#include <ArduinoJson.h>
#include <device/device.h>
#include <hal/pins.h>
#include <utils/utils.h>


StaticJsonDocument<512> newJsonDocument() {
    StaticJsonDocument<512> doc;
    doc["efuse_mac"] = DeviceConfig::getInstance().ID;
    doc["model_id"] = DeviceConfig::model_id;
    doc["model_name"] = DeviceConfig::model_name;
    return doc;
}

const char *Service::openLight() {
    brightness = 255;
    analogWrite(LED_BUILTIN, 255);
    return "open";
}

const char *Service::closeLight() {
    brightness = 0;
    analogWrite(LED_BUILTIN, 0);
    return "close";
}

const char *Service::switchLight() {
    return brightness > 0 ? closeLight() : openLight();
}

void Service::setBrightness(const uint8_t val) {
    float scaledVal = val * 2.56;

    // 确保 scaledVal 在 0 到 255 之间
    if (scaledVal > 255) scaledVal = 255;
    if (scaledVal < 0) scaledVal = 0;

    // 将浮点数转换为 uint8_t
    const auto targetBrightness = static_cast<uint8_t>(scaledVal);


    Serial.print("from ");
    Serial.print(brightness);
    Serial.print(" to ");
    Serial.println(targetBrightness);

    // avoid divide 0
    if (brightness == targetBrightness) {
        return;
    }

    const int step = targetBrightness > brightness ? 1 : -1;


    const int stepDelay = 500 / utils::abs_diff(targetBrightness, brightness);

    // 使用循环更新亮度
    while (brightness != targetBrightness) {
        brightness += step; // 增加或减少亮度
        analogWrite(LED_BUILTIN, brightness);
        delay(stepDelay);
        yield();
    }

    Serial.println(brightness);
}


void Service::init() {
    pinMode(LED_BUILTIN, OUTPUT); // 使用蓝色LED管脚
    Serial.println("SERVICE initialized");
}

void Service::reportStatus() const {
    StaticJsonDocument<512> doc(newJsonDocument());

    doc["type"] = "status";
    const JsonObject status = doc.createNestedObject("payload"); // Otherwise create a new one

    status["灯开关"] = brightness > 0;
    status["灯亮度"] = static_cast<int>(brightness / 2.55);

    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_STATUS, 1, res.c_str());
}

void Service::lightService(const StaticJsonDocument<512> &message) {
    StaticJsonDocument<512> doc(newJsonDocument());

    doc["type"] = "event";
    const char *serviceName = message["service_name"];
    if (strcmp("light", serviceName) == 0) {
        if (message["payload"].is<String>()) {
            const String payload = message["payload"];
            char *endp;
            const char *start = payload.c_str();
            const uint8_t value = strtoul(start, &endp, 10);
            if (endp != start) {
                setBrightness(value);
                doc["payload"] = value;
            } else if (strcmp(start, "true") == 0) {
                doc["payload"] = openLight();
            } else if (strcmp(start, "false") == 0) {
                doc["payload"] = closeLight();
            }
        }
        if (message["payload"].is<uint8_t>()) {
            const uint8_t value = message["payload"];
            setBrightness(value);
            doc["payload"] = value;
        }
    } else if (strcmp("open", serviceName) == 0) {
        doc["payload"] = openLight();
    } else if (strcmp("close", serviceName) == 0) {
        doc["payload"] = closeLight();
    } else if (strcmp("switch", serviceName) == 0) {
        doc["payload"] = switchLight();
    }

    if (!doc.containsKey("payload")) {
        return;
    }

    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_EVENTS, 2, res.c_str());
}

void Service::callback(const StaticJsonDocument<512> &message) {
    const char *serviceName = message["service_name"];
    if (strcmp("status", serviceName) != 0) {
        lightService(message);
    }
    reportStatus();
}
