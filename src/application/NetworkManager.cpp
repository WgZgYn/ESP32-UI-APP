//
// Created by 20495 on 24-10-9.
//

#include "NetworkManager.h"

#include <EEPROM.h>
#include <nvs_flash.h>
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

IPAddress NetworkManager::getIP() {
    return WiFi.localIP();
}

void NetworkManager::setup() {
    nvs_flash_init(); // Used to store the Wi-Fi connect state;

    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("Failed to initialize EEPROM");
    }

    EEPROM_inited = true;

    HAL::cleanInfo();
}

int32_t first_char_at(const char *str, const size_t len, const char c, size_t begin = 0) {
    while (begin < len && str[begin] != c) ++begin;
    if (begin == len) return -1;
    return begin;
}

void NetworkManager::update() {
    HAL::showInfo();
    switch (wifiState) {
        case WiFiState::None: {
            HAL::printInfo("None NetWork");
            HAL::delay(1000);
            HAL::printInfo("Open SoftAP");
            HAL::delay(1000);

            const IPAddress local(192, 168, 4, 1);
            const IPAddress gateway(192, 168, 4, 1);
            const IPAddress subnet(255, 255, 255, 0);

            WiFi.mode(WIFI_MODE_AP);
            WiFi.softAPConfig(local, gateway, subnet);
            WiFi.softAP(SSID, Password);

            HAL::printInfo("SSID: ");
            HAL::printInfo(SSID);
            HAL::delay(1000);

            HAL::printInfo("Password: ");
            HAL::printInfo(Password);
            HAL::delay(1000);

            wifiState = WiFiState::SoftAP;
        }
        break;

        case WiFiState::SoftAP: {
            HAL::printInfo("Please connect to AP");
            HAL::delay(1000);

            server.begin(serv_port);
            wifiState = WiFiState::WaitingClient;
        }
        break;

        case WiFiState::WaitingClient: {
            if (server.hasClient()) {
                if (client && client.connected()) {
                    client.stop(); // This need to be considered
                }
                client = server.available();
                HAL::printInfo("Client connected");
                wifiState = WiFiState::ClientConnected;
            } else {
                spin();
            }
        }
        break;

        case WiFiState::ClientConnected: {
            if (!client.connected()) {
                HAL::printInfo("Client Disconnected");
                wifiState = WiFiState::WaitingClient;
            }

            if (buffer[0] != '\0') {
                memset(buffer, 0, bufferSize); // 清空缓存区
            }

            size_t n = client.readBytesUntil(terminator, buffer, bufferSize); // This will not read the terminator.
            if (n == 0) {
                client.write(static_cast<uint8_t>(TcpResponse::NoData));
            } else if (n < bufferSize) {
                // buffer[n] = '\0'; // keep it C String-Like, no panic, it's ok

                const int32_t pos1 = first_char_at(buffer, n, '\t');
                if (pos1 == -1) {
                    client.write(static_cast<uint8_t>(TcpResponse::FormatError));
                    return;
                }
                buffer[pos1] = '\0'; // replace the '\t' to '\0'

                const int32_t pos2 = first_char_at(buffer, n, '\t', pos1 + 1);
                if (pos2 == -1) {
                    client.write(static_cast<uint8_t>(TcpResponse::FormatError));
                    return;
                }
                buffer[pos2] = '\0';

                WiFi_SSID = &buffer[0];
                WiFi_Password = &buffer[pos1 + 1];
                WiFi_IP = &buffer[pos2 + 1];

                no_host = pos2 + 1 == n - 1;
                HAL::printInfo("Config Finished");
                wifiState = WiFiState::WiFiConnecting;

                WiFi.mode(WIFI_MODE_STA);
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
            } else {
                client.write(static_cast<uint8_t>(TcpResponse::MessageTooLong));
            }
        }
        break;

        case WiFiState::WiFiConnecting: {
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
                client.write(static_cast<uint8_t>(TcpResponse::FinishWithoutHost));
                HAL::delay(100);
                HAL::printInfo("Config Finished");
                return;
            }
            // async_client.onConnect([&](void *arg, AsyncClient *client) {
            //     Serial.println("Connected");
            //     wifiState = WiFiState::HostConnected;
            //     const String id(ESP.getEfuseMac());
            //     if (client->space() > 50 && client->canSend()) {
            //         client->add(id.c_str(), id.length());
            //         client->send();
            //     }
            // }, &async_client);
            // async_client.connect(WiFi_IP, host_port); // Test it
            client.connect(WiFi_IP, host_port);
            wifiState = WiFiState::HostConnecting;
        }

        case WiFiState::HostConnecting: {
            if (client.connected()) {
                wifiState = WiFiState::HostConnected;
                const String id(ESP.getEfuseMac());
                client.write(id.c_str(), id.length());
                return;
            }
            spin();
        }
        break;

        case WiFiState::HostConnected: {
            if (client.connected()) {
                const int code = client.read();
                if (code != HOST_ACK) {
                    wifiState = WiFiState::WiFiConnected;
                    return;
                }
                wifiState = WiFiState::Finish;
                return;
            }
            wifiState = WiFiState::WiFiConnected;
        }

        case WiFiState::Finish: {
            HAL::delay(50);
        }
        break;

        default:
            break;
    }
}
