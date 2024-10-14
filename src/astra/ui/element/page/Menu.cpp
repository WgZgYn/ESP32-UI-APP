#include "Menu.h"

#include <cmath>
#include <astra/config/config.h>
#include <hal/hal.h>
#include "Selector.h"

namespace astra {
    namespace ui {
        Menu::Menu()
            : camera(&Camera::getInstance()), selector(&Selector::getInstance()) {
        }

        bool Menu::onOpen() {
            init(camera->getPosition());
            selector->inject(this);
            if (itemSize() == 0) {
                return false;
            }
            return true;
        }

        void Menu::onLoop() {
            render(camera->getPosition());
            selector->render(camera->getPosition());
            camera->update(this, selector);
        }


        // Called after Children be placed.
        void TileMenu::init(const std::vector<float> &_camera) {
            // entryAnimation();
            //受展开开关影响的坐标初始化
            if (cfg.tileUnfold) {
                for (Child &_iter: items)
                    _iter.position.x = _camera[0] - cfg.tilePicWidth; //unfold from left.
                positionForeground.wBar = 0; //bar unfold from left.
            } else {
                for (Child &_iter: items)
                    _iter.position.x = _iter.position.xTrg;
                positionForeground.wBar = positionForeground.wBarTrg;
            }

            //position.y = -cfg.tilePicHeight * 2;

            //始终执行的坐标初始化
            //底部箭头和虚线的初始化
            positionForeground.yArrow = sys::cfg.screenHeight;
            positionForeground.yDottedLine = sys::cfg.screenHeight;

            //顶部进度条的从上方滑入的初始化
            positionForeground.yBar = 0 - cfg.tileBarHeight; //注意这里是坐标从屏幕外滑入 而不是height从0变大
        }

        void TileMenu::render(const std::vector<float> &_camera) {
            // Item::updateConfig();

            HAL::setDrawType(1);


            for (Child &_iter: items) {
                //draw pic.
                HAL::drawBMP(_iter.position.x + _camera[0], ui::cfg.tilePicTopMargin + _camera[1],
                             ui::cfg.tilePicWidth, ui::cfg.tilePicHeight, _iter.tile.data());
                //这里的xTrg在addItem的时候就已经确定了
                animation(&_iter.position.x, _iter.position.xTrg, ui::cfg.tileAnimationSpeed);
            }

            //draw bar.
            //在屏幕最上方 两个像素高
            positionForeground.wBarTrg = static_cast<float>(selectIndex + 1) * (
                                             static_cast<float>(sys::cfg.screenWeight) / static_cast<float>(
                                                 itemSize()));
            HAL::drawBox(0, positionForeground.yBar, positionForeground.wBar, ui::cfg.tileBarHeight);

            //draw left arrow.
            HAL::drawHLine(ui::cfg.tileArrowMargin, positionForeground.yArrow, ui::cfg.tileArrowWidth);
            HAL::drawPixel(ui::cfg.tileArrowMargin + 1, positionForeground.yArrow + 1);
            HAL::drawPixel(ui::cfg.tileArrowMargin + 2, positionForeground.yArrow + 2);
            HAL::drawPixel(ui::cfg.tileArrowMargin + 1, positionForeground.yArrow - 1);
            HAL::drawPixel(ui::cfg.tileArrowMargin + 2, positionForeground.yArrow - 2);

            //draw right arrow.
            HAL::drawHLine(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.tileArrowWidth - ui::cfg.tileArrowMargin,
                           positionForeground.yArrow, ui::cfg.tileArrowWidth);
            HAL::drawPixel(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.tileArrowWidth,
                           positionForeground.yArrow + 1);
            HAL::drawPixel(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.tileArrowWidth - 1,
                           positionForeground.yArrow + 2);
            HAL::drawPixel(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.tileArrowWidth,
                           positionForeground.yArrow - 1);
            HAL::drawPixel(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.tileArrowWidth - 1,
                           positionForeground.yArrow - 2);

            //draw left button.
            HAL::drawHLine(ui::cfg.tileBtnMargin, positionForeground.yArrow + 2, 9);
            HAL::drawBox(ui::cfg.tileBtnMargin + 2, positionForeground.yArrow + 2 - 4, 5, 4);

            //draw right button.
            HAL::drawHLine(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.tileBtnMargin - 9,
                           positionForeground.yArrow + 2, 9);
            HAL::drawBox(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.tileBtnMargin - 9 + 2,
                         positionForeground.yArrow + 2 - 4, 5, 4);

            //draw dotted line.
            HAL::drawHDottedLine(0, positionForeground.yDottedLine, sys::cfg.screenWeight);

            animation(&positionForeground.yDottedLine, positionForeground.yDottedLineTrg, ui::cfg.tileAnimationSpeed);
            animation(&positionForeground.yArrow, positionForeground.yArrowTrg, ui::cfg.tileAnimationSpeed);

            animation(&positionForeground.wBar, positionForeground.wBarTrg, ui::cfg.tileAnimationSpeed);
            animation(&positionForeground.yBar, positionForeground.yBarTrg, ui::cfg.tileAnimationSpeed);
        }

        // Called after Children be placed.
        void ListMenu::init(const std::vector<float> &_camera) {
            // entryAnimation();
            //受展开开关影响的坐标初始化
            if (ui::cfg.listUnfold) {
                for (Child &_iter: items) _iter.position.y = _camera[1] - cfg.listLineHeight; //text unfold from top.
                positionForeground.hBar = 0; //bar unfold from top.
            } else {
                for (Child &_iter: items) _iter.position.y = _iter.position.yTrg;
                positionForeground.hBar = positionForeground.hBarTrg;
            }

            //始终执行的坐标初始化
            positionForeground.xBar = sys::cfg.screenWeight;
        }

        void ListMenu::render(const std::vector<float> &_camera) {
            // Item::updateConfig();

            HAL::setDrawType(1);

            //allow x > screen height, y > screen weight.
            for (Child &_iter: items) {
                HAL::drawChinese(_iter.position.x + _camera[0],
                                 _iter.position.y + ui::cfg.listTextHeight + ui::cfg.listTextMargin + _camera[1],
                                 _iter.title.c_str());
                //这里的yTrg在addItem的时候就已经确定了
                animation(&_iter.position.y, _iter.position.yTrg, ui::cfg.listAnimationSpeed);
            }

            //draw bar.
            positionForeground.hBarTrg = static_cast<float>(selectIndex + 1) * (
                                             static_cast<float>(sys::cfg.screenHeight) / static_cast<float>(
                                                 itemSize()));
            //画指示线
            HAL::drawHLine(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.listBarWeight, 0, ui::cfg.listBarWeight);
            HAL::drawHLine(static_cast<float>(sys::cfg.screenWeight) - ui::cfg.listBarWeight,
                           static_cast<float>(sys::cfg.screenHeight) - 1,
                           ui::cfg.listBarWeight);
            HAL::drawVLine(
                static_cast<float>(sys::cfg.screenWeight) - std::ceil(static_cast<float>(ui::cfg.listBarWeight) / 2.0f),
                0,
                sys::cfg.screenHeight);
            //draw bar.
            HAL::drawBox(positionForeground.xBar, 0, ui::cfg.listBarWeight, positionForeground.hBar);

            //light mode.
            if (ui::cfg.lightMode) {
                HAL::setDrawType(2);
                HAL::drawBox(0, 0, sys::cfg.screenWeight, sys::cfg.screenHeight);
                HAL::setDrawType(1);
            }

            animation(&positionForeground.hBar, positionForeground.hBarTrg, ui::cfg.listAnimationSpeed);
            animation(&positionForeground.xBar, positionForeground.xBarTrg, ui::cfg.listAnimationSpeed);
        }
    }
}
