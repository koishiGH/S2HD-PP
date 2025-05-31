#include "BitmapFont.hpp"
#include <SDL2/SDL_image.h>
#include <tinyxml2.h>
#include <iostream>

BitmapFont::BitmapFont() : texture_(nullptr), charHeight_(0), tracking_(0) {}
BitmapFont::~BitmapFont() { if (texture_) SDL_DestroyTexture(texture_); }

void BitmapFont::SetColorMod(Uint8 r, Uint8 g, Uint8 b) {
    colorR_ = r;
    colorG_ = g;
    colorB_ = b;
    if (texture_) SDL_SetTextureColorMod(texture_, r, g, b);
    if (overlayTexture_) SDL_SetTextureColorMod(overlayTexture_, r, g, b);
}

void BitmapFont::ResetColorMod() {
    colorR_ = 255;
    colorG_ = 255;
    colorB_ = 255;
    if (texture_) SDL_SetTextureColorMod(texture_, 255, 255, 255);
    if (overlayTexture_) SDL_SetTextureColorMod(overlayTexture_, 255, 255, 255);
}

bool BitmapFont::Load(const std::string& xmlPath, SDL_Renderer* renderer, const std::string& imageDir) {
    using namespace tinyxml2;
    XMLDocument doc;
    if (doc.LoadFile(xmlPath.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load font XML: " << xmlPath << std::endl;
        return false;
    }
    auto fontElem = doc.FirstChildElement("font");
    if (!fontElem) return false;

    fontElem->FirstChildElement("height")->QueryIntText(&charHeight_);
    fontElem->FirstChildElement("tracking")->QueryIntText(&tracking_);

    std::string shapeFile = fontElem->FirstChildElement("shape")->GetText();
    if (!shapeFile.empty() && shapeFile[0] == '/') shapeFile = shapeFile.substr(1);
    std::string imagePath = imageDir + "/" + shapeFile + ".png";
    SDL_Surface* surf = IMG_Load(imagePath.c_str());
    if (!surf) {
        std::cerr << "Failed to load font image: " << imagePath << std::endl;
        return false;
    }
    texture_ = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    auto chardefs = fontElem->FirstChildElement("chardefs");
    for (auto chardef = chardefs->FirstChildElement("chardef"); chardef; chardef = chardef->NextSiblingElement("chardef")) {
        const char* charAttr = chardef->Attribute("char");
        if (!charAttr) continue;
        char c = charAttr[0];
        FontChar fc;
        auto rectElem = chardef->FirstChildElement("rect");
        if (rectElem) {
            rectElem->QueryIntAttribute("x", &fc.rect.x);
            rectElem->QueryIntAttribute("y", &fc.rect.y);
            rectElem->QueryIntAttribute("w", &fc.rect.w);
            rectElem->QueryIntAttribute("h", &fc.rect.h);
        }
        auto widthElem = chardef->FirstChildElement("width");
        if (widthElem) widthElem->QueryIntText(&fc.width);
        else fc.width = fc.rect.w;
        auto offsetElem = chardef->FirstChildElement("offset");
        if (offsetElem) {
            offsetElem->QueryIntAttribute("x", &fc.offset.x);
            offsetElem->QueryIntAttribute("y", &fc.offset.y);
        }
        chars_[c] = fc;
    }
    return true;
}

bool BitmapFont::LoadOverlay(const std::string& overlayImagePath, SDL_Renderer* renderer) {
    if (overlayTexture_) {
        SDL_DestroyTexture(overlayTexture_);
        overlayTexture_ = nullptr;
    }
    SDL_Surface* surf = IMG_Load(overlayImagePath.c_str());
    if (!surf) {
        std::cerr << "Failed to load font overlay image: " << overlayImagePath << std::endl;
        return false;
    }
    overlayTexture_ = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return overlayTexture_ != nullptr;
}

void BitmapFont::RenderText(SDL_Renderer* renderer, const std::string& text, int x, int y, bool useOverlay) {
    int cursor = x;
    for (char c : text) {
        if (c == ' ') {
            cursor += 16;
            continue;
        }
        auto it = chars_.find(c);
        if (it == chars_.end()) continue;
        const FontChar& fc = it->second;
        SDL_Rect dst = { cursor + fc.offset.x, y + fc.offset.y, fc.rect.w, fc.rect.h };
        SDL_RenderCopy(renderer, texture_, &fc.rect, &dst);
        if (useOverlay && overlayTexture_) {
            SDL_RenderCopy(renderer, overlayTexture_, &fc.rect, &dst);
        }
        cursor += fc.width + tracking_;
    }
}

int BitmapFont::GetTextWidth(const std::string& text) const {
    int width = 0;
    for (char c : text) {
        if (c == ' ') {
            width += 16;
            continue;
        }
        auto it = chars_.find(c);
        if (it != chars_.end()) {
            width += it->second.width + tracking_;
        }
    }
    return width;
}
