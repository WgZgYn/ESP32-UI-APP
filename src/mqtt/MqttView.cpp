//
// Created by 20495 on 24-10-9.
//

#include "MqttView.h"

#include <hal/hal.h>

void mqtt::MqttView::update() {
    HAL::canvasClear();

    float h = HAL::getFontHeight();
    HAL::drawChinese(0, h, Message::current().topic.c_str());
    HAL::drawChinese(0, h * 2, String(Message::current().qos).c_str());
    HAL::drawChinese(0, h * 3, Message::current().payload.c_str());

    HAL::canvasUpdate();
}
