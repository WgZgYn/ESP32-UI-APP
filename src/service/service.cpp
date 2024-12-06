#include "service.h"
#include <ArduinoJson.h>
#include <Adafruit_AM2320.h>
#include <device/device.h>
#include <hal/hal.h>

#define SDA_PIN 21
#define SCL_PIN 22

extern "C" {
#include "freertos/timers.h"
#include "freertos/FreeRTOS.h"
}


Adafruit_AM2320 am2320 = Adafruit_AM2320();
TimerHandle_t sensorTimer;

void sensorTimerCallback(TimerHandle_t timer) {
    Service &service = Service::getInstance();
    float temp = am2320.readTemperature();
    float hum = am2320.readHumidity();

    service.temperature = temp;
    service.humidity = hum;

    service.reportStatus();
}


StaticJsonDocument<512> newJsonDocument() {
    StaticJsonDocument<512> doc;
    doc["efuse_mac"] = DeviceConfig::getInstance().ID;
    doc["model_id"] = DeviceConfig::model_id;
    doc["model_name"] = DeviceConfig::model_name;
    return doc;
}


void Service::init() {
    Wire.begin(SDA_PIN, SCL_PIN);

    bool ok = am2320.begin();
    Serial.println("begining am2320");
    Serial.println(ok ? "ok" : "err");

    sensorTimer = xTimerCreate(
        "SensorTimer",
        pdMS_TO_TICKS(5000),
        pdTRUE,
        nullptr,
        sensorTimerCallback
    );
    if (sensorTimer != NULL) {
        xTimerStart(sensorTimer, 0);
    }
    Serial.println("SERVICE initialized");
}

void Service::reportStatus() const {
    StaticJsonDocument<512> doc(newJsonDocument());

    doc["type"] = "status";
    const JsonObject status = doc.createNestedObject("payload"); // Otherwise create a new one


    // fetch data in the background task

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" Humidity: ");
    Serial.println(humidity);

    status["temperature"] = temperature;
    status["humidity"] = humidity;


    String res;
    serializeJson(doc, res);
    mqtt::publish(mqtt::MQTT_PUBLISH_STATUS, 1, res.c_str());
}


void Service::callback(const StaticJsonDocument<512> &message) {
    const char *serviceName = message["service_name"];
    if (strcmp("status", serviceName) == 0) {
        reportStatus();
    }
}

void ServiceUI::onInit() {
}

bool ServiceUI::onOpen() {
    return true;
}

void ServiceUI::onLoop() {
    HAL::cleanInfo();
    const String temperature = "temperature: " + String(service.temperature);
    const String humidity = "humidity: " + String(service.humidity);
    HAL::printInfo(temperature);
    HAL::printInfo("\n");
    HAL::printInfo(humidity);
    HAL::delay(100);
}

void ServiceUI::onExit() {
}
