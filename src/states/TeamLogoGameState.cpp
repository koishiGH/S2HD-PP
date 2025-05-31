#include "TeamLogoGameState.hpp"
#include <core/GameContext.hpp>
#include <resources/ResourceManager.hpp>
#include <iostream>

TeamLogoGameState::TeamLogoGameState(GameContext* gameContext)
    : gameContext_(gameContext)
    , logoTexture_(nullptr)
    , loaded_(false)
    , opacity_(0.0f)
    , showTimer_(SHOW_TIME)
    , finished_(false)
    , phase_(Phase::FadingIn)
{
}

TeamLogoGameState::~TeamLogoGameState() {
    if (logoTexture_) {
        SDL_DestroyTexture(logoTexture_);
        logoTexture_ = nullptr;
    }
}

bool TeamLogoGameState::Initialize() {
    logoTexture_ = ResourceManager::GetInstance().LoadTexture("TEAMLOGO.png");
    if (!logoTexture_) {
        std::cerr << "Failed to load TEAMLOGO.png" << std::endl;
        return false;
    }
    loaded_ = true;
    opacity_ = 0.0f;
    showTimer_ = SHOW_TIME;
    finished_ = false;
    phase_ = Phase::FadingIn;
    return true;
}

void TeamLogoGameState::Update() {
    if (!loaded_ || finished_) return;

    switch (phase_) {
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

void TeamLogoGameState::HandleEvent(const SDL_Event& event) {}

void TeamLogoGameState::Render() {
    if (!loaded_ || !logoTexture_) return;

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
    
    int logoW, logoH;
    SDL_QueryTexture(logoTexture_, nullptr, nullptr, &logoW, &logoH);

    SDL_Rect logoRect;
    logoRect.w = logoW;
    logoRect.h = logoH;
    logoRect.x = drawX + (drawW - logoW) / 2;
    logoRect.y = drawY + (drawH - logoH) / 2;

    SDL_SetTextureBlendMode(logoTexture_, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(logoTexture_, static_cast<Uint8>(opacity_ * 255));

    SDL_RenderCopy(renderer, logoTexture_, nullptr, &logoRect);
}

bool TeamLogoGameState::IsFinished() const {
    return finished_;
} 