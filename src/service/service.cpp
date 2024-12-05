#include "service.h"
#include <ArduinoJson.h>
#include <hal/hal.h>
#include <utils/utils.h>

constexpr int AIR_CONDITION_OFFSET = 1;

const String Service::ID{ESP.getEfuseMac() + AIR_CONDITION_OFFSET};

const char *modeStr(const int mode) {
    switch (mode) {
        case 0:
            return "Heating";
        case 1:
            return "Refrigeration";
        case 2:
            return "Dehumidify";
        case 3:
            return "AirSupply";
        default:
            return "";
    }
}

const char *windStr(const int wind) {
    switch (wind) {
        case 0:
            return "Low";
        case 1:
            return "Mid";
        case 2:
            return "High";
        default:
            return "";
    }
}

StaticJsonDocument<512> newJsonDocument() {
    StaticJsonDocument<512> doc;
    doc["efuse_mac"] = Service::ID;
    doc["model_id"] = Service::model_id;
    doc["model_name"] = Service::model_name;
    return doc;
}

void Service::init() {
    Serial.println("SERVICE initialized");
}

void Service::reportStatus() const {
    StaticJsonDocument<512> doc(newJsonDocument());
    doc["type"] = "status";
    const JsonObject status = doc.createNestedObject("payload"); // Otherwise create a new one

    status["电源"] = power;
    status["温度"] = temperature;
    status["风速"] = windSpeed;
    status["模式"] = mode;

    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_STATUS, 1, res.c_str());
}

void Service::setPower(bool power) {
    StaticJsonDocument<512> doc(newJsonDocument());
    doc["type"] = "event";
    const JsonObject payload = doc.createNestedObject("payload"); // Otherwise create a new one

    this->power = power;
    payload["电源"] = this->power;

    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_EVENTS, 2, res.c_str());
}

void Service::setMode(int mode) {
    StaticJsonDocument<512> doc(newJsonDocument());
    doc["type"] = "event";
    const JsonObject payload = doc.createNestedObject("payload"); // Otherwise create a new one

    if (mode > 3 || mode < 0) {
        return;
    }
    this->mode = mode;
    payload["模式"] = this->mode;

    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_EVENTS, 2, res.c_str());
}

void Service::setWindSpeed(int speed) {
    StaticJsonDocument<512> doc(newJsonDocument());
    doc["type"] = "event";
    const JsonObject payload = doc.createNestedObject("payload"); // Otherwise create a new one

    if (speed > 2 || speed < 0) {
        return;
    }

    windSpeed = speed;
    payload["风速"] = windSpeed;

    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_EVENTS, 2, res.c_str());
}

void Service::setTemperature(float temp) {
    StaticJsonDocument<512> doc(newJsonDocument());
    doc["type"] = "event";
    const JsonObject payload = doc.createNestedObject("payload"); // Otherwise create a new one

    if (temp > maxTemp || temp < minTemp) {
        return;
    }
    temp = static_cast<float>(static_cast<int>(temp * 2)) / 2.0f;
    temperature = temp;
    payload["温度"] = temperature;

    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_EVENTS, 2, res.c_str());
}


void Service::callback(const StaticJsonDocument<512> &message) {
    const char *serviceName = message["service_name"];
    if (strcmp("status", serviceName) != 0) {
        if (strcmp("power-on", serviceName) == 0) {
            setPower(true);
        } else if (strcmp("power-off", serviceName) == 0) {
            setPower(false);
        } else if (strcmp("temperature", serviceName) == 0) {
            const float temp = message["payload"];
            setTemperature(temp);
        } else if (strcmp("wind", serviceName) == 0) {
            const int windSpeed = message["payload"];
            setWindSpeed(windSpeed);
        } else if (strcmp("mode", serviceName) == 0) {
            const int mode = message["payload"];
            setMode(mode);
        }
    }
    reportStatus();
}

void ServiceUI::onInit() {
}

bool ServiceUI::onOpen() {
    return true;
}

void ServiceUI::onLoop() {
    HAL::cancelInfo();
    const String power = String("power: ") + String(service.power);
    HAL::printInfo(power);
    HAL::delay(50);
    const String wind = String("wind: ") + windStr(service.windSpeed);
    HAL::printInfo(wind);
    HAL::delay(50);
    const String mode = String("mode: ") + modeStr(service.mode);
    HAL::printInfo(mode);
    HAL::delay(50);
    const String temp = String("temp: ") + String(service.temperature);
    HAL::printInfo(temp);
    HAL::delay(50);
}

void ServiceUI::onExit() {
}
