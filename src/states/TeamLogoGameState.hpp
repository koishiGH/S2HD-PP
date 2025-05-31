#pragma once

#include "GameState.hpp"
#include <SDL2/SDL.h>
#include <memory>

class GameContext;

class TeamLogoGameState : public GameState {
public:
    explicit TeamLogoGameState(GameContext* gameContext);
    ~TeamLogoGameState() override;

    bool Initialize() override;
    void Update() override;
    void Render() override;
    void HandleEvent(const SDL_Event& event) override;
    bool IsFinished() const;

private:
    enum class Phase {
        FadingIn,
        Showing,
        FadingOut,
        Done
    };

    Phase phase_;
    static constexpr int FADE_TIME = 60;
    static constexpr int SHOW_TIME = 240;
    static constexpr float FADE_SPEED = 1.0f / FADE_TIME;

    GameContext* gameContext_;
    SDL_Texture* logoTexture_;
    bool loaded_;
    float opacity_;
    int showTimer_;
    bool finished_;
}; 