//
// Created by 20495 on 24-10-7.
//

#include "Animation.h"

#include <astra/config/config.h>
#include <utils/utils.h>
#include <hal/hal.h>


namespace astra {
    namespace ui {
        inline void Animation::exitAnimation() {
            static uint8_t fadeFlag = 1;
            static uint8_t bufferLen = 8 * HAL::getBufferTileHeight() * HAL::getBufferTileWidth();
            auto *bufferPointer = static_cast<uint8_t *>(HAL::getCanvasBuffer());

            HAL::delay(static_cast<long>(cfg.fadeAnimationSpeed));
            static uint8_t mask[4]{0xAA, 0x00, 0x55, 0x00};
            if (cfg.lightMode) {
                if (fadeFlag >= 1 && fadeFlag <= 4) {
                    for (uint16_t i = 0; i < bufferLen; ++i)
                        if (i % 2 != 0)
                            bufferPointer[i] = bufferPointer[i] & mask[fadeFlag - 1];
                } else {
                    fadeFlag = 0;
                }
            } else {
                if (fadeFlag >= 1 && fadeFlag <= 4) {
                    for (uint16_t i = 0; i < bufferLen; ++i)
                        if (i % 2 != 0)
                            bufferPointer[i] = bufferPointer[i] | mask[fadeFlag - 1];
                } else {
                    fadeFlag = 0;
                }
            }
            fadeFlag++;
        }

        inline void Animation::blur() {
            static uint8_t bufferLen = 8 * HAL::getBufferTileHeight() * HAL::getBufferTileWidth();
            static auto *bufferPointer = static_cast<uint8_t *>(HAL::getCanvasBuffer());
            for (uint16_t i = 0; i < bufferLen; ++i) bufferPointer[i] = bufferPointer[i] & (i % 2 == 0 ? 0x55 : 0xAA);
        }

        inline void Animation::animation(float *_pos, float _posTrg, float _speed) {
            if (*_pos != _posTrg) {
                if (utils::abs_diff(*_pos, _posTrg) < 0.15f) *_pos = _posTrg;
                else *_pos += (_posTrg - *_pos) / ((100 - _speed) / 1.0f);
            }
        }
    }
}
