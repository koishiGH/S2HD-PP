#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <SDL2/SDL.h>

class ResourceManager {
public:
    static ResourceManager& GetInstance();

    bool Initialize(const std::string& dataPath);
    void SetRenderer(SDL_Renderer* renderer);
    void Shutdown();

    SDL_Texture* LoadTexture(const std::string& path);
    void* LoadSound(const std::string& path);
    std::string LoadText(const std::string& path);

    void UnloadTexture(const std::string& path);
    void UnloadSound(const std::string& path);

private:
    ResourceManager() = default;
    ~ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::string dataPath_;
    SDL_Renderer* renderer_;

    std::unordered_map<std::string, SDL_Texture*> textureCache_;
    std::unordered_map<std::string, void*> soundCache_;
}; 