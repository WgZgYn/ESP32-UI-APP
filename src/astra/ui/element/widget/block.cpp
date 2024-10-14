//
// Created by 20495 on 24-10-10.
//

#include "block.h"

#include <algorithm>
#include <hal/hal.h>

void block::drawLine() {
}

void applyRotation(const float rotationMatrix[3][3], float points[8][3]) {
    float np[8][3]{}; // 临时数组保存旋转后的坐标
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            float sum = 0;
            for (int k = 0; k < 3; k++) {
                sum += rotationMatrix[j][k] * points[i][k];
            }
            np[i][j] = sum;
        }
    }
    // 更新原始点的坐标
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
            points[i][j] = np[i][j];
        }
    }
}

void block::rotateX() {
    constexpr static float rx[3][3]{
        {1, 0, 0},
        {0, std::cos(d), -std::sin(d)},
        {0, std::sin(d), std::cos(d)}
    };
    applyRotation(rx, points);
}

void block::rotateY() {
    constexpr static float ry[3][3]{
        {std::cos(d), 0, std::sin(d)},
        {0, 1, 0},
        {-std::sin(d), 0, std::cos(d)}
    };
    applyRotation(ry, points);
}

void block::rotateZ() {
    constexpr static float rz[3][3]{
        {std::cos(d), -std::sin(d), 0},
        {std::sin(d), std::cos(d), 0},
        {0, 0, 1}
    };
    applyRotation(rz, points);
}

void block::update() {
    HAL::canvasClear();
    auto Height = sys::cfg.screenHeight;
    auto Width = sys::cfg.screenWeight;
    auto CenterY = Height / 2;
    auto CenterX = Width / 2;
    float Scale = std::min(CenterX, CenterY) / 2.;

    for (int i = 0; i < 4; i++) {
        HAL::drawLine(points[i][0] * Scale + CenterX,
                      points[i][1] * Scale + CenterY,
                      points[i + 4][0] * Scale + CenterX,
                      points[i + 4][1] * Scale + CenterY);
    }

    // 顶层的水平边 (0-1, 1-2, 2-3, 3-0)
    for (int i = 0; i < 4; i++) {
        int next = (i + 1) % 4;
        HAL::drawLine(points[i][0] * Scale + CenterX,
                      points[i][1] * Scale + CenterY,
                      points[next][0] * Scale + CenterX,
                      points[next][1] * Scale + CenterY);
    }

    // 底层的水平边 (4-5, 5-6, 6-7, 7-4)
    for (int i = 4; i < 8; i++) {
        int next = (i + 1) % 4 + 4;
        HAL::drawLine(points[i][0] * Scale + CenterX,
                      points[i][1] * Scale + CenterY,
                      points[next][0] * Scale + CenterX,
                      points[next][1] * Scale + CenterY);
    }

    HAL::canvasUpdate();
    HAL::delay(10);
    rotateX();
    rotateY();
    rotateZ();
}
