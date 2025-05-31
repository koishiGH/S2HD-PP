#pragma once

#include "GameState.hpp"
#include <SDL2/SDL.h>
#include <memory>

class GameContext;

class DisclaimerGameState : public GameState {
public:
    explicit DisclaimerGameState(GameContext* gameContext);
    ~DisclaimerGameState() override;

    bool Initialize() override;
    void Update() override;
    void Render() override;
    void HandleEvent(const SDL_Event& event) override;

    bool IsFinished() const { return finished_; }

private:
    static constexpr int FADE_TIME = 60;
    static constexpr int SHOW_TIME = 240;
    static constexpr int WAIT_TIME = 60;
    static constexpr float FADE_SPEED = 1.0f / FADE_TIME;

    GameContext* gameContext_;
    SDL_Texture* disclaimerTexture_;
    bool loaded_;
    float opacity_;
    int showTimer_;
    int waitTimer_;
    bool fadingIn_;
    bool fadingOut_;
    bool finished_;

    enum class Phase {
        Waiting,
        FadingIn,
        Showing,
        FadingOut,
        Done
    };
    Phase phase_;
};
