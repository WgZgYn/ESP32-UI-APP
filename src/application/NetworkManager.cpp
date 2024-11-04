//
// Created by 20495 on 24-10-9.
//

#include "NetworkManager.h"

#include <EEPROM.h>
#include <WiFi.h>
#include <hal/hal.h>

constexpr size_t EEPROM_SIZE = 128;


void spin() {
    static uint8_t time = 0;
    if (time % 4 == 0) {
        HAL::printInfo("|");
    } else if (time % 4 == 1) {
        HAL::printInfo("/");
    } else if (time % 4 == 2) {
        HAL::printInfo("-");
    } else if (time % 4 == 3) {
        HAL::printInfo("\\");
    }
    ++time;
    HAL::delay(50);
    HAL::cancelInfo();
}

int32_t first_char_at(const char *str, const size_t len, const char c, size_t begin = 0) {
    while (begin < len && str[begin] != c) ++begin;
    if (begin == len) return -1;
    return begin;
}

#ifdef USING_BLUETOOTH
void NetworkManager::startBTH() {
    HAL::printInfo("NetworkManager::startBTH()");
    HAL::delay(500);
    server.begin("ESP32");
}
#endif

void NetworkManager::startAP() const {
    const IPAddress local(192, 168, 4, 1);
    const IPAddress gateway(192, 168, 4, 1);
    const IPAddress subnet(255, 255, 255, 0);

    HAL::printInfo("Opening SoftAP");
    HAL::delay(1000);


    // WiFi.mode(WIFI_MODE_AP);
    WiFi.softAPConfig(local, gateway, subnet);
    WiFi.softAP(SSID, Password);


    HAL::printInfo("WiFi: ");
    HAL::printInfo(SSID);
    HAL::delay(1000);
    HAL::printInfo(Password);
    HAL::delay(1000);
}

void NetworkManager::startServer() {
    server.begin();
}
#ifdef USING_BLUETOOTH
void NetworkManager::recvClient() {
    if (server.available()) {
        server.write(static_cast<uint8_t>(TcpResponse::AskForPair));
    }
}
#else
void NetworkManager::recvClient() {
    if (remoteClient && remoteClient.connected()) {
        remoteClient.write(static_cast<uint8_t>(TcpResponse::OtherClientConnect));
        remoteClient.stop(); // This need to be considered
    }
    remoteClient = server.available();
    remoteClient.write(static_cast<uint8_t>(TcpResponse::AskForPair));
}
#endif

bool NetworkManager::readMessage() {
    if (buffer[0] != '\0') {
        memset(buffer, 0, bufferSize); // 清空缓存区
    }
#ifdef USING_BLUETOOTH
#define CLIENT server
#define SERVER server
#else
#define CLIENT remoteClient
#define SERVER server
#endif
    size_t n = CLIENT.readBytesUntil(terminator, buffer, bufferSize); // This will not read the terminator.
    if (n == 0) {
        CLIENT.write(static_cast<uint8_t>(TcpResponse::NoData));
        return false;
    }

    if (n >= bufferSize) {
        CLIENT.write(static_cast<uint8_t>(TcpResponse::MessageTooLong));
        return false;
    }

    // buffer[n] = '\0'; // keep it C String-Like, no panic, it's ok
    const int32_t pos1 = first_char_at(buffer, n, '\t');
    if (pos1 == -1) {
        CLIENT.write(static_cast<uint8_t>(TcpResponse::FormatError));
        return false;
    }
    buffer[pos1] = '\0'; // replace the '\t' to '\0'
    const int32_t pos2 = first_char_at(buffer, n, '\t', pos1 + 1);
    if (pos2 == -1) {
        CLIENT.write(static_cast<uint8_t>(TcpResponse::FormatError));
        return false;
    }
    buffer[pos2] = '\0';

    CLIENT.write(static_cast<uint8_t>(TcpResponse::Valid));

    WiFi_SSID = &buffer[0];
    WiFi_Password = &buffer[pos1 + 1];
    WiFi_IP = &buffer[pos2 + 1];

    no_host = pos2 + 1 == n - 1;

    wifiState = WiFiState::WiFiConnecting;

    // WiFi.mode(WIFI_MODE_APSTA); // MOVED
    WiFi.begin(WiFi_SSID, WiFi_Password);
    HAL::printInfo("Config Finished");
    HAL::printInfo("Connecting to WiFi");
    HAL::printInfo("SSID: ");
    Serial.println(WiFi_SSID);
    HAL::printInfo(WiFi_SSID);
    HAL::printInfo("Password: ");
    Serial.println(WiFi_Password);
    HAL::printInfo(WiFi_Password);
    HAL::printInfo("Host: ");
    Serial.println(WiFi_IP);
    HAL::printInfo(WiFi_IP);

    return true;
}

bool NetworkManager::loadSavedWiFi() {
    const int flag = EEPROM.read(0);
    Serial.println(flag);

    if (EEPROM.read(0) != 0xFF) {
        Serial.println("load saved wifi config");
        EEPROM.readBytes(0, buffer, 128);
        EEPROM_loaded = true;
        for (int i = 0; i < bufferSize; i++) {
            if (buffer[i] == 0) {
                Serial.print("0");
            } else {
                Serial.print(buffer[i]);
            }
        }
        Serial.println();
        return true;
    }
    return false;
}

IPAddress NetworkManager::getIP() {
    return WiFi.localIP();
}

void NetworkManager::setup() {
    if (inited) {
        HAL::cleanInfo();
        return;
    }

    if (loadSavedWiFi()) {
        const int32_t pos1 = first_char_at(buffer, bufferSize, '\0');
        if (pos1 == -1) {
            EEPROM_loaded = false;
            return;
        }
        const int32_t pos2 = first_char_at(buffer, bufferSize, '\0', pos1 + 1);
        if (pos2 == -1) {
            EEPROM_loaded = false;
            return;
        }
        WiFi_SSID = &buffer[0];
        WiFi_Password = &buffer[pos1 + 1];
        WiFi_IP = &buffer[pos2 + 1];


        Serial.println(WiFi_SSID);
        Serial.println(WiFi_Password);
        Serial.println(WiFi_IP);


        wifiState = WiFiState::WiFiConnecting;
        WiFi.begin(WiFi_SSID, WiFi_Password);

        HAL::printInfo("Connecting to WiFi");
        HAL::printInfo("SSID: ");
        Serial.println(WiFi_SSID);
        HAL::printInfo(WiFi_SSID);
        HAL::printInfo("Password: ");
        Serial.println(WiFi_Password);
        HAL::printInfo(WiFi_Password);
        HAL::printInfo("Host: ");
        Serial.println(WiFi_IP);
        HAL::printInfo(WiFi_IP);
    }


    HAL::cleanInfo();
    inited = true;
}


void NetworkManager::update() {
    HAL::showInfo();
    switch (wifiState) {
        case WiFiState::None: {
#ifdef USING_BLUETOOTH
            startBTH();
#else
            startAP();
#endif
            wifiState = WiFiState::SoftAP;
        }
        break;

        case WiFiState::SoftAP: {
            {
                HAL::printInfo("Please connect to AP");
                HAL::delay(1000);
            }

            startServer();
            wifiState = WiFiState::WaitingClient;
        }
        break;

        case WiFiState::WaitingClient: {
            if (server.hasClient()) {
                HAL::printInfo("Client connected");
                recvClient();
                wifiState = WiFiState::ClientConnected;
            } else {
                spin();
            }
        }
        break;

        case WiFiState::ClientConnected: {
#ifdef USING_BLUETOOTH
            if (!server.hasClient())
#else
            if (!remoteClient.connected())
#endif
            {
                HAL::printInfo("Client Disconnected");
                wifiState = WiFiState::WaitingClient;
                return;
            }
            readMessage();
        }
        break;

        case WiFiState::WiFiConnecting: {
            static int times = 0;
            if (++times == 50) {
                Serial.println("time out");
                times = 0;
                if (remoteClient.connected()) {
                    wifiState = WiFiState::ClientConnected;
                    remoteClient.write(static_cast<uint8_t>(TcpResponse::WiFiConnectionTimeout)); // WiFi Timeout
                } else {
                    wifiState = WiFiState::WaitingClient;
                }
                return;
            }


            if (WiFi.status() == WL_CONNECTED) {
                HAL::printInfo("WiFi Connected");
                wifiState = WiFiState::WiFiConnected;
            } else {
                spin();
            }
        }
        break;

        case WiFiState::WiFiConnected: {
            if (WiFi_IP == nullptr || WiFi_IP[0] == '\0') {
                wifiState = WiFiState::Finish;
                CLIENT.write(static_cast<uint8_t>(TcpResponse::FinishWithoutHost));
                HAL::delay(100);
                HAL::printInfo("Config Finished");
                return;
            }

            localClient.connect(WiFi_IP, host_port, 1000);
            wifiState = WiFiState::HostConnecting;
        }

        case WiFiState::HostConnecting: {
            static int times = 0;
            Serial.println(times);
            if (++times == 5) {
                times = 0;
                if (remoteClient.connected()) {
                    remoteClient.write(static_cast<uint8_t>(TcpResponse::HostConnectionTimeOut));
                    wifiState = WiFiState::ClientConnected;
                } else {
                    wifiState = WiFiState::WaitingClient;
                }
                return;
            }


            if (localClient.connected()) {
                wifiState = WiFiState::HostConnected;
                localClient.write(static_cast<uint8_t>(TcpResponse::AskForPair));
                return;
            }
            spin();
            wifiState = WiFiState::WiFiConnected;
        }
        break;

        case WiFiState::HostConnected: {
            if (!localClient.connected()) {
                remoteClient.write(static_cast<uint8_t>(TcpResponse::FinishWithHost));
                wifiState = WiFiState::WiFiConnected;
                return;
            }

            const int code = localClient.read();
            if (code != HOST_ACK) {
                wifiState = WiFiState::HostConnecting;
                return;
            }

            wifiState = WiFiState::Finish;
        }

        case WiFiState::Finish: {
            Serial.println("Finish");

            if (!EEPROM_loaded) {
                EEPROM.writeBytes(0, buffer, bufferSize);
                EEPROM.commit();
                Serial.println("store the wifi config");
            }

            HAL::delay(50);
            remoteClient.write(static_cast<uint8_t>(TcpResponse::Finish));
            remoteClient.stop();

            app::App::getInstance().ui = true;
        }
        break;
        default:
            break;
    }
}
