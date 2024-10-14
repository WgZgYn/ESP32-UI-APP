//
// Created by 20495 on 24-10-7.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <cstdint>
#include <vector>

#include "Animation.h"


namespace astra {
    namespace ui {
        class Selector;
        class Menu;

        class Camera final : public Animation {
            float xInit, yInit;

            Camera(float _x, float _y); //build a camera instance with position.
        public:
            float x, y;
            bool moving = false;

            uint8_t outOfView(float _x, float _y) const;

            std::vector<float> getPosition() const;

            //在启动器中新建selector和camera 然后注入menu render
            //在启动器中执行下述方法即可实现视角移动
            //启动器要判断是否超过视角范围 若超过则移动摄像机
            //所有过程中 渲染好的元素绝对坐标都是不变的 只有摄像机的坐标在变
            void go(float _x, float _y);

            void goDirect(float _x, float _y);

            void goHorizontal(float _x);

            void goVertical(float _y);

            void goToNextPageItem();

            void goToPreviewPageItem();

            void goToListItemPage(uint8_t _index);

            void goToListItemRolling(const std::vector<float> &_posSelector);

            void goToTileItem(uint8_t _index);

            bool isMoving() const;

            void reset();

            void update(Menu *_menu, Selector *_selector);


            static Camera &getInstance() {
                static Camera camera(0.0, 0.0);
                return camera;
            }
        };
    }
}


#endif //CAMERA_H
