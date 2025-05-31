#include "TitleGameState.hpp"
#include "title/Background.hpp"
#include "title/TitleResources.hpp"
#include "../graphics/BitmapFont.hpp"
#include "../input/InputManager.hpp"
#include <iostream>

TitleGameState::TitleGameState(GameContext* context)
    : context_(context), font_(nullptr) {
}

TitleGameState::~TitleGameState() = default;

bool TitleGameState::Initialize() {
    LoadResources();
    return true;
}

void TitleGameState::LoadResources() {
    auto resourcePaths = TitleResources::GetAllResourcePaths();
    for (const auto& path : resourcePaths) {
        if (!ResourceManager::GetInstance().LoadTexture(path)) {
            std::cerr << "Failed to load resource: " << path << std::endl;
        }
    }

    font_ = std::make_unique<BitmapFont>();
    if (!font_->Load("data/SONICORCA/FONTS/HUD.font", context_->GetRenderer(), "data/SONICORCA/FONTS/HUD")) {
        std::cerr << "Failed to load HUD font" << std::endl;
    }

    if (!font_->LoadOverlay("data/SONICORCA/FONTS/HUD/OVERLAYSILVER.png", context_->GetRenderer())) {
        std::cerr << "Failed to load HUD overlay" << std::endl;
    }

    background_ = std::make_unique<Background>(context_);
    uilmao_ = std::make_unique<UserInterface>(context_, this);
    loaded_ = true;
}

void TitleGameState::Update() {
    if (!loaded_) return;

    /*
    // Was done for hud shit lol
    if (InputManager::justPressed(InputManager::GetScancode(InputManager::KEY_Z))) {
        isFinished_ = true;
        return;
    }
    */

    switch (phase_) {
        case TitlePhase::IntroText:
            ticks_++;
            if (ticks_ == INTRO_TEXT_END) {
                phase_ = TitlePhase::FadeToBlack;
                fadingOut_ = true;
            }
            break;
        case TitlePhase::FadeToBlack:
            if (fadeOutOpacity_ > 0.0f) {
                fadeOutOpacity_ -= 0.0166666675f;
            } else {
                phase_ = TitlePhase::WhiteFlash;
                whiteFlashTicks_ = 0;
            }
            break;
        case TitlePhase::WhiteFlash:
            whiteFlashTicks_++;
            if (whiteFlashTicks_ > 15) {
                phase_ = TitlePhase::MainTitle;
            }
            break;
        case TitlePhase::MainTitle:
            background_->Update();
            if (uilmao_) uilmao_->Update();
            break;
    }
}

void TitleGameState::Render() {
    if (!loaded_) return;

    switch (phase_) {
        case TitlePhase::IntroText:
            background_->Render();
            DrawIntroText();
            break;
        case TitlePhase::FadeToBlack:
            background_->Render();
            DrawIntroText();
            if (fadeOutOpacity_ < 1.0f) {
                SDL_SetRenderDrawColor(context_->GetRenderer(), 0, 0, 0, static_cast<Uint8>((1.0f - fadeOutOpacity_) * 255));
                SDL_RenderFillRect(context_->GetRenderer(), nullptr);
            }
            break;
        case TitlePhase::WhiteFlash:
            SDL_SetRenderDrawColor(context_->GetRenderer(), 255, 255, 255, 255);
            SDL_RenderFillRect(context_->GetRenderer(), nullptr);
            break;
        case TitlePhase::MainTitle:
            background_->Render();
            if (uilmao_) uilmao_->Draw();
            break;
    }
}

void TitleGameState::HandleEvent(const SDL_Event& event) {}

void TitleGameState::DrawIntroText() {
    float opacity = 0.0f;
    if (ticks_ >= INTRO_TEXT_START && ticks_ < INTRO_TEXT_FULL) {
        opacity = static_cast<float>(ticks_ - INTRO_TEXT_START) / (INTRO_TEXT_FULL - INTRO_TEXT_START);
    } else if (ticks_ >= INTRO_TEXT_FULL && ticks_ < INTRO_TEXT_HOLD) {
        opacity = 1.0f;
    } else if (ticks_ >= INTRO_TEXT_HOLD && ticks_ < INTRO_TEXT_END) {
        opacity = 1.0f - static_cast<float>(ticks_ - INTRO_TEXT_HOLD) / (INTRO_TEXT_END - INTRO_TEXT_HOLD);
    }

    if (opacity <= 0.0f || !font_) return;

    const std::vector<std::string> introText = {
        "SONIC",
        "AND",
        "MILES \"TAILS\" PROWER",
        "IN"
    };

    int y = 540 - (static_cast<int>(introText.size()) * 128) / 2;
    SDL_Renderer* renderer = context_->GetRenderer();
    for (const auto& text : introText) {
        int textWidth = font_->GetTextWidth(text);
        font_->RenderText(renderer, text, 960 - textWidth / 2, y, static_cast<Uint8>(opacity * 255));
        y += 128;
    }
}

void TitleGameState::RestartEvents() {
    ticks_ = 0;
    fadeOutOpacity_ = 1.0f;
    fadingOut_ = false;
    if (background_) {
        background_->Reset();
    }
    if (uilmao_)
    {
        uilmao_->Reset();
    }
}

void TitleGameState::TransitionToGameplay() {
    auto gameplayState = std::make_unique<GameplayState>(context_);
    gameplayState->SetCharacterSelection(uilmao_->GetCharacterSelection());    
    isFinished_ = true;
} 