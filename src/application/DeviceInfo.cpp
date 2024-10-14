//
// Created by 20495 on 24-10-11.
//

#include "DeviceInfo.h"
#include <string>

#include <Arduino.h>
#include <hal/hal.h>

DeviceInfo::DeviceInfo() {
    info.emplace_back(String("ChipCores:\n") + String(ESP.getChipCores()));
    info.emplace_back(String("ChipRevision:\n") + String(ESP.getChipRevision()));
    info.emplace_back(String("ChipModel:\n") + String(ESP.getChipModel()));
    info.emplace_back(String("CpuFreqMHz:\n") + String(ESP.getCpuFreqMHz()));

    info.emplace_back(String("FlashChipMode:\n") + String(ESP.getFlashChipMode()));
    info.emplace_back(String("FlashChipSpeed:\n") + String(ESP.getFlashChipSpeed()));
    info.emplace_back(String("FlashChipSize:\n") + String(ESP.getFlashChipSize()));

    info.emplace_back(String("EfuseMac:\n") + String(ESP.getEfuseMac()));

    info.emplace_back(String("CycleCount:\n") + String(ESP.getCycleCount()));

    info.emplace_back(String("HeapSize:\n") + String(ESP.getHeapSize()));
    info.emplace_back(String("FreeHeap:\n") + String(ESP.getFreeHeap()));
    info.emplace_back(String("MinFreeHeap:\n") + String(ESP.getMinFreeHeap()));
    info.emplace_back(String("MaxAllocHeap:\n") + String(ESP.getMaxAllocHeap()));

    info.emplace_back(String("PsramSize:\n") + String(ESP.getPsramSize()));
    info.emplace_back(String("FreePsram:\n") + String(ESP.getFreePsram()));
    info.emplace_back(String("MinFreePsram:\n") + String(ESP.getMinFreePsram()));
    info.emplace_back(String("MaxAllocPsram:\n") + String(ESP.getMaxAllocPsram()));

    info.emplace_back(String("SketchSize:\n") + String(ESP.getSketchSize()));
    info.emplace_back(String("FreeSketchSpace:\n") + String(ESP.getFreeSketchSpace()));
    info.emplace_back(String("SketchMD5:\n") + String(ESP.getSketchMD5()));

    info.emplace_back(String("SdkVersion:\n") + String(ESP.getSdkVersion()));
}

std::vector<std::string> wrapText(const std::string &text, const uint8_t screenWidth) {
    std::vector<std::string> lines;
    std::string currentLine;
    currentLine.reserve(screenWidth + 1);

    for (size_t i = 0; i < text.size(); ++i) {
        currentLine += text[i];
        if (HAL::getFontWidth(currentLine.c_str()) >= screenWidth || text[i] == '\n') {
            lines.emplace_back(currentLine);
            currentLine.clear();
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine); // 添加剩余的部分
    }

    return lines;
}

void DeviceInfo::setup() {
    HAL::cleanInfo();
}

void DeviceInfo::update() {
    static int index = 0;
    std::string text = info[index].c_str();

    std::vector<std::string> msgs(wrapText(text, sys::cfg.screenWeight - 1));

    for (const std::string &msg: msgs) {
        HAL::printInfo(msg.c_str());
        HAL::delay(1000);
    }

    index = (index + 1) % info.size();
}
