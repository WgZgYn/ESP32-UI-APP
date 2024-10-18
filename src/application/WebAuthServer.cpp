//
// Created by 20495 on 24-10-18.
//

#include "WebAuthServer.h"

#include <hal/hal.h>

void WebAuthService::setup() {
    // if the softAP is not started, start it
    if (WiFi.getMode() != WIFI_MODE_AP) {
        WiFi.mode(WIFI_MODE_AP);
        WiFi.softAP("ESP32-1", "12345678");
    }

    // Handle root url
    server.on("/", HTTP_GET, [this] {
        Serial.println("Root URL");
        server.send(200, "text/html", "<h1>Hello World</h1>");
    });

    // make the softAP web auth server
    Serial.println("Starting WebAuthServer");
    server.begin(80);
    dnsServer.start(53, "*", WiFi.softAPIP());
}

void WebAuthService::loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}
