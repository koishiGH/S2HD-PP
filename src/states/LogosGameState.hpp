#pragma once

#include "GameState.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>
#include "../graphics/BitmapFont.hpp"

class GameContext;

class LogosGameState : public GameState {
public:
    explicit LogosGameState(GameContext* gameContext);
    ~LogosGameState() override;

    bool Initialize() override;
    void Update() override;
    void Render() override;
    void HandleEvent(const SDL_Event& event) override;

    bool IsFinished() const { return finished_; }

private:
    enum class Phase {
        Loading,
        SonicIn,
        SonicOut,
        SonicIn2,
        FadeOut,
        Done
    };

    GameContext* gameContext_;
    SDL_Texture* engineTexture_;
    SDL_Texture* enginePartialTexture_;
    SDL_Texture* engineSonicTexture_;
    bool loaded_;
    bool finished_;

    Phase phase_;
    int timer_;
    int sonicX_;
    int sonicVX_;
    int sonicFrame_;
    bool smallSonic_;
    float fadeOpacity_;

    void drawPoweredBy(SDL_Renderer* renderer, int winW, int winH);
    void drawEngineLogo(SDL_Renderer* renderer, int winW, int winH);
    void drawSmallSonic(SDL_Renderer* renderer, int winW, int winH);
    void drawSonic(SDL_Renderer* renderer, int winW, int winH);

    std::unique_ptr<BitmapFont> font_;
};
