#pragma once

#include "../../core/GameContext.hpp"
#include "../../resources/ResourceManager.hpp"
#include <SDL2/SDL.h>
#include <memory>

class Background {
public:
    Background(GameContext* context);
    ~Background();

    void Update();
    void Render();
    void Reset();
    void WipeOut();

    bool IsVisible() const { return visible_; }
    void SetVisible(bool visible) { visible_ = visible; }

private:
    GameContext* context_;
    SDL_Texture* backgroundSky_ = nullptr;
    SDL_Texture* backgroundIsland_ = nullptr;
    SDL_Texture* backgroundDeathEgg_ = nullptr;
    SDL_Texture* wipeTexture_ = nullptr;
    
    float backgroundSkyCentreX_ = 1260.0f;
    float backgroundIslandCentreX_ = 1088.0f;
    float backgroundFlash_ = 0.0f;
    int ticks_ = 0;
    int wipeHeight_ = 0;
    bool wipeTransitionActive_ = false;
    bool visible_ = false;

    static constexpr float BACKGROUND_SKY_VELOCITY = -0.05f;
    static constexpr float BACKGROUND_ISLAND_VELOCITY = -0.1f;
}; 