//
// Created by 20495 on 24-10-18.
//

#ifndef WEBAUTHSERVER_H
#define WEBAUTHSERVER_H

#include <DNSServer.h>
#include <WebServer.h>
#include <app/app.h>

class WebAuthService final : public app::Service {
    WebServer server;
    DNSServer dnsServer;

public:
    WebAuthService() = default;
    ~WebAuthService() override = default;

    void setup() override;

    void loop() override;

    static WebAuthService& getInstance() {
        static WebAuthService instance;
        return instance;
    }
};


#endif //WEBAUTHSERVER_H
