//
// Created by 20495 on 24-10-7.
//

#include "Camera.h"

#include <cmath>
#include <HardwareSerial.h>

#include "Selector.h"
#include "Menu.h"

namespace astra {
    namespace ui {
        //这里的坐标应该都是负的 因为最终渲染的时候是加上摄像机的坐标
        //所以说比如想显示下一页 应该是item本身的坐标减去摄像机的坐标 这样才会让item向上移动
        //一个办法是用户传进来正的坐标 但是在摄像机内部 所有坐标都取其相反数 负的

        Camera::Camera(const float _x = 0.0f, const float _y = 0.0f) {
            this->xInit = 0 - _x;
            this->yInit = 0 - _y;

            this->x = 0 - _x;
            this->y = 0 - _y;
        }

        /**
         *
         * @param _x
         * @param _y
         * @return 0: in view, 1: upper, 2: lower
         */
        uint8_t Camera::outOfView(const float _x, const float _y) const {
            if (_x < 0 - this->x || _y < 0 - this->y) return 1;
            if (_x > (0 - this->x) + static_cast<float>(sys::cfg.screenWeight) - 1
                || _y > (0 - this->y) + static_cast<float>(sys::cfg.screenHeight) - 1)
                return 2;
            return 0;
        }

        std::vector<float> Camera::getPosition() const {
            return {x, y};
        }

        // /**
        //  * @brief
        //  * @param _pos
        //  * @param _posTrg
        //  * @param _speed
        //  *
        //  * @note only support in loop. 仅支持在循环内执行
        //  */
        void Camera::go(const float _x, const float _y) {
            moving = true;
            animation(&this->x, (0 - _x), ui::cfg.cameraAnimationSpeed);
            animation(&this->y, (0 - _y), ui::cfg.cameraAnimationSpeed);
            if (this->x == 0 - _x && this->y == 0 - _y) moving = false;
        }

        void Camera::goDirect(const float _x, const float _y) {
            this->x = 0 - _x;
            this->y = 0 - _y;
        }

        void Camera::goHorizontal(const float _x) {
            moving = true;
            animation(&this->x, 0 - _x, ui::cfg.cameraAnimationSpeed);
            if (this->x == 0 - _x) moving = false;
        }

        void Camera::goVertical(const float _y) {
            moving = true;
            animation(&this->y, 0 - _y, ui::cfg.cameraAnimationSpeed);
            if (this->y == 0 - _y) moving = false;
        }

        void Camera::goToNextPageItem() {
            moving = true;
            animation(&y, y - static_cast<float>(sys::cfg.screenHeight), ui::cfg.cameraAnimationSpeed);
            if (this->y == y - static_cast<float>(sys::cfg.screenHeight)) moving = false;
        }

        void Camera::goToPreviewPageItem() {
            moving = true;
            animation(&y, y + static_cast<float>(sys::cfg.screenHeight), ui::cfg.cameraAnimationSpeed);
            if (this->y == y + static_cast<float>(sys::cfg.screenHeight)) moving = false;
        }

        void Camera::goToListItemPage(const uint8_t _index) {
            static const uint8_t maxItemPerScreen =
                    sys::cfg.screenHeight / static_cast<uint8_t>(ui::cfg.listLineHeight);
            uint8_t _page = 0;

            moving = true;

            if (_index == 0) _page = 0;
            else if (_index % maxItemPerScreen == 0) _page = _index / maxItemPerScreen;
            else _page = std::floor(_index / maxItemPerScreen);
            go(0, static_cast<float>(_page * sys::cfg.screenHeight));

            if (this->y == static_cast<float>(_page * sys::cfg.screenHeight)) moving = false;
        }

        void Camera::goToListItemRolling(const std::vector<float> &_posSelector) {
            //这是一个让页面在一定情况下向下或向上滚动一行的函数
            //当index向上超越了一个屏幕可以显示的内容 就要向上滚动一行 滚动到以当前选择项为第一项的页面
            //当index向下超越了一个屏幕可以显示的内容 就要向下滚动一行 滚动到以当前选择项为最后一项的页面
            //正常情况下 不移动

            //如果不碰边界 不更新端点
            //端点惰性赋值
            //计算出每页第一行的index和最后一行的index
            //碰到下边界就向下滚动 碰到上边界就向上滚动

            //最开始左端点是0 右端点是max-1
            //index超过右端点 就向下滚动index-右端点行 同时左右端点都加上index-右端点

            static uint8_t direction = 0; //0: no roll, 1: up, 2: down

            moving = true;
            if (outOfView(_posSelector[0], _posSelector[1]) == 1) direction = 1;
            if (outOfView(_posSelector[0], _posSelector[1]) == 2) direction = 2;

            if (direction == 1) {
                go(_posSelector[0], _posSelector[1]);
                if (this->x == 0 - _posSelector[0] && this->y == 0 - _posSelector[1]) direction = 0;
            }
            if (direction == 2) {
                go(_posSelector[0],
                   _posSelector[1] + ui::cfg.listLineHeight - static_cast<float>(sys::cfg.screenHeight));
                if (this->x == 0 - _posSelector[0] && this->y == 0 - (
                        _posSelector[1] + ui::cfg.listLineHeight - static_cast<float>(sys::cfg.screenHeight)))
                    direction = 0;
            }

            if (!outOfView(_posSelector[0], _posSelector[1])) moving = false;
        }

        void Camera::goToTileItem(const uint8_t _index) {
            moving = true;
            go(static_cast<float>(_index) * (ui::cfg.tilePicWidth + ui::cfg.tilePicMargin), 0);
            if (this->x == 0 - static_cast<float>(_index) * (ui::cfg.tilePicWidth + ui::cfg.tilePicMargin))
                moving = false;
        }

        bool Camera::isMoving() const {
            return moving;
        }

        void Camera::reset() {
            moving = true;
            animation(&this->x, xInit, ui::cfg.cameraAnimationSpeed);
            animation(&this->y, yInit, ui::cfg.cameraAnimationSpeed);
            if (this->x == xInit && this->y == yInit) moving = false;
        }

        void Camera::update(Menu *_menu, Selector *_selector) {
            // 不完善

            if (dynamic_cast<ListMenu *>(_menu)) {
                if (ui::cfg.listPageTurningMode == 0) goToListItemPage(_menu->selectIndex);
                else if (ui::cfg.listPageTurningMode == 1) goToListItemRolling(_selector->getPosition());
            } else if (dynamic_cast<TileMenu *>(_menu)) {
                goToTileItem(_menu->selectIndex);
            }


            // switch (_menu->getType()) {
            //     case ItemType::LIST_MENU:
            //         if (ui::cfg.listPageTurningMode == 0) goToListItemPage(_menu->selectIndex);
            //         else if (ui::cfg.listPageTurningMode == 1) goToListItemRolling(_selector->getPosition());
            //         break;
            //     case ItemType::TILE_MENU:
            //         goToTileItem(_menu->selectIndex);
            //         break;
            //     default:
            //         break;
            // }
        }
    }
}
