//
// Created by 20495 on 24-10-18.
//

#include "WebAuthServer.h"

bool WebAuthService::CaptiveRequestHandler::handle(WebServer &server, const HTTPMethod requestMethod, const String requestUri) {
    if (requestMethod == HTTP_POST && requestUri == "/connect") {
        const String ssid = server.arg("ssid");
        const String password = server.arg("password");

        Serial.println(ssid);
        Serial.println(password);

        WiFi.begin(ssid, password);
        server.send(200);
    } else if (requestMethod == HTTP_GET && requestUri == "/scan") {
        Serial.println(47);
        int n = WiFi.scanNetworks();
        Serial.println(49);
        String s = "[";
        for (int i = 0; i < n; i++) {
            Serial.println(52);
            Serial.println(i);
            if (i != n - 1) {
                s += R"({"ssid":")" + WiFi.SSID(i) +
                        R"(","rssi":")" + WiFi.RSSI(i) +
                        R"(","encryption":")" + WiFi.encryptionType(i) + "\"},";
            } else {
                s += R"({"ssid":")" + WiFi.SSID(i) +
                        R"(","rssi":")" + WiFi.RSSI(i) +
                        R"(","encryption":")" + WiFi.encryptionType(i) + "\"}";
            }
        }
        s += "]";
        Serial.println(s);
        server.send(200, "application/json", s.c_str());
    } else {
        server.send(200, web);
    }
    return true;
}

void WebAuthService::setup() {
    // if the softAP is not started, start it
    WiFi.softAP("ESP32-1", "12345678");

    server.addHandler(new CaptiveRequestHandler());
    Serial.println("Starting WebAuthServer");
    server.begin(80);
    dnsServer.start(53, "*", WiFi.softAPIP());
}

void WebAuthService::loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}
