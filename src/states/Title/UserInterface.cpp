#include "UserInterface.hpp"
#include "../TitleGameState.hpp"
#include "../../resources/ResourceManager.hpp"
#include "../../input/InputManager.hpp"
#include <iostream>
#include <cmath>

UserInterface::UserInterface(GameContext* gameContext, TitleGameState* titleGameState)
    : gameContext_(gameContext)
    , titleGameState_(titleGameState)
    , visible_(true)
    , loaded_(false)
    , ticks_(0)
    , pressStartActive_(true)
    , selectionIndex_(0)
    , busy_(false)
    , pressStartScale_(1.0f)
    , pressStartOpacity_(1.0f)
    , pressStartWhiteAdditive_(0.0f)
    , textOpacity_(1.0f)
    , textWhiteAdditive_(0.0f)
    , levelSelectInputState_(0)
    , levelSelectEnabled_(false)
    , levelSelectSelectionIndex_(0)
    , characterSelectionIndex_(0)
    , characterSelectActive_(false)
    , characterSelectOpacity_(0.0f)
    , characterSelected_(false)
    , demoTimeout_(720)
    , characterSelectTimer_(60)
{
    textureSelectionMarker_ = ResourceManager::GetInstance().LoadTexture("TITLE/SELECTIONMARKER.png");
    textureZigZag_ = ResourceManager::GetInstance().LoadTexture("TITLE/ZIGZAG.png");
    textureLeftArrow_ = ResourceManager::GetInstance().LoadTexture("MENU/LEFT.png");
    textureRightArrow_ = ResourceManager::GetInstance().LoadTexture("MENU/RIGHT.png");

    fontImpactRegular_ = std::make_unique<BitmapFont>();
    fontImpactItalic_ = std::make_unique<BitmapFont>();
    if (!fontImpactRegular_->Load("data/SONICORCA/FONTS/IMPACT/REGULAR.font", gameContext_->GetRenderer(), "data/SONICORCA/FONTS/IMPACT/REGULAR") ||
        !fontImpactItalic_->Load("data/SONICORCA/FONTS/IMPACT/ITALIC.font", gameContext_->GetRenderer(), "data/SONICORCA/FONTS/IMPACT/ITALIC")) {
        std::cerr << "Failed to load fonts!" << std::endl;
    }


    InitialiseMenuItemWidgets();
    InitialiseLevelSelect();
}

UserInterface::~UserInterface() {}

void UserInterface::Reset() {
    ticks_ = 0;
    pressStartActive_ = true;
    pressStartScale_ = 1.0f;
    pressStartOpacity_ = 1.0f;
    pressStartWhiteAdditive_ = 0.0f;
    textOpacity_ = 1.0f;
    textWhiteAdditive_ = 0.0f;
    selectionIndex_ = 0;
    levelSelectInputState_ = 0;
    levelSelectEnabled_ = false;
    levelSelectSelectionIndex_ = 0;
    characterSelectionIndex_ = 0;
    characterSelectActive_ = false;
    characterSelectOpacity_ = 0.0f;
    characterSelected_ = false;
    demoTimeout_ = 720;
    characterSelectTimer_ = 60;
    InitialiseMenuItemWidgets();
}

void UserInterface::Update() {
    if (!visible_) return;

    ticks_++;

    if (!pressStartActive_ && busy_) {
        if (pressStartScale_ > 1.0f) {
            pressStartScale_ = std::max(1.0f, pressStartScale_ - 0.04f);
        }
        
        if (pressStartWhiteAdditive_ > 0.0f) {
            pressStartWhiteAdditive_ = std::max(0.0f, pressStartWhiteAdditive_ - 0.1f);
        }
        
        if (textOpacity_ < 1.0f) {
            textOpacity_ = std::min(1.0f, textOpacity_ + 0.1f);
        }
        if (textWhiteAdditive_ > 0.0f) {
            textWhiteAdditive_ = std::max(0.0f, textWhiteAdditive_ - 0.1f);
        }
        
        if (pressStartOpacity_ > 0.0f) {
            pressStartOpacity_ = std::max(0.0f, pressStartOpacity_ - 0.1f);
        }

        if (pressStartOpacity_ == 0.0f && textOpacity_ == 1.0f) {
            busy_ = false;
        }
    }

    if (characterSelectActive_) {
        characterSelectOpacity_ = std::min(1.0f, characterSelectOpacity_ + 0.06666667f);
        textOpacity_ = 1.0f - characterSelectOpacity_;
    } else {
        characterSelectOpacity_ = std::max(0.0f, characterSelectOpacity_ - 0.06666667f);
        textOpacity_ = 1.0f - characterSelectOpacity_;
    }

    HandleInput();

    if (demoTimeout_ > 0) {
        demoTimeout_--;
        if (demoTimeout_ == 0) {
            StartDemo();
        }
    }

    if (characterSelected_) {
        characterSelectTimer_--;
        if (characterSelectTimer_ == 0) {
            OnSelectCharacter();
        }
    }

    if (markerAnimating_) {
        float t = static_cast<float>(markerAnimFrame_) / markerAnimLength_;
        for (int i = 0; i < 2; ++i) {
            markerPositions_[i].x = markerStart_[i].x + (markerTarget_[i].x - markerStart_[i].x) * t;
            float hop = -32.0f * std::sin(3.14159f * t);
            markerPositions_[i].y = markerStart_[i].y + (markerTarget_[i].y - markerStart_[i].y) * t + hop;
        }
        markerAnimFrame_++;
        if (markerAnimFrame_ >= markerAnimLength_) {
            markerAnimating_ = false;
            markerPositions_[0] = markerTarget_[0];
            markerPositions_[1] = markerTarget_[1];
        }
    }
}

void UserInterface::Draw() {
    if (!visible_) return;

    DrawPressStart();
    if (!pressStartActive_) {
        if (characterSelectOpacity_ != 0.0f) {
            DrawCharacterSelect();
        }
        if (characterSelectOpacity_ != 1.0f) {
            DrawMenuItems();
        }
    }
    if (levelSelectEnabled_) {
        DrawLevelSelect();
    }
}

void UserInterface::HandleInput() {
    if (busy_) return;

    using IM = InputManager;

    if (levelSelectEnabled_) {
        if (IM::justPressed(IM::GetScancode(IM::KEY_UP))) {
            levelSelectSelectionIndex_ = NegMod(levelSelectSelectionIndex_ - 1, static_cast<int>(levelSelectItems_.size()));
        } else if (IM::justPressed(IM::GetScancode(IM::KEY_DOWN))) {
            levelSelectSelectionIndex_ = NegMod(levelSelectSelectionIndex_ + 1, static_cast<int>(levelSelectItems_.size()));
        }
        if (IM::justPressed(IM::GetScancode(IM::KEY_ENTER))) {
            OnLevelSelectStart();
        }
        if (IM::justPressed(IM::GetScancode(IM::KEY_Z))) {
            levelSelectEnabled_ = false;
        }
        return;
    }

    if (pressStartActive_) {
        if (IM::justPressed(IM::GetScancode(IM::KEY_Z))) {
            busy_ = true;
            pressStartScale_ = 1.2f;  
            pressStartWhiteAdditive_ = 1.0f;  
            pressStartOpacity_ = 1.0f;
            textOpacity_ = 0.0f;  
            textWhiteAdditive_ = 1.0f;
            pressStartActive_ = false;
            demoTimeout_ = 0;
        }
        return;
    }

    
    if (characterSelectActive_) {
        
        if (IM::justPressed(IM::GetScancode(IM::KEY_LEFT))) {
            characterSelectionIndex_ = (characterSelectionIndex_ + 2) % 3;
            
        } else if (IM::justPressed(IM::GetScancode(IM::KEY_RIGHT))) {
            characterSelectionIndex_ = (characterSelectionIndex_ + 1) % 3;
            
        }
        
        if (IM::justPressed(IM::GetScancode(IM::KEY_X))) {
            characterSelectActive_ = false;
            
            return;
        }
        
        if (IM::justPressed(IM::GetScancode(IM::KEY_Z))) {
            
            characterSelected_ = true;
            characterSelectTimer_ = 60;
            return;
        }
        return;
    }

    
    
    if (IM::justPressed(IM::GetScancode(IM::KEY_X))) {
        demoTimeout_ = 720;
        pressStartActive_ = true;
        pressStartOpacity_ = 1.0f;
        pressStartScale_ = 1.0f;
        pressStartWhiteAdditive_ = 0.0f;
        selectionIndex_ = 0;
        InitialiseMenuItemWidgets();
        
        return;
    }
    
    if (IM::justPressed(IM::GetScancode(IM::KEY_LEFT))) {
        int newSelectionIndex = NegMod(selectionIndex_ - 1, static_cast<int>(menuItems_.size()));
        StartMarkerTween(newSelectionIndex);
        selectionIndex_ = newSelectionIndex;
        
    } else if (IM::justPressed(IM::GetScancode(IM::KEY_RIGHT))) {
        int newSelectionIndex = NegMod(selectionIndex_ + 1, static_cast<int>(menuItems_.size()));
        StartMarkerTween(newSelectionIndex);
        selectionIndex_ = newSelectionIndex;
        
    }
    
    if (IM::justPressed(IM::GetScancode(IM::KEY_ENTER))) {
        if (selectionIndex_ >= 0 && selectionIndex_ < static_cast<int>(menuItems_.size())) {
            if (menuItems_[selectionIndex_].action) {
                menuItems_[selectionIndex_].action();
            }
            
        }
    }
}

void UserInterface::InitialiseMenuItemWidgets() {
    menuItems_.clear();
    menuItems_.push_back(MenuItem("QUIT", [this]() { OnSelectQuit(); }));
    menuItems_.push_back(MenuItem("NEW GAME", [this]() { OnSelectNewGame(); }));
    menuItems_.push_back(MenuItem("OPTIONS", [this]() { OnSelectOptions(); }));

    menuItemWidgets_.resize(menuItems_.size());
    
    for (size_t i = 0; i < menuItems_.size(); i++) {
        menuItemWidgets_[i] = {
            static_cast<int>(i),
            static_cast<int>(i),
            1.0f,
            static_cast<float>(960 + 400 * (i - menuItems_.size()/2)),
            1.0f
        };
    }
    SetSelectionMarkerPositions();
}

void UserInterface::SetSelectionMarkerPositions() {
    if (!textureSelectionMarker_) return;
    
    int markerWidth, markerHeight;
    SDL_QueryTexture(textureSelectionMarker_, nullptr, nullptr, &markerWidth, &markerHeight);
    
    for (auto& widget : menuItemWidgets_) {
        const auto& menuItem = menuItems_[widget.menuItemIndex];
        int textWidth = fontImpactRegular_->GetTextWidth(menuItem.text);
        widget.markerX = static_cast<int>(widget.x - textWidth/2 - markerWidth - 20); 
        widget.markerY = static_cast<int>(900 - markerHeight/2);
    }
}

int UserInterface::GetMenuItemWidth(int index) {
    
    return 0;
}

int UserInterface::GetMarkerOffset(int index) {
    return GetMenuItemWidth(index) / 2 + 48;
}

void UserInterface::InitialiseLevelSelect() {
    
}

void UserInterface::OnSelectNewGame() {
    characterSelectActive_ = true;
}

void UserInterface::OnSelectCharacter() {
    busy_ = true;
    
    ApplyCharacterSelection();
    
    titleGameState_->TransitionToGameplay();
}

void UserInterface::OnSelectOptions() {
    busy_ = true;
    
}

void UserInterface::OnSelectQuit() {
    busy_ = true;
    
}

void UserInterface::StartDemo() {
    busy_ = true;
    
}

void UserInterface::OnLevelSelectStart() {
    busy_ = true;
    
}

void UserInterface::ApplyCharacterSelection() {
    
}

void UserInterface::DrawPressStart() {
    if (!fontImpactItalic_ || !pressStartActive_) return;
    SDL_Renderer* renderer = gameContext_->GetRenderer();

    float fade = (sinf(ticks_ * 0.02f) + 1.0f) * 0.5f;
    Uint8 alpha = static_cast<Uint8>(fade * pressStartOpacity_ * 255);

    std::string text = "PRESS START";
    int textWidth = fontImpactItalic_->GetTextWidth(text);
    int x = 960 - textWidth / 2;
    int y = 900 - fontImpactItalic_->GetHeight() / 2;

    int zigzagWidth = 80;
    int zigzagOffset = (ticks_ * 4) % 160;

    DrawZigZag(x - zigzagWidth - 16, y + fontImpactItalic_->GetHeight() / 2, zigzagWidth, zigzagOffset);
    DrawZigZag(x + textWidth + 16, y + fontImpactItalic_->GetHeight() / 2, zigzagWidth, zigzagOffset);

    SDL_Rect destRect = {
        static_cast<int>(x - (textWidth * (pressStartScale_ - 1.0f) / 2)),
        static_cast<int>(y - (fontImpactItalic_->GetHeight() * (pressStartScale_ - 1.0f) / 2)),
        static_cast<int>(textWidth * pressStartScale_),
        static_cast<int>(fontImpactItalic_->GetHeight() * pressStartScale_)
    };

    fontImpactItalic_->RenderText(renderer, text, x + 2, y + 2, false);
    
    SDL_SetTextureColorMod(fontImpactItalic_->GetTexture(),
        255,
        static_cast<Uint8>(255 * (1.0f - pressStartWhiteAdditive_)),
        static_cast<Uint8>(255 * (1.0f - pressStartWhiteAdditive_)));
    fontImpactItalic_->RenderText(renderer, text, x, y, true);
    SDL_SetTextureColorMod(fontImpactItalic_->GetTexture(), 255, 255, 255);
}

void UserInterface::DrawZigZag(int x, int y, int width, int animOffset) {
    if (!textureZigZag_) return;
    SDL_Renderer* renderer = gameContext_->GetRenderer();

    int texW = 0, texH = 0;
    SDL_QueryTexture(textureZigZag_, nullptr, nullptr, &texW, &texH);

    int drawX = x - animOffset;
    while (drawX < x + width) {
        SDL_Rect dst = { drawX, y - texH / 2, texW, texH };
        SDL_RenderCopy(renderer, textureZigZag_, nullptr, &dst);
        drawX += texW;
    }
}

void UserInterface::DrawMenuItems() {
    SDL_Renderer* renderer = gameContext_->GetRenderer();
    int y = 900; 
    
    
    int markerWidth = 0, markerHeight = 0;
    if (textureSelectionMarker_)
        SDL_QueryTexture(textureSelectionMarker_, nullptr, nullptr, &markerWidth, &markerHeight);
    
    
    if (textureSelectionMarker_ && selectionIndex_ >= 0 && selectionIndex_ < static_cast<int>(menuItemWidgets_.size())) {
        const auto& widget = menuItemWidgets_[selectionIndex_];
        const auto& menuItem = menuItems_[widget.menuItemIndex];
        int textWidth = fontImpactRegular_->GetTextWidth(menuItem.text);
        int markerOffset = textWidth / 2 + 48; 

        
        SDL_Rect leftDst = { static_cast<int>(markerPositions_[0].x), static_cast<int>(markerPositions_[0].y), markerWidth, markerHeight };
        SDL_RenderCopy(renderer, textureSelectionMarker_, nullptr, &leftDst);

        
        SDL_Rect rightDst = { static_cast<int>(markerPositions_[1].x), static_cast<int>(markerPositions_[1].y), markerWidth, markerHeight };
        SDL_RenderCopyEx(renderer, textureSelectionMarker_, nullptr, &rightDst, 0, nullptr, SDL_FLIP_HORIZONTAL);
    }
    
    
    for (const auto& widget : menuItemWidgets_) {
        const auto& menuItem = menuItems_[widget.menuItemIndex];
        bool selected = (widget.menuItemIndex == selectionIndex_);
        DrawMenuItem(menuItem.text, static_cast<int>(widget.x), y, widget.opacity, widget.scale, selected);
    }
}

void UserInterface::DrawMenuItem(const std::string& text, int x, int y, float opacity, float scale, bool selected) {
    if (!fontImpactRegular_) return;
    SDL_Renderer* renderer = gameContext_->GetRenderer();
    int textWidth = fontImpactRegular_->GetTextWidth(text);
    int drawX = x - static_cast<int>(textWidth * scale / 2);
    int drawY = static_cast<int>(y - fontImpactRegular_->GetHeight() * scale / 2);
    Uint8 alpha = static_cast<Uint8>(opacity * 255);
    
    fontImpactRegular_->RenderText(renderer, text, drawX + 2, drawY + 2, false);
    
    fontImpactRegular_->RenderText(renderer, text, drawX, drawY, true);
}

void UserInterface::DrawCharacterSelect() {
    SDL_Renderer* renderer = gameContext_->GetRenderer();

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(192 * characterSelectOpacity_));
    SDL_Rect overlayRect = { 0, 800, 1920, 200 };
    SDL_RenderFillRect(renderer, &overlayRect);

    std::string title = "SELECT CHARACTER";
    int titleWidth = fontImpactRegular_->GetTextWidth(title);
    int titleX = 960 - titleWidth / 2;
    int titleY = 820;
    fontImpactRegular_->RenderText(renderer, title, titleX, titleY, true);

    const char* options[3] = { "SONIC & TAILS", "SONIC", "TAILS" };
    int selected = characterSelectionIndex_;

    for (int i = 0; i < 3; ++i) {
        std::string text = options[i];
        int textWidth = fontImpactRegular_->GetTextWidth(text);
        int x = 640 + i * 320 - textWidth / 2;
        int y = 900;

        if (i == selected) {
            if (textureLeftArrow_) {
                SDL_Rect leftArrow = { x - 48, y, 32, 32 };
                SDL_RenderCopy(renderer, textureLeftArrow_, nullptr, &leftArrow);
            }
            if (textureRightArrow_) {
                SDL_Rect rightArrow = { x + textWidth + 16, y, 32, 32 };
                SDL_RenderCopy(renderer, textureRightArrow_, nullptr, &rightArrow);
            }

            SDL_SetRenderDrawColor(renderer, 255, 255, 0, static_cast<Uint8>(255 * characterSelectOpacity_));
            SDL_Rect highlight = { x - 16, y - 8, textWidth + 32, 64 };
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_SetTextureAlphaMod(fontImpactRegular_->GetTexture(), static_cast<Uint8>(255 * characterSelectOpacity_));
        fontImpactRegular_->RenderText(renderer, text, x, y, true);
        SDL_SetTextureAlphaMod(fontImpactRegular_->GetTexture(), 255);
    }
}

void UserInterface::DrawLevelSelect() {
    
}

int UserInterface::NegMod(int x, int divisor) {
    while (x < 0) {
        x += divisor;
    }
    return x % divisor;
}

void UserInterface::StartMarkerTween(int newSelection) {
    int markerWidth = 0, markerHeight = 0;
    SDL_QueryTexture(textureSelectionMarker_, nullptr, nullptr, &markerWidth, &markerHeight);

    
    const auto& widget = menuItemWidgets_[selectionIndex_];
    const auto& menuItem = menuItems_[widget.menuItemIndex];
    int textWidth = fontImpactRegular_->GetTextWidth(menuItem.text);
    int markerOffset = textWidth / 2 + 48;
    int y = widget.markerY;

    
    markerStart_[0] = markerPositions_[0];
    markerStart_[1] = markerPositions_[1];

    
    const auto& newWidget = menuItemWidgets_[newSelection];
    const auto& newMenuItem = menuItems_[newWidget.menuItemIndex];
    int newTextWidth = fontImpactRegular_->GetTextWidth(newMenuItem.text);
    int newMarkerOffset = newTextWidth / 2 + 48;
    int newY = newWidget.markerY;

    markerTarget_[0] = { static_cast<float>(newWidget.x - newMarkerOffset), static_cast<float>(newY) };
    markerTarget_[1] = { static_cast<float>(newWidget.x + newMarkerOffset - markerWidth), static_cast<float>(newY) };

    markerAnimFrame_ = 0;
    markerAnimating_ = true;
} 
