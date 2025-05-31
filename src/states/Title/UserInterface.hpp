#pragma once

#include "../../core/GameContext.hpp"
#include "../../graphics/BitmapFont.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>

class TitleGameState;

class UserInterface {
public:
    explicit UserInterface(GameContext* gameContext, TitleGameState* titleGameState);
    ~UserInterface();

    void Reset();
    void Update();
    void Draw();

    bool IsVisible() const { return visible_; }
    void SetVisible(bool visible) { visible_ = visible; }

    void StartMarkerTween(int newSelection);
    int GetCharacterSelection() const { return characterSelectionIndex_; }

    void DrawZigZag(int x, int y, int width, int animOffset);

private:
    struct MenuItem {
        std::string text;
        std::function<void()> action;

        MenuItem(const std::string& t, std::function<void()> a) : text(t), action(a) {}
    };

    struct MenuItemWidget {
        int relativeIndex;
        int menuItemIndex;
        float opacity;
        float x;
        float scale;
        int markerX;
        int markerY;
    };

    struct LevelSelectItem {
        std::string text;
        std::string mnemonic;
        int act;
        int number;
    };

    struct MarkerPos {
        float x, y;
    };

    void HandleInput();
    void InitialiseMenuItemWidgets();
    void SetSelectionMarkerPositions();
    int GetMenuItemWidth(int index);
    int GetMarkerOffset(int index);
    void InitialiseLevelSelect();
    void OnSelectNewGame();
    void OnSelectCharacter();
    void OnSelectOptions();
    void OnSelectQuit();
    void StartDemo();
    void OnLevelSelectStart();
    void ApplyCharacterSelection();
    void DrawPressStart();
    void DrawZigZag();
    void DrawMenuItems();
    void DrawMenuItem(const std::string& text, int x, int y, float opacity, float scale, bool selected = false);
    void DrawCharacterSelect();
    void DrawLevelSelect();

    static int NegMod(int x, int divisor);

    GameContext* gameContext_;
    TitleGameState* titleGameState_;
    bool visible_ = true;
    bool loaded_ = false;
    int ticks_ = 0;
    bool pressStartActive_ = true;
    int selectionIndex_ = 0;
    bool busy_ = false;
    float pressStartScale_ = 1.0f;
    float pressStartOpacity_ = 1.0f;
    float pressStartWhiteAdditive_ = 0.0f;
    float textOpacity_ = 1.0f;
    float textWhiteAdditive_ = 0.0f;
    int levelSelectInputState_ = 0;
    bool levelSelectEnabled_ = false;
    int levelSelectSelectionIndex_ = 0;
    int characterSelectionIndex_ = 0;
    bool characterSelectActive_ = false;
    float characterSelectOpacity_ = 0.0f;
    bool characterSelected_ = false;
    int demoTimeout_ = 720;
    int characterSelectTimer_ = 60;

    std::vector<MenuItem> menuItems_;
    std::vector<MenuItemWidget> menuItemWidgets_;
    std::vector<LevelSelectItem> levelSelectItems_;

    SDL_Texture* textureSelectionMarker_ = nullptr;
    SDL_Texture* textureZigZag_ = nullptr;
    SDL_Texture* textureLeftArrow_ = nullptr;
    SDL_Texture* textureRightArrow_ = nullptr;
    std::unique_ptr<BitmapFont> fontImpactRegular_;
    std::unique_ptr<BitmapFont> fontImpactItalic_;

    MarkerPos markerPositions_[2];
    MarkerPos markerStart_[2];
    MarkerPos markerTarget_[2];
    int markerAnimFrame_ = 0;
    int markerAnimLength_ = 10;
    bool markerAnimating_ = false;

    float GetPressStartScaleX(int t) {
        if (t <= 5) return 1.0f;
        if (t <= 10) return 1.2f;
        if (t <= 20) return 0.0f;
        return 0.0f;
    }

    float GetPressStartScaleY(int t) {
        if (t <= 5) return 1.45f;
        if (t <= 10) return 1.0f;
        if (t <= 20) return 0.0f;
        return 0.0f;
    }

    float GetPressStartWhiteAdditive(int t) {
        if (t <= 10) return t / 10.0f;
        return 1.0f;
    }

    float GetPressStartOpacity(int t) {
        if (t <= 10) return 1.0f;
        if (t <= 20) return 1.0f - (t - 10) / 10.0f;
        return 0.0f;
    }

    float GetTextOpacity(int t) {
        if (t <= 20) return 0.0f;
        if (t <= 30) return (t - 20) / 10.0f;
        return 1.0f;
    }

    float GetTextWhiteAdditive(int t) {
        if (t <= 20) return 1.0f;
        if (t <= 30) return 1.0f - (t - 20) / 10.0f;
        return 0.0f;
    }
}; 