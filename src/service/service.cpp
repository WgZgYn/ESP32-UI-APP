#include "service.h"
#include <ArduinoJson.h>
#include <utils/utils.h>


const char *Service::openLight() {
    light = true;
    brightness = 255;
    analogWrite(LED_BUILTIN, 255);
    return "open";
}

const char *Service::closeLight() {
    light = false;
    brightness = 0;
    analogWrite(LED_BUILTIN, 0);
    return "close";
}

const char *Service::switchLight() {
    return light ? closeLight() : openLight();
}

void Service::setBrightness(const uint8_t val) {
    float scaledVal = val * 2.56;

    // 确保 scaledVal 在 0 到 255 之间
    if (scaledVal > 255) scaledVal = 255;
    if (scaledVal < 0) scaledVal = 0;

    // 将浮点数转换为 uint8_t
    const uint8_t targetBrightness = static_cast<uint8_t>(scaledVal);


    Serial.print("from ");
    Serial.print(brightness);
    Serial.print(" to ");
    Serial.println(targetBrightness);

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
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP); // 按钮连接到GPIO0，低电平触发
    Serial.println("SERVICE initialized");
}

bool Service::callback(const char *cmd, StaticJsonDocument<128> &doc) {
    if (strcmp(cmd, "open") == 0) {
        doc["payload"] = openLight();
        return true;
    }
    if (strcmp(cmd, "close") == 0) {
        doc["payload"] = closeLight();
        return true;
    }
    if (strcmp(cmd, "switch") == 0) {
        doc["payload"] = switchLight();
        return true;
    }
    if (strncmp(cmd, "light=", 6) == 0) {
        const char *val = cmd + 6;
        char *endptr;
        const unsigned intValue = strtoul(val, &endptr, 10);
        if (endptr != val) {
            setBrightness(intValue);
            doc["payload"] = cmd;
            return true;
        }
        if (strcmp(val, "true") == 0) {
            doc["payload"] = openLight();
            return true;
        }
        if (strcmp(val, "false") == 0) {
            doc["payload"] = closeLight();
            return true;
        }
        return false;
    }
    if (strcmp(cmd, "status") == 0) {
        // Change the type to status
        doc["type"] = "status";
        JsonObject status = doc.createNestedObject("payload");
        status["灯开关"] = light ? "true" : "false";
        status["灯亮度"] = brightness;
        return true;
    }
    return false;
}
