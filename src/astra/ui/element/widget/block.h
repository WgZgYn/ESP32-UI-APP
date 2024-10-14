//
// Created by 20495 on 24-10-10.
//

#ifndef BLOCK_H
#define BLOCK_H
#include <cmath>

#include "widget.h"

class block final : public astra::ui::Widget {
private:
    constexpr static float d = 0.02;

    void drawLine();

public:
    float points[8][3]{
        {1, 1, -1}, {1, -1, -1}, {-1, -1, -1}, {-1, 1, -1},
        {1, 1, 1}, {1, -1, 1}, {-1, -1, 1}, {-1, 1, 1},
    };

    void rotateX();

    void rotateY();

    void rotateZ();

    void update() override;

    static block &getInstance() {
        static block instance;
        return instance;
    }
};


#endif //BLOCK_H
