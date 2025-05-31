#include "InputManager.hpp"
#include <algorithm>

std::unordered_set<SDL_Scancode> InputManager::currentPressedKeys;
std::unordered_set<SDL_Scancode> InputManager::previousPressedKeys;

static const SDL_Scancode trackedKeys[] = {
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    SDL_SCANCODE_C,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT
};

void InputManager::UpdateKeyStates() {
    const Uint8* state = SDL_GetKeyboardState(NULL);
    previousPressedKeys = currentPressedKeys;
    currentPressedKeys.clear();
    for (SDL_Scancode key : trackedKeys) {
        if (state[key]) {
            currentPressedKeys.insert(key);
        }
    }
}

bool InputManager::justPressed(SDL_Scancode key) {
    return currentPressedKeys.find(key) != currentPressedKeys.end() &&
           previousPressedKeys.find(key) == previousPressedKeys.end();
}

bool InputManager::justReleased(SDL_Scancode key) {
    return previousPressedKeys.find(key) != previousPressedKeys.end() &&
           currentPressedKeys.find(key) == currentPressedKeys.end();
}

bool InputManager::isDown(SDL_Scancode key) {
    return currentPressedKeys.find(key) != currentPressedKeys.end();
}

SDL_Scancode InputManager::GetScancode(GameKey key) {
    switch (key) {
        case KEY_Z: return SDL_SCANCODE_Z;
        case KEY_X: return SDL_SCANCODE_X;
        case KEY_C: return SDL_SCANCODE_C;
        case KEY_ENTER: return SDL_SCANCODE_RETURN;
        case KEY_UP: return SDL_SCANCODE_UP;
        case KEY_DOWN: return SDL_SCANCODE_DOWN;
        case KEY_LEFT: return SDL_SCANCODE_LEFT;
        case KEY_RIGHT: return SDL_SCANCODE_RIGHT;
        default: return SDL_SCANCODE_UNKNOWN;
    }
} 