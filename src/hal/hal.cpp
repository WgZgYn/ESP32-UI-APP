#include "hal.h"
#include <Arduino.h>

HAL *HAL::hal = nullptr;

void HAL::destroy() {
    if (hal == nullptr) return;
    hal = nullptr;
}

namespace sys {
    const config &cfg = config::getSystemConfig();
}

/**
 * @brief log printer. 自动换行的日志输出
 *
 * @param _msg message want to print. 要输出的信息
 * @note cannot execute within a loop. 不能在循环内执行
 */

std::deque<std::string> HAL::queue;

void HAL::showInfo() {
    const uint8_t _fontHeight = getFontHeight();

    canvasClear();
    setDrawType(2); //反色显示

    uint8_t i = 0;
    for (std::string &str: queue) {
        drawEnglish(0, static_cast<float>(_fontHeight + i * (1 + _fontHeight)), str.c_str());
        ++i;
    }

    canvasUpdate();
    setDrawType(1); //回归实色显示
}

void HAL::cleanInfo() {
    queue.clear();
}

void HAL::cancelInfo() {
    queue.pop_back();
}

void HAL::_printInfo(const char *_msg) {
    // static std::mutex mutex; // freeRTOS

    static const uint8_t _max_lines = getSystemConfig().screenHeight / getFontHeight();
    if (queue.size() >= _max_lines)
        queue.pop_front();
    queue.emplace_back(_msg);


    showInfo();
}

bool HAL::_getAnyKey() {
    for (int i = 0; i < key::KEY_NUM; i++) {
        if (getKey(static_cast<key::KEY_INDEX>(i))) return true;
    }
    return false;
}

/**
 * @brief key scanner default. 默认按键扫描函数
 *
 * @note run per 5 ms.
 * @return key::keyValue
 * @todo test this fucking function.
 */
void HAL::_keyScan() {
    static uint8_t _timeCnt = 0;
    static bool _lock = false;
    static key::KEY_FILTER _keyFilter = key::CHECKING;
    switch (_keyFilter) {
        case key::CHECKING:
            if (getAnyKey()) {
                if (getKey(key::KEY_0)) _keyFilter = key::KEY_0_CONFIRM;
                if (getKey(key::KEY_1)) _keyFilter = key::KEY_1_CONFIRM;
            }
            _timeCnt = 0;
            _lock = false;
            break;
        case key::KEY_0_CONFIRM:
        case key::KEY_1_CONFIRM:
            //filter
            if (getAnyKey()) {
                if (!_lock) _lock = true;
                _timeCnt++;

                //timer
                if (_timeCnt > 100) {
                    //long press 1s
                    if (getKey(key::KEY_0)) {
                        key[key::KEY_0] = key::PRESS;
                        key[key::KEY_1] = key::RELEASE;
                    }
                    if (getKey(key::KEY_1)) {
                        key[key::KEY_1] = key::PRESS;
                        key[key::KEY_0] = key::RELEASE;
                    }
                    _timeCnt = 0;
                    _lock = false;
                    _keyFilter = key::RELEASED;
                }
            } else {
                if (_lock) {
                    _keyFilter = key::RELEASED;
                    if (_keyFilter == key::KEY_0_CONFIRM) {
                        key[key::KEY_0] = key::CLICK;
                        key[key::KEY_1] = key::RELEASE;
                    }
                    if (_keyFilter == key::KEY_1_CONFIRM) {
                        key[key::KEY_1] = key::CLICK;
                        key[key::KEY_0] = key::RELEASE;
                    }
                } else {
                    _keyFilter = key::CHECKING;
                    key[key::KEY_0] = key[key::KEY_1] = key::RELEASE;
                }
            }
            break;
        case key::RELEASED: if (!getAnyKey()) _keyFilter = key::CHECKING;
            break;
        default: break;
    }
}

/**
 * @brief default key tester. 默认按键测试函数
 */
void HAL::_keyTest() {
    // if (getAnyKey()) {
    //     for (uint8_t i = 0; i < key::KEY_NUM; i++) {
    //         if (key[i] == key::CLICK) {
    //             //do something when key clicked
    //             if (i == 0) break;
    //             if (i == 1) break;
    //         } else if (key[i] == key::PRESS) {
    //             //do something when key pressed
    //             if (i == 0) break;
    //             if (i == 1) break;
    //         }
    //     }
    //     for (key::keyAction &k: key) {
    //         k = key::RELEASE;
    //     }
    // }
}
