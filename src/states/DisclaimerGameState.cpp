#include "DisclaimerGameState.hpp"
#include "../core/GameContext.hpp"
#include "../resources/ResourceManager.hpp"
#include <iostream>

DisclaimerGameState::DisclaimerGameState(GameContext* gameContext)
    : gameContext_(gameContext)
    , disclaimerTexture_(nullptr)
    , loaded_(false)
    , opacity_(0.0f)
    , showTimer_(SHOW_TIME)
    , waitTimer_(WAIT_TIME)
    , fadingIn_(false)
    , fadingOut_(false)
    , finished_(false)
    , phase_(Phase::Waiting)
{
    phase_ = Phase::Waiting;
}

DisclaimerGameState::~DisclaimerGameState() {
    if (disclaimerTexture_) {
        SDL_DestroyTexture(disclaimerTexture_);
        disclaimerTexture_ = nullptr;
    }
}

bool DisclaimerGameState::Initialize() {
    disclaimerTexture_ = ResourceManager::GetInstance().LoadTexture("DISCLAIMER.png");
    if (!disclaimerTexture_) {
        std::cerr << "Failed to load DISCLAIMER.png" << std::endl;
        return false;
    }
    loaded_ = true;
    return true;
}

void DisclaimerGameState::Update() {
    if (!loaded_ || finished_) return;

    switch (phase_) {
        case Phase::Waiting:
            if (waitTimer_ > 0) {
                --waitTimer_;
            } else {
                phase_ = Phase::FadingIn;
                opacity_ = 0.0f;
            }
            break;
        case Phase::FadingIn:
            opacity_ += FADE_SPEED;
            if (opacity_ >= 1.0f) {
                opacity_ = 1.0f;
                phase_ = Phase::Showing;
            }
            break;
        case Phase::Showing:
            if (showTimer_ > 0) {
                --showTimer_;
            } else {
                phase_ = Phase::FadingOut;
            }
            break;
        case Phase::FadingOut:
            opacity_ -= FADE_SPEED;
            if (opacity_ <= 0.0f) {
                opacity_ = 0.0f;
                phase_ = Phase::Done;
                finished_ = true;
            }
            break;
        case Phase::Done:
            // nun
            break;
    }
}

void DisclaimerGameState::HandleEvent(const SDL_Event& event) {}

void DisclaimerGameState::Render() {
    if (!loaded_ || !disclaimerTexture_) return;

    SDL_Renderer* renderer = gameContext_->GetRenderer();

    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    float targetAspect = 1280.0f / 720.0f;
    float windowAspect = (float)winW / (float)winH;
    int drawW, drawH, drawX, drawY;
    if (windowAspect > targetAspect) {
        drawH = winH;
        drawW = (int)(winH * targetAspect);
        drawX = (winW - drawW) / 2;
        drawY = 0;
    } else {
        drawW = winW;
        drawH = (int)(winW / targetAspect);
        drawX = 0;
        drawY = (winH - drawH) / 2;
    }

    int texW, texH;
    SDL_QueryTexture(disclaimerTexture_, nullptr, nullptr, &texW, &texH);

    SDL_Rect destRect;
    destRect.w = texW;
    destRect.h = texH;
    destRect.x = drawX + (drawW - texW) / 2;
    destRect.y = drawY + (drawH - texH) / 2;

    SDL_SetTextureBlendMode(disclaimerTexture_, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(disclaimerTexture_, static_cast<Uint8>(opacity_ * 255));

    SDL_RenderCopy(renderer, disclaimerTexture_, nullptr, &destRect);
}
