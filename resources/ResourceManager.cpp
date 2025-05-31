#include "ResourceManager.hpp"
#include "../core/ModManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <filesystem>

ResourceManager& ResourceManager::GetInstance() {
    static ResourceManager instance;
    return instance;
}

ResourceManager::~ResourceManager() {
    Shutdown();
}

bool ResourceManager::Initialize(const std::string& dataPath) {
    dataPath_ = dataPath;    
    std::filesystem::path exePath = std::filesystem::current_path();
    
    std::filesystem::path modsPath = exePath / "mods";
    
    if (!ModManager::GetInstance().Initialize(modsPath)) {
        std::cerr << "Failed to initialize mods!" << std::endl;
        return false;
    }
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void ResourceManager::SetRenderer(SDL_Renderer* renderer) {
    renderer_ = renderer;
}

void ResourceManager::Shutdown() {
    for (auto& pair : textureCache_) {
        SDL_DestroyTexture(pair.second);
    }
    textureCache_.clear();

    for (auto& pair : soundCache_) {
        Mix_FreeChunk(static_cast<Mix_Chunk*>(pair.second));
    }
    soundCache_.clear();

    Mix_Quit();
    IMG_Quit();
}

SDL_Texture* ResourceManager::LoadTexture(const std::string& path) {
    if (!renderer_) {
        std::cerr << "Renderer not set in ResourceManager!" << std::endl;
        return nullptr;
    }

    auto it = textureCache_.find(path);
    if (it != textureCache_.end()) {
        return it->second;
    }

    std::filesystem::path resolvedPath = ModManager::GetInstance().ResolveAssetPath(path);
    std::string fullPath;
    
    if (resolvedPath.is_absolute()) {
        fullPath = resolvedPath.string();
    } else {
        fullPath = dataPath_ + "/" + resolvedPath.string();
        std::cout << "Loading: data/SONICORCA/" << path << std::endl;
    }
    
    SDL_Surface* loadedSurface = IMG_Load(fullPath.c_str());
    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, loadedSurface);
    SDL_FreeSurface(loadedSurface);

    if (texture == nullptr) {
        std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    textureCache_[path] = texture;
    return texture;
}

void* ResourceManager::LoadSound(const std::string& path) {
    auto it = soundCache_.find(path);
    if (it != soundCache_.end()) {
        return it->second;
    }

    std::filesystem::path resolvedPath = ModManager::GetInstance().ResolveAssetPath(path);
    std::string fullPath = dataPath_ + "/" + resolvedPath.string();
    
    Mix_Chunk* sound = Mix_LoadWAV(fullPath.c_str());
    if (sound == nullptr) {
        std::cerr << "Unable to load sound " << path << "! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return nullptr;
    }

    soundCache_[path] = sound;
    return sound;
}

std::string ResourceManager::LoadText(const std::string& path) {
    std::filesystem::path resolvedPath = ModManager::GetInstance().ResolveAssetPath(path);
    std::string fullPath = dataPath_ + "/" + resolvedPath.string();
    
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << path << "!" << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void ResourceManager::UnloadTexture(const std::string& path) {
    auto it = textureCache_.find(path);
    if (it != textureCache_.end()) {
        SDL_DestroyTexture(it->second);
        textureCache_.erase(it);
    }
}

void ResourceManager::UnloadSound(const std::string& path) {
    auto it = soundCache_.find(path);
    if (it != soundCache_.end()) {
        Mix_FreeChunk(static_cast<Mix_Chunk*>(it->second));
        soundCache_.erase(it);
    }
} 