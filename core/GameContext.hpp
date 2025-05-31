#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include "../resources/ResourceManager.hpp"
#include "../states/GameState.hpp"

class GameContext {
public:
    GameContext();
    ~GameContext();

    bool Initialize();
    void Run();
    void Shutdown();

    SDL_Window* GetWindow() const { return window_; }
    SDL_Renderer* GetRenderer() const { return renderer_; }
    int GetScreenWidth() const { return screenWidth_; }
    int GetScreenHeight() const { return screenHeight_; }
    ResourceManager& GetResourceManager() { return ResourceManager::GetInstance(); }

    void SetFullscreen(bool fullscreen);
    bool IsFullscreen() const { return isFullscreen_; }

private:
    bool InitializeSDL();
    bool CreateWindow();
    bool CreateRenderer();
    bool InitializeResources();
    void HandleEvents();
    void Update();
    void Render();

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool isRunning_;
    bool isFullscreen_;
    
    const int screenWidth_ = 1280;
    const int screenHeight_ = 720;
    const std::string windowTitle_ = "Sonic 2 RE:HD - A w.i.p. Sonic 2 HD C++ remake";
    const std::string dataPath_ = "data/SONICORCA";

    std::unique_ptr<GameState> currentState_;
}; 