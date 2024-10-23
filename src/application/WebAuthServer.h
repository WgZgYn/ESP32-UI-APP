//
// Created by 20495 on 24-10-18.
//

#ifndef WEBAUTHSERVER_H
#define WEBAUTHSERVER_H

#include <DNSServer.h>
#include <WebServer.h>
#include <app/app.h>

#include "WiFiScan.h"

extern const char *web;

class WebAuthService final : public app::Service {
    WebServer server;
    DNSServer dnsServer;

    class CaptiveRequestHandler final : public RequestHandler {
        bool canHandle(HTTPMethod method, String uri) override {
            return true;
        }

        bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override;
    };

public:
    WebAuthService() = default;

    ~WebAuthService() override = default;

    void setup() override;

    void loop() override;

    static WebAuthService &getInstance() {
        static WebAuthService instance;
        return instance;
    }
};


#endif //WEBAUTHSERVER_H
