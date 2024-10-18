//
// Created by Fir on 2024/2/2.
//

#include "launcher.h"

#include <HardwareSerial.h>
#include <astra/astra_logo.h>


void UIConfig();

namespace astra {
    Launcher::Launcher(ui::LifecycleItem *menu)
        : app(menu),
          time(0) {
    }

    Launcher::~Launcher() = default;

    void Launcher::popInfo(const char *_info, const uint16_t _time) {
        const uint64_t beginTime = this->time;
        bool onRender = true;

        while (onRender) {
            time++;

            static float wPop = static_cast<float>(HAL::getFontWidth(_info)) + 2 * ui::cfg.popMargin; //宽度
            static float hPop = static_cast<float>(HAL::getFontHeight()) + 2 * ui::cfg.popMargin; //高度

            static float yPop = 0 - hPop - 8; //从屏幕上方滑入
            static float yPopTrg = 0;
            if (time - beginTime < _time) yPopTrg = (static_cast<float>(sys::cfg.screenHeight) - hPop) / 3; //目标位置 中间偏上
            else yPopTrg = 0 - hPop - 8; //滑出

            static float xPop = (static_cast<float>(sys::cfg.screenWeight) - wPop) / 2; //居中

            HAL::canvasClear();
            /*渲染一帧*/
            app->onLoop();
            /*渲染一帧*/

            HAL::setDrawType(0);
            HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, ui::cfg.popRadius + 2);
            HAL::setDrawType(1); //反色显示
            HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, ui::cfg.popRadius); //绘制一个圆角矩形
            HAL::drawEnglish(xPop + ui::cfg.popMargin,
                             yPop + ui::cfg.popMargin + static_cast<float>(HAL::getFontHeight()), _info); //绘制文字

            HAL::canvasUpdate();

            animation(&yPop, yPopTrg, ui::cfg.popSpeed); //动画

            // 这里条件可以加上一个如果按键按下 就退出
            if (time - beginTime >= _time && yPop == 0 - hPop - 8) onRender = false; //退出条件
        }
    }


    // 由当前 菜单类 页面进入条目的界面
    bool Launcher::open() {
        if (app == nullptr) return false; // 不应该发生
        Serial.println("Launcher::open");

        // 如果当前不是菜单
        const auto *menu = dynamic_cast<ui::Menu *>(app);
        if (menu == nullptr) {
            return false; // 无视open操作
        }

        Serial.println("Launcher::open enter");

        // 进行到菜单条目
        ui::LifecycleItem *next = menu->enter();
        if (next == nullptr) {
            // 空白页
            popInfo("unreferenced page!", 600);
            return false;
        }

        Serial.println("Launcher::open enter lifecycle");
        Serial.println(static_cast<uint8_t>(menu->getType()));

        Serial.println("Launcher::open app exit");
        app->onExit(); // 注销当前页面
        Serial.println("Launcher::open open next");
        if (!next->onOpen()) {
            // 启动下一个页面
            popInfo("can't open!", 600); // 启动失败
            return false;
        }


        Serial.println("Launcher::open switch next");
        this->app = next;
        return true;
    }

    bool Launcher::close() {
        if (app == nullptr) {
            // 不应该发生
            popInfo("unknown error app nullptr!", 600);
            return false;
        }
        app->onExit();
        auto *prev = dynamic_cast<ui::LifecycleItem *>(app->parent);
        if (prev == nullptr) {
            popInfo("no parent page!", 600);
            return false;
        } // This shouldn't happen
        {
            // This shouldn't happen
            auto *page = dynamic_cast<ui::Menu *>(prev);
            if (page != nullptr && page->itemSize() == 0) {
                popInfo("empty page!", 600);
                return false;
            }
        }

        app = prev;
        app->onOpen();
        return true;
    }


    void Launcher::setup() {
        astra::drawLogo(600);
        UIConfig();
        app->onOpen();
    }

    void Launcher::loop() {
        if (!app::App::getInstance().ui) return;

        HAL::canvasClear();

        app->onLoop();

        HAL::canvasUpdate();

        const int code = Serial.read();
        if (code != -1) {
            Serial.println(code);
            switch (code) {
                case '4': {
                    auto *selector = dynamic_cast<ui::Menu *>(app)->selector;
                    if (selector != nullptr) {
                        selector->last();
                    }
                }
                break;

                case '6': {
                    auto *selector = dynamic_cast<ui::Menu *>(app)->selector;
                    if (selector != nullptr) {
                        selector->next();
                    }
                }
                break;

                case '5':
                    Serial.println("open---");
                    open();
                    break;

                case '8':
                    Serial.println("close---");
                    close();
                    break;

                default:
                    break;
            }
        }
    }
}
