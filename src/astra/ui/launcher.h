//
// Created by Fir on 2024/2/2.
//
#pragma once
#ifndef ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_
#define ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_

#include <app/app.h>

#include "element/page/Camera.h"
#include "element/page/Selector.h"


namespace astra {
    class Launcher : public ui::Animation, public app::Service {
        ui::LifecycleItem *app;

        // ui::Selector& selector = ui::Selector::getInstance();
        // ui::Camera& camera = ui::Camera::getInstance();
        uint64_t time;

    public:
        // 启动一个具有生命周期的部件
        explicit Launcher(ui::LifecycleItem *menu);

        ~Launcher() override;

        void popInfo(const char *_info, uint16_t _time);

        // Only Useful for Menu
        bool open();

        // Should always work well when open is ok
        bool close();

        void setup() override;

        void loop() override;

        // const ui::Camera *getCamera() const { return &camera; }
        // const ui::Selector *getSelector() const { return &selector; }
    };
}

#endif //ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_
