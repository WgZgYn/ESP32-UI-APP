#pragma once
#include <app/app.h>
#include <astra/ui/element/page/item.h>
#include "hal/hal.h"

namespace game {
    class Game : virtual public app::Activity {
    protected:
        bool finish = false;

    public:
        Game() = default;

        ~Game() override = default;

        virtual void restart() = 0;

        void setup() override {
            restart();
        }

        astra::ui::ItemType getType() const override {
            return astra::ui::ItemType::Game;
        }
    };

    namespace GameOfLife {
        constexpr int width = 128;
        constexpr int height = 64;

        template<int WIDTH, int HEIGHT>
        class GameOfLife {
        public:
            enum Status {
                Dead = 0,
                Active = 1
            };

        private:
            constexpr static int max(const int a, const int b) {
                return a > b ? a : b;
            }

            constexpr static int min(const int a, const int b) {
                return a < b ? a : b;
            }

            static void setRandomSeed() {
                HAL::setRandomSeed();
            }

            // [a, b)
            static unsigned long random(const long a, const long b) {
                return HAL::random() % (b - a) + a;
            }


            void init(int lives = width * height / 2) {
                GameOfLife::setRandomSeed();
                while (lives) {
                    int randX = random(0, WIDTH);
                    int randY = random(0, HEIGHT);
                    if (pBoard->slot[randX][randY] == Status::Dead) {
                        pBoard->slot[randX][randY] = Status::Active;
                        --lives;
                    }
                }
            }

            Status update(int x, int y) {
                int judge = -1 * pBoard->slot[x][y];
                for (int i = max(x - 1, 0); i <= min(x + 1, WIDTH - 1); ++i) {
                    for (int k = max(y - 1, 0); k <= min(y + 1, HEIGHT - 1); ++k) {
                        if (pBoard->slot[i][k] == Status::Active)
                            ++judge;
                    }
                }
                if (pBoard->slot[x][y] == Status::Active) {
                    if (judge < 2 || judge > 3)
                        return Status::Dead;
                    else
                        return pBoard->slot[x][y];
                } else {
                    if (judge == 3) {
                        return Status::Active;
                    } else {
                        return Status::Dead;
                    }
                }
            }

        public:
            struct Board {
                Status slot[WIDTH][HEIGHT]{};
            };

            explicit GameOfLife(int lives = width * height / 2) {
                init(lives);
            }

            void restart() {
                for (int i = 0; i < width; i++) {
                    for (int j = 0; j < height; j++) {
                        nBoard->slot[i][j] = Status::Dead;
                        pBoard->slot[i][j] = Status::Dead;
                    }
                }

                init();
            }

            void update() {
                gen += 1;
                for (int i = 0; i < WIDTH; i++) {
                    for (int j = 0; j < HEIGHT; j++) {
                        nBoard->slot[i][j] = update(i, j);
                    }
                }

                Board *temp = nBoard;
                nBoard = pBoard;
                pBoard = temp;
            }

            const Board *get_board_status() const {
                return this->pBoard;
            }

        private:
            Board data, temp;
            Board *pBoard = &data, *nBoard = &temp;
            int gen = 0;
        };


        template class GameOfLife<width, height>;

        class Runner final : public Game {
            Runner() = default;

            using GAME = GameOfLife<width, height>;
            GAME game;
            static bool opened;

        public:
            static Runner &getInstance() {
                static Runner instance;
                return instance;
            }

            void restart() override {
                if (opened) {
                    finish = false;
                    game.restart();
                } else {
                    opened = true;
                }
            }

            void update() override {
                if (!finish) {
                    HAL::canvasClear();
                    const GAME::Board *b = game.get_board_status();
                    for (uint8_t i = 0; i < width; i++) {
                        for (uint8_t j = 0; j < height; j++) {
                            if (b->slot[i][j] == GAME::Status::Active) {
                                HAL::drawPixel(i, j);
                            }
                        }
                    }
                    HAL::canvasUpdate();
                    game.update();
                    HAL::delay(100);
                }
            }
        };
    }
}
