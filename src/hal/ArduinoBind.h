//
// Created by 20495 on 24-10-6.
//

#ifndef ARDUINOBIND_H
#define ARDUINOBIND_H

class ARDUINOBIND: virtual public HAL {
    void _delay(unsigned long _mill) override {
        ::delay(_mill); // specify the HAL's delay and Arduino's delay
    }

    unsigned long _millis() override {
        return millis();
    }

    unsigned long _getTick() override {
        // return millis();
        return micros() / 1000;
    }

    /**optional**/
    void _setRandomSeed() override {
        return randomSeed(analogRead(A0));
    }

    unsigned long _random() override {
        return ::random();
    }


};

#endif //ARDUINOBIND_H
