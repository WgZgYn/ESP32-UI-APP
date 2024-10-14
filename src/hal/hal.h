#pragma once
#ifndef ASTRA_CORE_SRC_HAL_HAL_H_
#define ASTRA_CORE_SRC_HAL_HAL_H_

#include <deque>
#include <string>
#include <Arduino.h>


class String;

namespace key {
    typedef enum keyFilter {
        CHECKING = 0,
        KEY_0_CONFIRM,
        KEY_1_CONFIRM,
        RELEASED,
    } KEY_FILTER;

    typedef enum keyAction {
        RELEASE = 0,
        CLICK,
        PRESS,
    } KEY_ACTION;

    typedef enum keyIndex {
        KEY_0 = 0,
        KEY_1,
        KEY_NUM,
    } KEY_INDEX;
}

namespace sys {
    struct config {
        uint8_t screenWeight = 128;
        uint8_t screenHeight = 64;
        float screenBright = 255;
        // todo place other param of system

        static config &getSystemConfig() {
            static config sysConfig;
            return sysConfig;
        }

    private:
        config() = default;
    };

    extern const config &cfg;
}

/**
 * @brief hardware abstraction layer.
 * @note provide a set of interfaces to access hardware.
 * @note provide some default implementation.
 */
class HAL {
    static HAL *hal;
    static std::deque<std::string> queue;

public:
    static HAL *get() {
        return hal;
    }

    static bool check() {
        return hal != nullptr;
    }

    static bool inject(HAL *_hal) {
        if (_hal == nullptr) {
            return false;
        }

        _hal->init();
        hal = _hal;
        return true;
    }

public:
    static void destroy(); // destroy HAL instance.
    HAL() = default;

    virtual ~HAL() = default;

    virtual const char *type() { return "Base"; }

    virtual void init() {
    };

public:
    static void *getCanvasBuffer() { return get()->_getCanvasBuffer(); }

    virtual void *_getCanvasBuffer() = 0;

    static uint8_t getBufferTileHeight() { return get()->_getBufferTileHeight(); }

    virtual uint8_t _getBufferTileHeight() = 0;

    static uint8_t getBufferTileWidth() { return get()->_getBufferTileWidth(); }

    virtual uint8_t _getBufferTileWidth() = 0;

    static void canvasUpdate() { get()->_canvasUpdate(); }

    virtual void _canvasUpdate() = 0;

    static void canvasClear() { get()->_canvasClear(); }

    virtual void _canvasClear() = 0;

    static void setFont(const uint8_t *_font) { get()->_setFont(_font); }

    virtual void _setFont(const uint8_t *_font) = 0;

    static uint8_t getFontWidth(const char *_text) { return get()->_getFontWidth(_text); }

    virtual uint8_t _getFontWidth(const char *_text) = 0;

    static uint8_t getFontHeight() { return get()->_getFontHeight(); }

    virtual uint8_t _getFontHeight() = 0;

    static void setDrawType(uint8_t _type) { get()->_setDrawType(_type); }

    virtual void _setDrawType(uint8_t _type) = 0;


    static void drawPixel(float _x, float _y) { get()->_drawPixel(_x, _y); }

    virtual void _drawPixel(float _x, float _y) = 0;

    static void drawLine(float _x, float _y, float _x2, float _y2) { get()->_drawLine(_x, _y, _x2, _y2); }

    virtual void _drawLine(float _x, float _y, float _x2, float _y2) = 0;

    // notice: _x和_y是字体左下角的坐标 _x and _y is the coordinate the lower left corner of the font
    // todo 检查所有的draw函数的坐标是否是左下角的坐标
    static void drawEnglish(float _x, float _y, const char *_text) { get()->_drawEnglish(_x, _y, _text); }

    virtual void _drawEnglish(float _x, float _y, const char *_text) = 0;

    static void drawChinese(float _x, float _y, const char *_text) { get()->_drawChinese(_x, _y, _text); }

    virtual void _drawChinese(float _x, float _y, const char *_text) = 0;

    static void drawVDottedLine(float _x, float _y, float _h) { get()->_drawVDottedLine(_x, _y, _h); }

    virtual void _drawVDottedLine(float _x, float _y, float _h) = 0;

    static void drawHDottedLine(float _x, float _y, float _l) { get()->_drawHDottedLine(_x, _y, _l); }

    virtual void _drawHDottedLine(float _x, float _y, float _l) = 0;

    static void drawVLine(float _x, float _y, float _h) { get()->_drawVLine(_x, _y, _h); }

    virtual void _drawVLine(float _x, float _y, float _h) = 0;

    static void drawHLine(float _x, float _y, float _l) { get()->_drawHLine(_x, _y, _l); }

    virtual void _drawHLine(float _x, float _y, float _l) = 0;

    static void drawBMP(float _x, float _y, float _w, float _h, const uint8_t *_bitMap) {
        get()->_drawBMP(_x,
                        _y,
                        _w,
                        _h,
                        _bitMap);
    }

    virtual void _drawBMP(float _x, float _y, float _w, float _h, const uint8_t *_bitMap) = 0;

    static void drawBox(float _x, float _y, float _w, float _h) { get()->_drawBox(_x, _y, _w, _h); }

    virtual void _drawBox(float _x, float _y, float _w, float _h) = 0;

    static void drawRBox(float _x, float _y, float _w, float _h, float _r) {
        get()->_drawRBox(_x,
                         _y,
                         _w,
                         _h,
                         _r);
    }

    virtual void _drawRBox(float _x, float _y, float _w, float _h, float _r) = 0;

    static void drawFrame(float _x, float _y, float _w, float _h) { get()->_drawFrame(_x, _y, _w, _h); }

    virtual void _drawFrame(float _x, float _y, float _w, float _h) = 0;

    static void drawRFrame(float _x, float _y, float _w, float _h, float _r) {
        get()->_drawRFrame(_x,
                           _y,
                           _w,
                           _h,
                           _r);
    }

    virtual void _drawRFrame(float _x, float _y, float _w, float _h, float _r) = 0;

    static void printInfo(const char *_msg) {
        get()->_printInfo(_msg);
    }

    static void printInfo(const String& _msg) {
        get()->_printInfo(_msg.c_str());
    }

    static void showInfo();

    static void cleanInfo();

    static void cancelInfo();

    virtual void _printInfo(const char *_msg);

    /**
     * @brief system timers.
     */
public:
    static void delay(unsigned long _mill) {
        if (get() == nullptr) return;
        get()->_delay(_mill);
    }

    virtual void _delay(unsigned long _mill) = 0;

    static unsigned long millis() { return get()->_millis(); }

    virtual unsigned long _millis() = 0;

    static unsigned long getTick() { return get()->_getTick(); }

    virtual unsigned long _getTick() = 0;

    static void setRandomSeed() { return get()->_setRandomSeed(); }

    virtual void _setRandomSeed() {
    }

    static unsigned long random() { return get()->_random(); }

    virtual unsigned long _random() { return 0; }


    /**
     * @brief buzzer.
     * */
public:
    static void beep(float _freq) { get()->_beep(_freq); }

    virtual void _beep(float _freq) = 0;

    static void beepStop() { get()->_beepStop(); }

    virtual void _beepStop() {
    };

    static void setBeepVol(uint8_t _vol) { get()->_setBeepVol(_vol); }

    virtual void _setBeepVol(uint8_t _vol) {
    };

    static void screenOn() { get()->_screenOn(); }

    virtual void _screenOn() {
    };

    static void screenOff() { get()->_screenOff(); }

    virtual void _screenOff() {
    };

    /**
     * @brief key.
     */
public:
    static bool getKey(key::KEY_INDEX _keyIndex) { return get()->_getKey(_keyIndex); }
    virtual bool _getKey(key::KEY_INDEX _keyIndex) { return false; };

    static bool getAnyKey() { return get()->_getAnyKey(); }

    virtual bool _getAnyKey();

protected:
    key::keyAction key[key::KEY_NUM]{static_cast<key::keyAction>(0)};

public:
    static void keyScan() { get()->_keyScan(); }

    virtual void _keyScan();

    static void keyTest() { return get()->_keyTest(); }

    virtual void _keyTest();

    /**
     * @brief system config.
     */
public:
    static const sys::config &getSystemConfig() { return sys::cfg; }
    // static void setSystemConfig(const sys::config &_cfg) { sys::cfg = _cfg; }

    static void updateConfig() { get()->_updateConfig(); }

    virtual void _updateConfig() {
    }
};

#endif // ASTRA_CORE_SRC_HAL_HAL_H_
