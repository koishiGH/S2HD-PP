#include "GameplayState.hpp"
#include <core/GameContext.hpp>
#include <SDL2/SDL_image.h>
#include <sstream>
#include <iomanip>
#include <cmath>

GameplayState::GameplayState(GameContext* context) 
    : context_(context)
    , checkeredTextureSonic_(nullptr)
    , checkeredTextureTails_(nullptr)
    , triangleTextureSonic_(nullptr)
    , triangleTextureTails_(nullptr)
    , triangleTextureKnuckles_(nullptr)
    , lifeTextureSonic_(nullptr)
    , lifeTextureTails_(nullptr)
    , score_(0)
    , time_(0)
    , rings_(0)
    , lives_(3)
    , showMilliseconds_(true)
    , redAnimation_(0.0)
    , characterSelection_(0)
{
}

GameplayState::~GameplayState() {
    if (checkeredTextureSonic_) SDL_DestroyTexture(checkeredTextureSonic_);
    if (checkeredTextureTails_) SDL_DestroyTexture(checkeredTextureTails_);
    if (triangleTextureSonic_) SDL_DestroyTexture(triangleTextureSonic_);
    if (triangleTextureTails_) SDL_DestroyTexture(triangleTextureTails_);
    if (triangleTextureKnuckles_) SDL_DestroyTexture(triangleTextureKnuckles_);
    if (lifeTextureSonic_) SDL_DestroyTexture(lifeTextureSonic_);
    if (lifeTextureTails_) SDL_DestroyTexture(lifeTextureTails_);
}

bool GameplayState::Initialize() {
    hudFont_ = std::make_unique<BitmapFont>();
    if (!hudFont_->Load("data/SONICORCA/FONTS/HUD.font", context_->GetRenderer(), "data/SONICORCA/FONTS/HUD")) {
        return false;
    }
    if (!hudFont_->LoadOverlay("data/SONICORCA/FONTS/HUD/OVERLAYSILVER.png", context_->GetRenderer())) {
        return false;
    }

    hudFontAlt_ = std::make_unique<BitmapFont>();
    if (!hudFontAlt_->Load("data/SONICORCA/FONTS/HUD.font", context_->GetRenderer(), "data/SONICORCA/FONTS/HUD")) {
        return false;
    }
    if (!hudFontAlt_->LoadOverlay("data/SONICORCA/FONTS/HUD/OVERLAYGOLD.png", context_->GetRenderer())) {
        return false;
    }

    auto loadTexture = [this](const std::string& path) -> SDL_Texture* {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) return nullptr;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(context_->GetRenderer(), surface);
        SDL_FreeSurface(surface);
        return texture;
    };

    checkeredTextureSonic_ = loadTexture("data/SONICORCA/HUD/CHECKERED.png");
    checkeredTextureTails_ = loadTexture("data/SONICORCA/HUD/CHECKERED/TAILS.png");
    triangleTextureSonic_ = loadTexture("data/SONICORCA/HUD/TRIANGLE.png");
    triangleTextureTails_ = loadTexture("data/SONICORCA/HUD/TRIANGLE/TAILS.png");
    triangleTextureKnuckles_ = loadTexture("data/SONICORCA/HUD/TRIANGLE/KNUCKLES.png");
    lifeTextureSonic_ = loadTexture("data/SONICORCA/HUD/LIFE/SONIC.png");
    lifeTextureTails_ = loadTexture("data/SONICORCA/HUD/LIFE/TAILS.png");

    return true;
}

void GameplayState::Update() {
    redAnimation_ = fmod(redAnimation_ + 0.05, 2.0);
    
    time_++;
}

void GameplayState::Render() {
    DrawHUD();
}

void GameplayState::DrawHUD() {
    DrawScore();
    DrawTime();
    DrawRings();
    DrawLives();
}

void GameplayState::GetCharacterTextures(SDL_Texture*& triangleTexture, SDL_Texture*& checkeredTexture, SDL_Texture*& lifeTexture) {
    triangleTexture = triangleTextureSonic_;
    checkeredTexture = checkeredTextureSonic_;
    lifeTexture = lifeTextureSonic_;

    if (characterSelection_ == 2) {
        triangleTexture = triangleTextureTails_;
        checkeredTexture = checkeredTextureTails_;
        lifeTexture = lifeTextureTails_;
    }
}

void GameplayState::DrawScore() {
    SDL_Texture* triangleTexture, *checkeredTexture, *lifeTexture;
    GetCharacterTextures(triangleTexture, checkeredTexture, lifeTexture);
    DrawTLInfo("SCORE", std::to_string(score_), 226, 98, false, true, triangleTexture, checkeredTexture);
}

void GameplayState::DrawTime() {
    int minutes = time_ / 3600;
    int seconds = (time_ / 60) % 60;
    int milliseconds = (time_ % 60) * 100 / 60;

    std::stringstream ss;
    if (showMilliseconds_) {
        ss << minutes << "'" << std::setw(2) << std::setfill('0') << seconds 
           << "\"" << std::setw(2) << std::setfill('0') << milliseconds;
    } else {
        ss << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
    }

    SDL_Texture* triangleTexture, *checkeredTexture, *lifeTexture;
    GetCharacterTextures(triangleTexture, checkeredTexture, lifeTexture);
    DrawTLInfo("TIME", ss.str(), 226, 162, minutes >= 9, true, triangleTexture, checkeredTexture);
}

void GameplayState::DrawRings() {
    SDL_Texture* triangleTexture, *checkeredTexture, *lifeTexture;
    GetCharacterTextures(triangleTexture, checkeredTexture, lifeTexture);
    DrawTLInfo("RINGS", std::to_string(rings_), 226, 226, rings_ == 0, true, triangleTexture, checkeredTexture);
}

void GameplayState::DrawLives() {
    if (lives_ < 0) return;

    SDL_Texture* triangleTexture, *checkeredTexture, *lifeTexture;
    GetCharacterTextures(triangleTexture, checkeredTexture, lifeTexture);
    DrawCharacterIcon(lifeTexture, 264, 958);

    std::string livesText = "×" + std::to_string(lives_);
    hudFont_->RenderText(context_->GetRenderer(), livesText, 300, 934);
}

void GameplayState::DrawTLInfo(const std::string& caption, const std::string& value,
                              int x, int y, bool redAnimate, bool rightAligned,
                              SDL_Texture* triangleTexture, SDL_Texture* checkeredTexture) {
    this->DrawCharacterIcon(checkeredTexture, x - 8, y + 8);

    if (caption == "SCORE") {
        this->DrawCharacterIcon(triangleTexture, x + 125, y + 15);
    }
    else if (caption == "TIME") {
        this->DrawCharacterIcon(triangleTexture, x + 75, y + 20);
    }
    else if (caption == "RINGS") {
        this->DrawCharacterIcon(triangleTexture, x + 105, y + 18);
    }

    int valueX = x + 200;
    hudFont_->RenderText(context_->GetRenderer(), value, valueX, y, true);

    if (caption == "RINGS" && this->rings_ == 0) {
        double animValue = this->redAnimation_ > 1.0 ? 2.0 - this->redAnimation_ : this->redAnimation_;
        Uint8 red = 255;
        Uint8 green = static_cast<Uint8>(255 * (1.0 - animValue));
        Uint8 blue = static_cast<Uint8>(255 * (1.0 - animValue));
        hudFontAlt_->SetColorMod(red, green, blue);
    } else {
        hudFontAlt_->ResetColorMod();
    }

    hudFontAlt_->RenderText(context_->GetRenderer(), caption, x, y, true);
    hudFontAlt_->ResetColorMod();
}

void GameplayState::DrawCharacterIcon(SDL_Texture* texture, int x, int y) {
    if (!texture) return;
    
    if (characterSelection_ == 0 || characterSelection_ == 1) {
        if (texture == triangleTextureTails_ || texture == checkeredTextureTails_ || texture == lifeTextureTails_) {
            return;
        }
    } else if (characterSelection_ == 2) {
        if (texture == triangleTextureSonic_ || texture == checkeredTextureSonic_ || texture == lifeTextureSonic_) {
            return;
        }
    }
    
    SDL_Rect dst = {x, y, 0, 0};
    SDL_QueryTexture(texture, nullptr, nullptr, &dst.w, &dst.h);
    SDL_RenderCopy(context_->GetRenderer(), texture, nullptr, &dst);
}

void GameplayState::HandleEvent(const SDL_Event& event) {}

void GameplayState::SetCharacterSelection(int selection) {
    characterSelection_ = selection;
}