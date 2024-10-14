//
// Created by 20495 on 24-10-6.
//

#ifndef U8G2BIND_H
#define U8G2BIND_H

#include <U8g2lib.h>
#include "hal.h"

#define OLED_CS_PIN 15
#define OLED_DC_PIN 5
#define OLED_RES_PIN 4

#define R16(X) ((uint16_t)std::round(X))

class U8G2BIND: virtual public HAL {
public:
    U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2{U8G2_R0, OLED_CS_PIN, OLED_DC_PIN, OLED_RES_PIN};

    void u8g2_init() {
        u8g2.begin();
        u8g2.setFontMode(1);
        u8g2.setFontDirection(0);
        u8g2.setFont(u8g2_font_myfont);
    }

    void *_getCanvasBuffer() override {
        return u8g2.getBufferPtr();
    }

    uint8_t _getBufferTileHeight() override {
        return u8g2.getBufferTileHeight();
    }

    uint8_t _getBufferTileWidth() override {
        return u8g2.getBufferTileWidth();
    }

    void _canvasUpdate() override {
        u8g2.sendBuffer();
    }

    void _canvasClear() override {
        u8g2.clearBuffer();
    }

    void _setFont(const uint8_t *_font) override {
        u8g2.setFontMode(1);
        u8g2.setFontDirection(0);
        u8g2.setFont(_font);
    }

    uint8_t _getFontWidth(const char *_text) override {
        return u8g2.getUTF8Width(_text);
    }

    uint8_t _getFontHeight() override {
        return u8g2.getMaxCharHeight();
    }

    void _setDrawType(uint8_t _type) override {
        return u8g2.setDrawColor(_type);
    }

    void _drawPixel(const float _x, const float _y) override {
        u8g2.drawPixel(R16(_x), R16(_y));
    }

    void _drawLine(float _x, float _y, float _x2, float _y2) override {
        u8g2.drawLine(R16(_x), R16(_y), R16(_x2), R16(_y2));
    }

    // notice: _x和_y是字体左下角的坐标 _x and _y is the coordinate the lower left corner of the font
    // 检查所有的draw函数的坐标是否是左下角的坐标

    void _drawEnglish(const float _x, const float _y, const char *_text) override {
        u8g2.drawStr(R16(_x), R16(_y), _text);
    }

    void _drawChinese(const float _x, const float _y, const char *_text) override {
        u8g2.drawUTF8(R16(_x), R16(_y), _text);
    }

    void _drawVDottedLine(const float _x, const float _y, const float _h) override {
        for (unsigned char i = 0; i < static_cast<unsigned char>(std::round(_h)); i++) {
            if (i % 8 == 0 || (i - 1) % 8 == 0 || (i - 2) % 8 == 0)
                continue;
            // u8g2.drawPixel(R16(_x), R16(_y) + i);
            _drawPixel(_x, _y + static_cast<float>(i));
        }
    }

    void _drawHDottedLine(const float _x, const float _y, const float _l) override {
        for (uint8_t i = 0; i < static_cast<uint8_t>(_l); i++) {
            if (i % 8 == 0 || (i - 1) % 8 == 0 || (i - 2) % 8 == 0)
                continue;
            // u8g2_DrawPixel(&canvasBuffer, R16(_x) + i, R16(_y));
            _drawPixel(_x + static_cast<float>(i), _y);
        }
    }

    void _drawVLine(const float _x, const float _y, const float _h) override {
        u8g2.drawVLine(R16(_x), R16(_y), R16(_h));
    }

    void _drawHLine(const float _x, const float _y, const float _l) override {
        u8g2.drawHLine(R16(_x), R16(_y), R16(_l));
    }

    void _drawBMP(const float _x, const float _y, const float _w, const float _h, const unsigned char *_bitMap) override {
        u8g2.drawXBMP(R16(_x), R16(_y), R16(_w), R16(_h), _bitMap);
    }

    void _drawBox(const float _x, const float _y, const float _w, const float _h) override {
        u8g2.drawBox(R16(_x), R16(_y), R16(_w), R16(_h));
    }

    void _drawRBox(const float _x, const float _y, const float _w, const float _h, const float _r) override {
        u8g2.drawRBox(R16(_x), R16(_y), R16(_w), R16(_h), R16(_r));
    }

    void _drawFrame(const float _x, const float _y, const float _w, const float _h) override {
        u8g2.drawFrame(R16(_x), R16(_y), R16(_w), R16(_h));
    }

    void _drawRFrame(const float _x, const float _y, const float _w, const float _h, const float _r) override {
        u8g2.drawRFrame(R16(_x), R16(_y), R16(_w), R16(_h), R16(_r));
    }
};

#endif //U8G2BIND_H
