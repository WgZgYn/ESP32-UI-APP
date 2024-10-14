//
// Created by Fir on 2024/1/21.
//

#pragma once
#ifndef ASTRA_ASTRA__H
#define ASTRA_ASTRA__H
#include <cstdint>


namespace astra {
    namespace ui {
        enum class ItemType: uint8_t {
            ITEM,
            Lifecycle,
            MENU,
            TILE_MENU,
            LIST_MENU,
            Game,
            Widget,
            Activity,
        };

        class Item {
        public:
            Item *parent = nullptr;

            Item() = default;

            virtual ~Item() = default;

            virtual ItemType getType() const {
                return ItemType::ITEM;
            }
        };


        class LifecycleItem : public Item {
        public:
            ItemType getType() const override {
                return ItemType::Lifecycle;
            }


            virtual void onInit() = 0;
            virtual bool onOpen() = 0;
            virtual void onLoop() = 0;
            virtual void onExit() = 0;
        };
    }
}

#endif //ASTRA_ASTRA__H
