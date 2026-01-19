#pragma once


#include <array>
#include <iostream>

class InputHandler {
public:
    InputHandler() : keyStates({}) {};

    enum KeyStateMasks {
        DOWN = 1,
        PRESSED = 2,
        RELEASED = 4,
    };

    inline void setKeyState(const unsigned char key, const unsigned char state) { keyStates[key] = state; }

    void reset() 
    {
        for (unsigned char& keyState : keyStates)
            keyState &= DOWN;
    }

    inline void clearKeyState(const unsigned char key) { keyStates[key] &= DOWN; }

    inline bool isDown(const unsigned char key) const { return keyStates[key] & DOWN; }
    inline bool wasPressed(const unsigned char key) const { return keyStates[key] & PRESSED; }
    inline bool wasReleased(const unsigned char key) const { return keyStates[key] & RELEASED; }

private:
    std::array<unsigned char, 256> keyStates;
};