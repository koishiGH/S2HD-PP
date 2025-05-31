#pragma once
#include "../core/GameContext.hpp"
#include "../graphics/BitmapFont.hpp"
#include <memory>
#include <string>
#include <SDL2/SDL.h>

class GameplayState : public GameState {
public:
    GameplayState(GameContext* context);
    ~GameplayState() override;

    bool Initialize() override;
    void Update() override;
    void Render() override;
    void HandleEvent(const SDL_Event& event) override;
    bool IsFinished() const { return false; }

    void DrawCharacterIcon(SDL_Texture* texture, int x, int y);
    void SetCharacterSelection(int selection);

private:
    GameContext* context_;
    std::unique_ptr<BitmapFont> hudFont_;
    std::unique_ptr<BitmapFont> hudFontAlt_;
    SDL_Texture* checkeredTextureSonic_;
    SDL_Texture* checkeredTextureTails_;
    SDL_Texture* triangleTextureSonic_;
    SDL_Texture* triangleTextureTails_;
    SDL_Texture* triangleTextureKnuckles_;
    SDL_Texture* lifeTextureSonic_;
    SDL_Texture* lifeTextureTails_;
    
    int score_;
    int time_;
    int rings_;
    int lives_;
    bool showMilliseconds_;
    double redAnimation_;
    int characterSelection_;  // 0 = Sonic & Tails, 1 = Sonic, 2 = Tails

    void DrawHUD();
    void DrawScore();
    void DrawTime();
    void DrawRings();
    void DrawLives();
    void DrawTLInfo(const std::string& caption, const std::string& value, 
                    int x, int y, bool redAnimate, bool rightAligned,
                    SDL_Texture* triangleTexture, SDL_Texture* checkeredTexture);
    void GetCharacterTextures(SDL_Texture*& triangleTexture, SDL_Texture*& checkeredTexture, SDL_Texture*& lifeTexture);
}; 