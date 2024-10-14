//
// Created by 20495 on 24-10-6.
//

#ifndef SELECTOR_H
#define SELECTOR_H

#include "Menu.h"
#include "Animation.h"

namespace astra {
    namespace ui {
        class Menu;

        class Selector final : public Animation {
            Menu *menu = nullptr;
            Selector() = default;

        public:
            //列表页中就是选择框的坐标 磁贴页中就是大框的坐标
            float x = 0, xTrg = 0;
            float y = 0, yTrg = 0;

            struct {
                float w = 0, wTrg = 0;
                float h = 0, hTrg = 0;
            };


            struct {
                float yText = 0, yTextTrg = 0; //磁贴页标题坐标
            };


            //最牛逼的来了 在磁贴中 文字和大框就是selector
            //这样就可以弄磁贴的文字出现动画了
            ////todo 在磁贴中 选择的时候 摄像机和selector都要移动 磁贴的selector是一个空心方框 + 底部的字体

            std::vector<float> getPosition();


            void last();

            void next();

            void go(uint8_t _index);

            bool inject(Menu *_menu); //inject menu instance to prepare for render.
            bool destroy(); //destroy menu instance.

            void render(const std::vector<float> &_camera);

            static Selector &getInstance() {
                static Selector instance;
                return instance;
            }
        };
    }
}


#endif //SELECTOR_H
