#include "Background.hpp"
#include "TitleResources.hpp"
#include <iostream>

Background::Background(GameContext* context)
    : context_(context) {
    backgroundSky_ = ResourceManager::GetInstance().LoadTexture(TitleResources::BACKGROUND_SKY);
    backgroundIsland_ = ResourceManager::GetInstance().LoadTexture(TitleResources::BACKGROUND_ISLAND);
    backgroundDeathEgg_ = ResourceManager::GetInstance().LoadTexture(TitleResources::BACKGROUND_DEATHEGG);
    wipeTexture_ = ResourceManager::GetInstance().LoadTexture(TitleResources::WIPE);

    if (!backgroundSky_ || !backgroundIsland_ || !backgroundDeathEgg_ || !wipeTexture_) {
        std::cerr << "Failed to load background textures!" << std::endl;
    }
}

Background::~Background() {}

void Background::Update() {
    if (ticks_ == 0) {
        visible_ = true;
        backgroundFlash_ = 1.0f;
    } else {
        backgroundFlash_ = std::max(0.0f, backgroundFlash_ - 1.0f / 32.0f);
    }

    backgroundSkyCentreX_ += BACKGROUND_SKY_VELOCITY;
    backgroundIslandCentreX_ += BACKGROUND_ISLAND_VELOCITY;

    int skyWidth = 0;
    SDL_QueryTexture(backgroundSky_, nullptr, nullptr, &skyWidth, nullptr);
    if (backgroundSkyCentreX_ + skyWidth / 2 < 0) {
        backgroundSkyCentreX_ = skyWidth / 2;
    }

    int islandWidth = 0;
    SDL_QueryTexture(backgroundIsland_, nullptr, nullptr, &islandWidth, nullptr);
    if (backgroundIslandCentreX_ < -islandWidth) {
        backgroundIslandCentreX_ = 1920 + islandWidth;
    }

    if (wipeTransitionActive_) {
        wipeHeight_ += 20;
        if (wipeHeight_ >= 600) {
            wipeTransitionActive_ = false;
        }
    }

    ticks_++;
}

void Background::Render() {
    if (!visible_) return;

    SDL_Renderer* renderer = context_->GetRenderer();

    int skyWidth = 0, skyHeight = 0;
    SDL_QueryTexture(backgroundSky_, nullptr, nullptr, &skyWidth, &skyHeight);
    
    float x = backgroundSkyCentreX_;
    while (x - skyWidth / 2 < 1920) {
        SDL_Rect dest = {static_cast<int>(x - skyWidth / 2), 540 - skyHeight / 2, skyWidth, skyHeight};
        SDL_RenderCopy(renderer, backgroundSky_, nullptr, &dest);
        x += skyWidth;
    }

    int deathEggWidth = 0, deathEggHeight = 0;
    SDL_QueryTexture(backgroundDeathEgg_, nullptr, nullptr, &deathEggWidth, &deathEggHeight);
    SDL_Rect deathEggDest = {1750 - deathEggWidth / 2, 192 - deathEggHeight / 2, deathEggWidth, deathEggHeight};
    SDL_RenderCopy(renderer, backgroundDeathEgg_, nullptr, &deathEggDest);

    int islandWidth = 0, islandHeight = 0;
    SDL_QueryTexture(backgroundIsland_, nullptr, nullptr, &islandWidth, &islandHeight);
    SDL_Rect islandDest = {static_cast<int>(backgroundIslandCentreX_ - islandWidth / 2), 540 - islandHeight / 2, islandWidth, islandHeight};
    SDL_RenderCopy(renderer, backgroundIsland_, nullptr, &islandDest);

    if (backgroundFlash_ > 0.0f) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, static_cast<Uint8>(backgroundFlash_ * 255));
        SDL_RenderFillRect(renderer, nullptr);
    }

    if (wipeHeight_ > 0) {
        int wipeWidth = 0, wipeHeight = 0;
        SDL_QueryTexture(wipeTexture_, nullptr, nullptr, &wipeWidth, &wipeHeight);
        
        SDL_Rect topWipeDest = {0, wipeHeight_ - wipeHeight, wipeWidth, wipeHeight};
        SDL_RenderCopy(renderer, wipeTexture_, nullptr, &topWipeDest);
        
        SDL_Rect bottomWipeDest = {0, 1080 - wipeHeight_, wipeWidth, wipeHeight};
        SDL_RenderCopyEx(renderer, wipeTexture_, nullptr, &bottomWipeDest, 0, nullptr, SDL_FLIP_VERTICAL);
    }
}

void Background::Reset() {
    ticks_ = 0;
    backgroundFlash_ = 0.0f;
    backgroundSkyCentreX_ = 1260.0f;
    backgroundIslandCentreX_ = 1088.0f;
    wipeHeight_ = 0;
    wipeTransitionActive_ = false;
    visible_ = false;
}

void Background::WipeOut() {
    wipeTransitionActive_ = true;
} 