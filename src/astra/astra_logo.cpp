#include <vector>
#include "astra_logo.h"

#include "config/config.h"
#include "utils/utils.h"

namespace astra {
    /***
     *   *     *      *         *
     *    *     powered by *    *
     *       * Astra UI *
     *  *       *     *     *
     *     *  *     *           *
     */


    void drawLogo(const uint16_t _time) {
        auto animation = [](float &_pos, float _posTrg, float _speed) -> void {
            if (_pos != _posTrg) {
                if (utils::abs_diff(_pos, _posTrg) < 0.15f) _pos = _posTrg;
                else _pos += (_posTrg - _pos) / ((100 - _speed) / 1.0f);
            }
        };

        static bool onRender = true;
        static bool isInit = false;

        static uint16_t time = 0;

        while (onRender) {
            time++;

            static std::vector<float> yStars;
            static std::vector<float> yStarsTrg;
            static std::vector<float> xStars;

            static auto text = "astra UI";
            static auto copyRight = "powered by";

            HAL::setFont(ui::cfg.logoTitleFont);
            static float xTitle = static_cast<float>(sys::cfg.screenWeight - HAL::getFontWidth(text)) / 2;
            HAL::setFont(ui::cfg.logoCopyRightFont);
            static float xCopyRight = static_cast<float>(sys::cfg.screenWeight - HAL::getFontWidth(copyRight)) / 2;
            HAL::setFont(ui::cfg.mainFont);

            //都是左上角坐标
            static float yTitle = 0 - ui::cfg.logoTextHeight - 1;
            static float yCopyRight = 0 - ui::cfg.logoCopyRightHeight - 1;
            static float yTitleTrg = 0;
            static float yCopyRightTrg = 0;

            static float xBackGround = 0;
            static float yBackGround = 0 - static_cast<float>(sys::cfg.screenHeight) - 1;

            static float yBackGroundTrg = 0;

            if (time < _time) {
                yBackGroundTrg = 0;
                //星星坐标初始化 注意星星的坐标代表其中心点的位置 注意仅初始化一次
                if (!isInit) {
                    yStars.clear();
                    yStarsTrg.clear();
                    xStars.clear();

                    for (uint8_t i = 0; i < ui::cfg.logoStarNum; i++) {
                        //设置随机种子
                        HAL::setRandomSeed();

                        yStars.push_back(0 - ui::cfg.logoStarLength - 1);

                        //产生从1到screenHeight的随机数
                        yStarsTrg.push_back(
                            1.0f + static_cast<float>(
                                HAL::random() % static_cast<uint16_t>(
                                    static_cast<float>(sys::cfg.screenHeight) - 2 * ui::cfg.logoStarLength - 2 + 1)));
                        //产生从1到screenWeight的随机数
                        xStars.push_back(
                            1.0f + static_cast<float>(
                                HAL::random() % static_cast<uint16_t>(
                                    static_cast<float>(sys::cfg.screenWeight) - 2 * ui::cfg.logoStarLength - 2 + 1)));
                    }
                    isInit = true;
                }
                yTitleTrg = static_cast<float>(sys::cfg.screenHeight) / 2 - ui::cfg.logoTextHeight / 2; //居中
                yCopyRightTrg = yTitleTrg - ui::cfg.logoCopyRightHeight - 4;
            } else {
                //一起退场
                yBackGroundTrg = 0 - static_cast<float>(sys::cfg.screenHeight) - 1;
                yStarsTrg.assign(ui::cfg.logoStarNum, 0 - ui::cfg.logoStarLength - 1);
                yTitleTrg = 0 - ui::cfg.logoTextHeight - 1;
                yCopyRightTrg = 0 - ui::cfg.logoCopyRightHeight - 1;
            }

            //遮罩先进场 然后是星星 然后是文字
            //一起退场
            HAL::canvasClear();

            HAL::setDrawType(0);
            //遮罩
            HAL::drawBox(xBackGround, yBackGround, HAL::getSystemConfig().screenWeight,
                         HAL::getSystemConfig().screenHeight);
            animation(yBackGround, yBackGroundTrg, ui::cfg.logoAnimationSpeed);
            HAL::setDrawType(1);
            HAL::drawHLine(0, yBackGround + static_cast<float>(sys::cfg.screenHeight),
                           HAL::getSystemConfig().screenWeight);

            //画星星
            for (uint8_t i = 0; i < ui::cfg.logoStarNum; i++) {
                HAL::drawHLine(xStars[i] - ui::cfg.logoStarLength - 1, yStars[i], ui::cfg.logoStarLength);
                HAL::drawHLine(xStars[i] + 2, yStars[i], ui::cfg.logoStarLength);
                HAL::drawVLine(xStars[i], yStars[i] - ui::cfg.logoStarLength - 1, ui::cfg.logoStarLength);
                HAL::drawVLine(xStars[i], yStars[i] + 2, ui::cfg.logoStarLength);

                animation(yStars[i], yStarsTrg[i], ui::cfg.logoAnimationSpeed);
            }

            HAL::setFont(ui::cfg.logoTitleFont);
            HAL::drawEnglish(xTitle, yTitle + ui::cfg.logoTextHeight, text);
            HAL::setFont(ui::cfg.logoCopyRightFont);
            HAL::drawEnglish(xCopyRight, yCopyRight + ui::cfg.logoCopyRightHeight, copyRight);
            animation(yTitle, yTitleTrg, ui::cfg.logoAnimationSpeed);
            animation(yCopyRight, yCopyRightTrg, ui::cfg.logoAnimationSpeed);

            HAL::canvasUpdate();

            if (time >= _time && yBackGround == 0 - static_cast<float>(sys::cfg.screenHeight) - 1) onRender = false;
        }
    }
}
