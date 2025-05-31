#include "LogosGameState.hpp"
#include "../core/GameContext.hpp"
#include "../resources/ResourceManager.hpp"
#include <iostream>

LogosGameState::LogosGameState(GameContext* gameContext)
    : gameContext_(gameContext)
    , engineTexture_(nullptr)
    , enginePartialTexture_(nullptr)
    , engineSonicTexture_(nullptr)
    , loaded_(false)
    , finished_(false)
    , phase_(Phase::Loading)
    , timer_(0)
    , sonicX_(-256)
    , sonicVX_(100)
    , sonicFrame_(0)
    , smallSonic_(true)
    , fadeOpacity_(0.0f)
    , font_(nullptr)
{}

LogosGameState::~LogosGameState() {
    if (engineTexture_) SDL_DestroyTexture(engineTexture_);
    if (enginePartialTexture_) SDL_DestroyTexture(enginePartialTexture_);
    if (engineSonicTexture_) SDL_DestroyTexture(engineSonicTexture_);
}

bool LogosGameState::Initialize() {
    engineTexture_ = ResourceManager::GetInstance().LoadTexture("ENGINE.png");
    enginePartialTexture_ = ResourceManager::GetInstance().LoadTexture("ENGINE/PARTIAL.png");
    engineSonicTexture_ = ResourceManager::GetInstance().LoadTexture("ENGINE/SONIC.png");

    if (!engineTexture_ || !enginePartialTexture_ || !engineSonicTexture_) {
        std::cerr << "Failed to load one or more engine logo resources" << std::endl;
        return false;
    }
    loaded_ = true;
    phase_ = Phase::SonicIn;
    timer_ = 8;
    sonicX_ = -256;
    sonicVX_ = 100;
    sonicFrame_ = 0;
    smallSonic_ = true;
    fadeOpacity_ = 0.0f;
    finished_ = false;

    font_ = std::make_unique<BitmapFont>();
    if (!font_->Load("data/SONICORCA/FONTS/HUD.font", gameContext_->GetRenderer(), "data/SONICORCA/FONTS/HUD")) {
        std::cerr << "Failed to load HUD font" << std::endl;
        return false;
    }
    if (!font_->LoadOverlay("data/SONICORCA/FONTS/HUD/OVERLAYSILVER.png", gameContext_->GetRenderer())) {
        std::cerr << "Failed to load HUD overlay" << std::endl;
        return false;
    }

    return true;
}

void LogosGameState::Update() {
    if (!loaded_ || finished_) return;

    switch (phase_) {
        case Phase::SonicIn:
            if (timer_ > 0) { timer_--; return; }
            sonicX_ += sonicVX_;
            sonicFrame_ = (sonicFrame_ + 1) % 8;
            if (sonicX_ >= 2176) {
                smallSonic_ = false;
                phase_ = Phase::SonicOut;
                sonicX_ = 2944;
                sonicVX_ = -266;
                timer_ = 8;
            }
            break;
        case Phase::SonicOut:
            if (timer_ > 0) { timer_--; return; }
            sonicX_ += sonicVX_;
            sonicFrame_ = (sonicFrame_ + 1) % 8;
            if (sonicX_ <= -1024) {
                phase_ = Phase::SonicIn2;
                sonicX_ = -1024;
                sonicVX_ = 200;
                timer_ = 16;
            }
            break;
        case Phase::SonicIn2:
            if (timer_ > 0) { timer_--; return; }
            sonicX_ += sonicVX_;
            sonicFrame_ = (sonicFrame_ + 1) % 8;
            if (sonicX_ >= 2944) {
                phase_ = Phase::FadeOut;
                timer_ = 90;
            }
            break;
        case Phase::FadeOut:
            if (timer_ > 0) { timer_--; return; }
            fadeOpacity_ += 1.0f / 60.0f;
            if (fadeOpacity_ >= 1.0f) {
                fadeOpacity_ = 1.0f;
                phase_ = Phase::Done;
                finished_ = true;
            }
            break;
        case Phase::Done:
            // nun
            break;
        default:
            break;
    }
}

void LogosGameState::Render() {
    if (!loaded_) return;

    SDL_Renderer* renderer = gameContext_->GetRenderer();
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    if (smallSonic_) {
        drawSmallSonic(renderer, winW, winH);
    } else {
        drawEngineLogo(renderer, winW, winH);
        drawSonic(renderer, winW, winH);
    }

    if (phase_ == Phase::FadeOut || phase_ == Phase::Done) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(fadeOpacity_ * 255));
        SDL_Rect rect = {0, 0, winW, winH};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void LogosGameState::HandleEvent(const SDL_Event& event) {}

void LogosGameState::drawEngineLogo(SDL_Renderer* renderer, int winW, int winH) {
    int texW, texH;
    SDL_QueryTexture(engineTexture_, nullptr, nullptr, &texW, &texH);
    SDL_Rect destRect = { winW/2 - texW/2, winH/2 - texH/2, texW, texH };
    SDL_SetTextureBlendMode(engineTexture_, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(renderer, engineTexture_, nullptr, &destRect);
}

void LogosGameState::drawSmallSonic(SDL_Renderer* renderer, int winW, int winH) {
    int frameW = 1024, frameH = 1120;
    int dispW = 256, dispH = 280;
    SDL_Rect srcRect = { sonicFrame_ * frameW, 0, frameW, frameH };
    SDL_Rect destRect = { sonicX_ - 128, 40, dispW, dispH };
    SDL_SetTextureBlendMode(engineSonicTexture_, SDL_BLENDMODE_BLEND);

    SDL_RendererFlip flip = (sonicVX_ > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, engineSonicTexture_, &srcRect, &destRect, 0, nullptr, flip);
}

void LogosGameState::drawSonic(SDL_Renderer* renderer, int winW, int winH) {
    int frameW = 1024, frameH = 1120;
    SDL_Rect srcRect = { sonicFrame_ * frameW, 0, frameW, frameH };
    SDL_Rect destRect = { sonicX_ - 512, -20, frameW, frameH };
    SDL_SetTextureBlendMode(engineSonicTexture_, SDL_BLENDMODE_BLEND);

    SDL_RendererFlip flip = (sonicVX_ > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, engineSonicTexture_, &srcRect, &destRect, 0, nullptr, flip);
}
