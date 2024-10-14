#include "UI_VIEW.h"

#include <application/DeviceInfo.h>
#include <application/NetworkManager.h>
#include <application/WiFiScan.h>
#include <game/game.h>
#include <mqtt/MqttView.h>

#include "launcher.h"
#include "element/widget/block.h"

astra::ui::TileMenu rootPage = astra::ui::TileMenu();
astra::ui::ListMenu secondPage = astra::ui::ListMenu();
astra::Launcher astraLauncher = astra::Launcher(&rootPage);

void UIConfig() {
    static bool initialized = false;
    if (initialized) return;
    rootPage.add("Time", icon::moon, &block::getInstance());
    rootPage.add("MQTT", icon::settings0, &mqtt::MqttView::getInstance());
    rootPage.add("GameOfLife", icon::science, &game::GameOfLife::Runner::getInstance());
    rootPage.add("secondPage", icon::settings1, &secondPage);

    secondPage.add("-Device Pair", &NetworkManager::getInstance());
    secondPage.add("-Device Info", &DeviceInfo::getInstance());
    secondPage.add("-WiFi Scan", &WiFiScan::getInstance());
    secondPage.add("-测试测试测试4");
    secondPage.add("-测试测试测试5");
    secondPage.add("-测试测试测试6");
    initialized = true;
}
