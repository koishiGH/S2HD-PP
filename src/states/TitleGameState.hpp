#pragma once

#include "GameState.hpp"
#include "../core/GameContext.hpp"
#include "../resources/ResourceManager.hpp"
#include "../graphics/BitmapFont.hpp"
#include "title/Background.hpp"
#include "title/UserInterface.hpp"
#include "GameplayState.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <memory>

class TitleGameState : public GameState {
public:
    explicit TitleGameState(GameContext* context);
    ~TitleGameState() override;

    bool Initialize() override;
    void Update() override;
    void Render() override;
    bool IsFinished() const { return isFinished_; }
    void HandleEvent(const SDL_Event& event) override;
    void TransitionToGameplay();

private:
    void LoadResources();
    void DrawIntroText();
    void RestartEvents();

    GameContext* context_;
    std::unique_ptr<Background> background_;
    std::unique_ptr<UserInterface> uilmao_;
    std::unique_ptr<BitmapFont> font_;
    
    int ticks_ = 0;
    bool loaded_ = false;
    float fadeOutOpacity_ = 1.0f;
    bool fadingOut_ = false;
    
    static constexpr int INTRO_TEXT_START = 60;
    static constexpr int INTRO_TEXT_FULL = 120;
    static constexpr int INTRO_TEXT_HOLD = 180;
    static constexpr int INTRO_TEXT_END = 240;

    enum class TitlePhase {
        IntroText,
        FadeToBlack,
        WhiteFlash,
        MainTitle
    };
    TitlePhase phase_ = TitlePhase::IntroText;
    int whiteFlashTicks_ = 0;
    bool isFinished_ = false;
}; 