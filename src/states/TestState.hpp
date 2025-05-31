#pragma once

#include <SDL2/SDL.h>
#include <resources/ResourceManager.hpp>

class TestState {
public:
    TestState();
    ~TestState();

    bool Initialize(SDL_Renderer* renderer);
    void Update();
    void Render(SDL_Renderer* renderer);

private:
    SDL_Texture* logoTexture_;
    SDL_Rect logoRect_;
}; 