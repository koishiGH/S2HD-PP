#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <memory>

struct FontChar {
    SDL_Rect rect;
    int width = 0;
    SDL_Point offset = {0, 0};
};

class BitmapFont {
public:
    BitmapFont();
    ~BitmapFont();

    bool Load(const std::string& xmlPath, SDL_Renderer* renderer, const std::string& imageDir);
    bool LoadOverlay(const std::string& overlayImagePath, SDL_Renderer* renderer);
    void RenderText(SDL_Renderer* renderer, const std::string& text, int x, int y, bool useOverlay = false);
    void SetColorMod(Uint8 r, Uint8 g, Uint8 b);
    void ResetColorMod();

    int GetHeight() const { return charHeight_; }
    int GetTracking() const { return tracking_; }
    SDL_Texture* GetTexture() const { return texture_; }

    int GetStringWidth(const std::string& text) const {
        int width = 0;
        for (char c : text) {
            if (c == ' ') {
                width += 16;
                continue;
            }
            auto it = chars_.find(c);
            if (it != chars_.end()) {
                width += it->second.width + tracking_;
            } else {
                width += tracking_;
            }
        }
        return width;
    }

    int GetTextWidth(const std::string& text) const;

private:
    std::unordered_map<char, FontChar> chars_;
    SDL_Texture* texture_;
    SDL_Texture* overlayTexture_ = nullptr;
    int charHeight_;
    int tracking_;
    Uint8 colorR_ = 255;
    Uint8 colorG_ = 255;
    Uint8 colorB_ = 255;
};
