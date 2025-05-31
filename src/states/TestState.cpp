#include "TestState.hpp"
#include <iostream>

TestState::TestState()
    : logoTexture_(nullptr)
{
    logoRect_.x = 0;
    logoRect_.y = 0;
    logoRect_.w = 0;
    logoRect_.h = 0;
}

TestState::~TestState() {
    if (logoTexture_) {
        SDL_DestroyTexture(logoTexture_);
        logoTexture_ = nullptr;
    }
}

bool TestState::Initialize(SDL_Renderer* renderer) {
    logoTexture_ = ResourceManager::GetInstance().LoadTexture("TEAMLOGO.png");
    if (!logoTexture_) {
        std::cerr << "Failed to load TEAMLOGO.png" << std::endl;
        return false;
    }

    int width, height;
    SDL_QueryTexture(logoTexture_, nullptr, nullptr, &width, &height);

    logoRect_.w = width;
    logoRect_.h = height;
    logoRect_.x = (1280 - width) / 2;
    logoRect_.y = (720 - height) / 2;

    return true;
}

void TestState::Update() {}

void TestState::Render(SDL_Renderer* renderer) {
    if (logoTexture_) {
        SDL_RenderCopy(renderer, logoTexture_, nullptr, &logoRect_);
    }
} 