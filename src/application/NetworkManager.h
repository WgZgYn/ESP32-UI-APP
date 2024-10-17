//
// Created by 20495 on 24-10-9.
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <IPAddress.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <WiFiServer.h>
#include <app/app.h>

void spin();

enum class WiFiState {
    None,
    SoftAP,
    WaitingClient,
    ClientConnected,
    WiFiConnecting,
    WiFiConnected,
    WiFiDisconnect,
    HostConnecting,
    HostConnected,
    HostAcknowledged,
    Finish,
};


class NetworkManager final : public app::Activity {
private:
    WiFiState wifiState = WiFiState::None;
    WiFiServer server;
    WiFiClient remoteClient;
    WiFiClient localClient;
    bool inited = false;
    // AsyncClient async_client;

    constexpr static size_t bufferSize = 128;
    constexpr static int host_port = 45677;
    constexpr static int serv_port = 45678;
    constexpr static int HOST_ACK = 200;
    constexpr static char terminator = '\n';

    const char *SSID = "ESP32-1";
    const char *Password = "12345678";

    char buffer[bufferSize + 1]; // Left one char as the end of the str

    char *WiFi_SSID = nullptr;
    char *WiFi_Password = nullptr;
    char *WiFi_IP = nullptr; // It's Optional, so strlen(WiFi_IP) can be 0

    bool EEPROM_inited = false;
    bool no_host = false;

    NetworkManager(): server{serv_port}, buffer{} {
    }


    enum class TcpResponse: uint8_t {
        Valid,
        NoData,
        MessageTooLong,
        FormatError,
        FinishWithoutHost,
        FinishWithHost,
        OtherClientConnect,
        AskForPair,
    };

    void startAP();

    void startServer();

    void recvClient();

    bool readMessage();

public:
    NetworkManager(const NetworkManager &) = delete;

    NetworkManager(NetworkManager &&) = delete;

    NetworkManager &operator=(const NetworkManager &) = delete;

    NetworkManager &operator=(NetworkManager &&) = delete;

    WiFiState getWiFiState() const { return wifiState; }

    static IPAddress getIP();

    void setup() override;

    void update() override;

    static NetworkManager &getInstance() {
        static NetworkManager instance;
        return instance;
    }
};

class WiFiManagerService final : public app::Service {
    void setup() override {
        auto &mgr = NetworkManager::getInstance();
        while (mgr.getWiFiState() != WiFiState::Finish) {
            mgr.update();
        }
    }

    void loop() override {
    }

public:
    static WiFiManagerService &getInstance() {
        static WiFiManagerService instance;
        return instance;
    }
};


#endif //NETWORKMANAGER_H