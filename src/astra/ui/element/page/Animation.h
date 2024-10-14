//
// Created by 20495 on 24-10-7.
//

#ifndef ANIMATION_H
#define ANIMATION_H


namespace astra {
    namespace ui {
        class Animation {
        public:
            virtual void entryAnimation() {}

            virtual void exitAnimation();

            virtual void blur();

            virtual void animation(float *_pos, float _posTrg, float _speed);

            virtual ~Animation() = default;
        };
    }
}


#endif //ANIMATION_H
