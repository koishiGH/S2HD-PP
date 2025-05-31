#include "GameContext.hpp"
#include "../states/DisclaimerGameState.hpp"
#include "../states/LogosGameState.hpp"
#include "../states/TeamLogoGameState.hpp"
#include "../states/TitleGameState.hpp"
#include "../states/GameplayState.hpp"
#include "../input/InputManager.hpp"
#include <iostream>
#include <SDL2/SDL.h>

const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

GameContext::GameContext() 
    : window_(nullptr)
    , renderer_(nullptr)
    , isRunning_(false)
    , isFullscreen_(true)
    , currentState_(std::make_unique<DisclaimerGameState>(this))
{
}

GameContext::~GameContext() {
    Shutdown();
}

bool GameContext::Initialize() {
    if (!InitializeSDL()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return false;
    }

    if (!CreateWindow()) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }

    if (!CreateRenderer()) {
        std::cerr << "Failed to create renderer" << std::endl;
        return false;
    }

    GetResourceManager().SetRenderer(renderer_);

    if (!InitializeResources()) {
        std::cerr << "Failed to initialize resources" << std::endl;
        return false;
    }

    if (!currentState_->Initialize()) {
        std::cerr << "Failed to initialize game state" << std::endl;
        return false;
    }

    isRunning_ = true;
    return true;
}

bool GameContext::InitializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool GameContext::CreateWindow() {
    window_ = SDL_CreateWindow(
        windowTitle_.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        screenWidth_,
        screenHeight_,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (window_ == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
    isFullscreen_ = true;

    return true;
}

bool GameContext::CreateRenderer() {
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

bool GameContext::InitializeResources() {
    return GetResourceManager().Initialize(dataPath_);
}

void GameContext::SetFullscreen(bool fullscreen) {
    if (isFullscreen_ == fullscreen) return;

    if (fullscreen) {
        SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window_, 0);
    }

    isFullscreen_ = fullscreen;
}

void GameContext::Run() {
    Uint32 frameStart;
    int frameTime;
    while (isRunning_) {
        frameStart = SDL_GetTicks();

        HandleEvents();
        Update();
        Render();

        frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void GameContext::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                isRunning_ = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_F11) {
                    SetFullscreen(!isFullscreen_);
                }
                break;
        }
    }
}

void GameContext::Update() {
    InputManager::UpdateKeyStates();
    currentState_->Update();

    if (dynamic_cast<DisclaimerGameState*>(currentState_.get())) {
        auto* disclaimer = static_cast<DisclaimerGameState*>(currentState_.get());
        if (disclaimer->IsFinished()) {
            currentState_ = std::make_unique<LogosGameState>(this);
            currentState_->Initialize();
        }
    }
    else if (dynamic_cast<LogosGameState*>(currentState_.get())) {
        auto* logos = static_cast<LogosGameState*>(currentState_.get());
        if (logos->IsFinished()) {
            currentState_ = std::make_unique<TeamLogoGameState>(this);
            currentState_->Initialize();
        }
    }
    else if (dynamic_cast<TeamLogoGameState*>(currentState_.get())) {
        auto* logos = static_cast<TeamLogoGameState*>(currentState_.get());
        if (logos->IsFinished()) {
            currentState_ = std::make_unique<TitleGameState>(this);
            currentState_->Initialize();
        }
    }
    else if (dynamic_cast<TitleGameState*>(currentState_.get())) {
        auto* title = static_cast<TitleGameState*>(currentState_.get());
        if (title->IsFinished()) {
            currentState_ = std::make_unique<GameplayState>(this);
            currentState_->Initialize();
        }
    }
}

void GameContext::Render() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
    currentState_->Render();
    SDL_RenderPresent(renderer_);
}

void GameContext::Shutdown() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    SDL_Quit();
} 