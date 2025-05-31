#pragma once

#include <SDL2/SDL.h>
#include <unordered_set>

class InputManager {
public:
    static void UpdateKeyStates();
    static bool justPressed(SDL_Scancode key);
    static bool justReleased(SDL_Scancode key);
    static bool isDown(SDL_Scancode key);

    enum GameKey {
        KEY_Z,
        KEY_X,
        KEY_C,
        KEY_ENTER,
        KEY_UP,
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT
    };
    static SDL_Scancode GetScancode(GameKey key);

private:
    static std::unordered_set<SDL_Scancode> currentPressedKeys;
    static std::unordered_set<SDL_Scancode> previousPressedKeys;
}; 