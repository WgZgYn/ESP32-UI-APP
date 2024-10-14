//
// Created by 20495 on 24-10-6.
//

#ifndef MENU_H
#define MENU_H
#include <HardwareSerial.h>
#include <string>
#include <vector>
#include <astra/config/config.h>
#include <hal/hal.h>
#include <icon/icon.h>

#include "item.h"
#include "Animation.h"
#include "Camera.h"
#include "Selector.h"

namespace astra {
    namespace ui {
        class Menu : public LifecycleItem, public Animation {
        public:
            struct Position {
                //这里的坐标都是左上角的坐标 但是要记住绘制文字的时候是左下角的坐标 需要再减去字体的高度
                float x, xTrg;
                float y, yTrg;
            };

            //前景元素的坐标
            struct PositionForeground {
                float hBar, hBarTrg; //进度条高度 通用
                float wBar, wBarTrg; //进度条宽度 通用
                float xBar, xBarTrg; //进度条x坐标 通用
                float yBar, yBarTrg; //进度条y坐标 通用

                /*TILE*/
                float yArrow, yArrowTrg;
                float yDottedLine, yDottedLineTrg;
                /*TILE*/
            };

            PositionForeground positionForeground{};
            uint8_t selectIndex = 0;
            Camera *camera;
            Selector *selector;


            // Menu() = default; // This should be commented

            Menu();

            ~Menu() override = default;

            ItemType getType() const override { return ItemType::MENU; }

            void onInit() override {
            }

            bool onOpen() override;

            void onLoop() override;

            void onExit() override {
                exitAnimation();
            }


            virtual void init(const std::vector<float> &_camera) = 0;

            virtual void render(const std::vector<float> &_camera) = 0;

            virtual void deInit() { exitAnimation(); }

            virtual uint8_t itemSize() const = 0;

            virtual Position getItemPosition(uint8_t _index) const = 0;

            virtual Item *parentItem() const { return parent; }

            virtual LifecycleItem *enter() const = 0;
        };

        class TileMenu : public Menu {
            class Child {
            public:
                std::string title;
                const icon::Tile &tile;
                Position position{};
                LifecycleItem *item;

                Child(const char *title, const icon::Tile &pic, LifecycleItem *item): title(title), tile(pic),
                    item(item) {
                }
            };

        public: // try to use friend class;
            std::vector<Child> items;

        public:
            explicit TileMenu() = default;

            ~TileMenu() override = default;

            void add(const char *title, const icon::Tile &pic, LifecycleItem *item = nullptr) {
                items.emplace_back(title, pic, item);
                Child &child = items.back();

                if (child.item != nullptr) {
                    child.item->parent = this;
                }


                child.position.xTrg = static_cast<float>(sys::cfg.screenWeight) / 2 - ui::cfg.tilePicWidth / 2 + (
                                          static_cast<float>(this->itemSize()) - 1)
                                      * (ui::cfg.tilePicMargin + ui::cfg.tilePicWidth);
                child.position.yTrg = ui::cfg.tilePicTopMargin;

                positionForeground.yBarTrg = 0;
                positionForeground.yArrowTrg = static_cast<float>(sys::cfg.screenHeight) - ui::cfg.
                                               tileArrowBottomMargin;
                positionForeground.yDottedLineTrg =
                        static_cast<float>(sys::cfg.screenHeight) - ui::cfg.tileDottedLineBottomMargin;
            }

            ItemType getType() const override { return ItemType::TILE_MENU; }

            void init(const std::vector<float> &_camera) override;

            void render(const std::vector<float> &_camera) override;

            Position getItemPosition(uint8_t _index) const override {
                return items[_index].position;
            }

            uint8_t itemSize() const override {
                return items.size();
            }

            LifecycleItem *enter() const override {
                return items[selectIndex].item;
            }
        };

        class ListMenu : public Menu {
        protected:
            class Child {
            public:
                std::string title;
                Position position{};
                LifecycleItem *item;

                Child(const char *title, LifecycleItem *item): title(title), item(item) {
                }
            };

        public:
            std::vector<Child> items;

            explicit ListMenu() = default;

            ~ListMenu() override = default;

            void add(const char *title, LifecycleItem *item = nullptr) {
                items.emplace_back(title, item);
                Serial.println(title);
                Serial.println("add new item");
                Child &child = items.back();
                if (child.item != nullptr) {
                    child.item->parent = this;
                }

                child.position.xTrg = cfg.listTextMargin;
                child.position.yTrg = (static_cast<float>(itemSize()) - 1) * cfg.listLineHeight;

                positionForeground.xBarTrg = static_cast<float>(sys::cfg.screenWeight) - cfg.listBarWeight;
            }

            ItemType getType() const override { return ItemType::LIST_MENU; }

            void init(const std::vector<float> &_camera) override;

            void render(const std::vector<float> &_camera) override;

            uint8_t itemSize() const override {
                return items.size();
            }

            Position getItemPosition(const uint8_t _index) const override {
                return items[_index].position;
            }

            LifecycleItem *enter() const override {
                return items[selectIndex].item;
            }
        };
    }
}


#endif //MENU_H
