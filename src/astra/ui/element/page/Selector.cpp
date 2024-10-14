//
// Created by 20495 on 24-10-6.
//

#include "Selector.h"

namespace astra {
    namespace ui {
        void Selector::go(const uint8_t _index) {
            // Item::updateConfig();

            if (_index >= menu->itemSize()) {
                return;
            }
            menu->selectIndex = _index;

            if (dynamic_cast<TileMenu *>(menu)) {
                const auto *_menu = dynamic_cast<TileMenu *>(menu);

                xTrg = _menu->items[_index].position.xTrg - ui::cfg.tileSelectBoxMargin;
                yTrg = _menu->items[_index].position.yTrg - ui::cfg.tileSelectBoxMargin;

                yText = sys::cfg.screenHeight; //给磁贴文字归零 从屏幕外滑入
                yTextTrg = static_cast<float>(sys::cfg.screenHeight) - ui::cfg.tileTextBottomMargin;

                wTrg = ui::cfg.tileSelectBoxWidth;
                hTrg = ui::cfg.tileSelectBoxHeight;
            } else if (dynamic_cast<ListMenu *>(menu)) {
                const auto *_menu = dynamic_cast<ListMenu *>(menu);
                xTrg = _menu->items[_index].position.xTrg - ui::cfg.selectorMargin;
                yTrg = _menu->items[_index].position.yTrg;

                wTrg = static_cast<float>(HAL::getFontWidth(_menu->items[_index].title.c_str())) +
                       ui::cfg.listTextMargin * 2;
                hTrg = ui::cfg.listLineHeight;
            }


            // switch (menu->getType()) {
            //     case ItemType::TILE_MENU: {
            //         const auto *_menu = dynamic_cast<TileMenu *>(menu);
            //
            //         xTrg = _menu->items[_index].position.xTrg - ui::cfg.tileSelectBoxMargin;
            //         yTrg = _menu->items[_index].position.yTrg - ui::cfg.tileSelectBoxMargin;
            //
            //         yText = sys::cfg.screenHeight; //给磁贴文字归零 从屏幕外滑入
            //         yTextTrg = static_cast<float>(sys::cfg.screenHeight) - ui::cfg.tileTextBottomMargin;
            //
            //         wTrg = ui::cfg.tileSelectBoxWidth;
            //         hTrg = ui::cfg.tileSelectBoxHeight;
            //     }
            //     break;
            //
            //     case ItemType::LIST_MENU: {
            //         const auto *_menu = dynamic_cast<ListMenu *>(menu);
            //         xTrg = _menu->items[_index].position.xTrg - ui::cfg.selectorMargin;
            //         yTrg = _menu->items[_index].position.yTrg;
            //
            //         wTrg = static_cast<float>(HAL::getFontWidth(_menu->items[_index].title.c_str())) +
            //                ui::cfg.listTextMargin * 2;
            //         hTrg = ui::cfg.listLineHeight;
            //     }
            //     break;
            //     default:
            //         break;
            // }
        }

        bool Selector::inject(Menu *_menu) {
            if (_menu == nullptr) return false;

            //if (this->menu != nullptr) return false;
            this->menu = _menu;

            go(this->menu->selectIndex); //注入之后要初始化选择框的位置
            return true;
        }

        bool Selector::destroy() {
            if (this->menu == nullptr) return false;
            this->menu = nullptr;
            return true;
        }

        void Selector::render(const std::vector<float> &_camera) {
            // Item::updateConfig();

            //实际上 这里已经实现过渡动画了
            animation(&x, xTrg, ui::cfg.selectorXAnimationSpeed);
            animation(&y, yTrg, ui::cfg.selectorYAnimationSpeed);
            animation(&h, hTrg, ui::cfg.selectorHeightAnimationSpeed);
            animation(&w, wTrg, ui::cfg.selectorWidthAnimationSpeed);

            if (dynamic_cast<TileMenu *>(menu)) {
                const auto *_menu = dynamic_cast<TileMenu *>(menu);
                    animation(&yText, yTextTrg, ui::cfg.selectorYAnimationSpeed);

                    //draw text.
                    //文字不受摄像机的影响
                    HAL::setDrawType(1);
                    const char *str = _menu->items[menu->selectIndex].title.c_str();
                    HAL::drawChinese(
                        (static_cast<float>(sys::cfg.screenWeight) - static_cast<float>(HAL::getFontWidth(str))) / 2.0f,
                        yText + ui::cfg.tileTitleHeight, str); // FIXME: in the first frame, this have some problem

                    //draw box.
                    //大框需要受摄像机的影响
                    HAL::setDrawType(2);
                    HAL::drawPixel(x + _camera[0], y + _camera[1]);
                    //左上角
                    HAL::drawHLine(x + _camera[0], y + _camera[1], ui::cfg.tileSelectBoxLineLength + 1);
                    HAL::drawVLine(x + _camera[0], y + _camera[1], ui::cfg.tileSelectBoxLineLength + 1);
                    //左下角
                    HAL::drawHLine(x + _camera[0], y + _camera[1] + h - 1, ui::cfg.tileSelectBoxLineLength + 1);
                    HAL::drawVLine(x + _camera[0], y + _camera[1] + h - ui::cfg.tileSelectBoxLineLength - 1,
                                   ui::cfg.tileSelectBoxLineLength);
                    //右上角
                    HAL::drawHLine(x + _camera[0] + w - ui::cfg.tileSelectBoxLineLength - 1, y + _camera[1],
                                   ui::cfg.tileSelectBoxLineLength);
                    HAL::drawVLine(x + _camera[0] + w - 1, y + _camera[1], ui::cfg.tileSelectBoxLineLength + 1);
                    //右下角
                    HAL::drawHLine(x + _camera[0] + w - ui::cfg.tileSelectBoxLineLength - 1, y + _camera[1] + h - 1,
                                   ui::cfg.tileSelectBoxLineLength);
                    HAL::drawVLine(x + _camera[0] + w - 1, y + _camera[1] + h - ui::cfg.tileSelectBoxLineLength - 1,
                                   ui::cfg.tileSelectBoxLineLength);

                    HAL::drawPixel(x + _camera[0] + w - 1, y + _camera[1] + h - 1);
            } else if(dynamic_cast<ListMenu *>(menu)) {
                //animation(&h, hTrg, ui::cfg.selectorAnimationSpeed);

                //draw select box.
                //受摄像机的影响
                HAL::setDrawType(2);
                HAL::drawRBox(x + _camera[0], y + _camera[1], w, h - 1, ui::cfg.selectorRadius);
                //HAL::drawRBox(x, y, w, ui::cfg.listLineHeight, ui::cfg.selectorRadius);
                HAL::setDrawType(1);
            }
        }

        std::vector<float> Selector::getPosition() {
            return {xTrg, yTrg};
        }

        void Selector::last() {
            uint8_t index = menu->selectIndex;

            if (index == 0) {
                index = menu->itemSize() - 1;
            } else {
                index--;
            }

            go(index);
        }

        void Selector::next() {
            uint8_t index = menu->selectIndex;
            if (index == menu->itemSize() - 1) {
                index = 0;
            } else {
                index++;
            }
            go(index);
        }
    }
}
