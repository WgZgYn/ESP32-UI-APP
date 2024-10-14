#pragma once

#ifndef MYHAL_HPP
#define MYHAL_HPP


#include <Arduino.h>
#include "hal.h"
#include "U8g2Bind.h"
#include "ArduinoBind.h"


#define R16(X) ((uint16_t)std::round(X))

class MyHAL final : public U8G2BIND, public ARDUINOBIND {
public:
    MyHAL() = default;

    ~MyHAL() override = default;


    const char *type() override {
        return "Derive";
    }

    void init() override {
        u8g2_init();
    }

    /**
     * @brief buzzer.
     * */
    void _beep(float _freq) override {
    }

    void _beepStop() override {
    }

    void _setBeepVol(uint8_t _vol) override {
    }


    void _screenOn() override {
        Serial.println("None _screenOn");
        // _ssd1306_transmit_cmd(0X8D); // set dc-dc command.
        // _ssd1306_transmit_cmd(0X14); // dc-dc enable.
        // _ssd1306_transmit_cmd(0XAF); // display on.
    }

    void _screenOff() override {
        Serial.println("None _screenOff");
        // _ssd1306_transmit_cmd(0X8D); // set dc-dc command.
        // _ssd1306_transmit_cmd(0X10); // dc-dc disable.
        // _ssd1306_transmit_cmd(0XAE); // display off.
    }

    /**
     * @brief key.
     */

    bool _getKey(key::KEY_INDEX _keyIndex) override {
        Serial.println("None _getKey");
        return false;
    }

    /**
     * @brief system config.
     */
    void _updateConfig() override {
    }

    static MyHAL& getInstance() {
        static MyHAL instance;
        return instance;
    }
};

#endif
