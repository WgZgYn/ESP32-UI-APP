#pragma once
#include <astra/ui/element/page/item.h>


namespace astra {
    namespace ui {
        class Widget : public LifecycleItem {
        public:
            Widget() = default;

            ~Widget() override = default;

            void onInit() override {
            }

            bool onOpen() override {
                return true;
            }

            void onLoop() override {
                update();
            }

            void onExit() override {
            }



            ItemType getType() const override {
                return ItemType::Widget;
            }

            virtual void update() = 0;
        };
    }
}
