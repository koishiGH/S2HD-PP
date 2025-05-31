#pragma once

#include "Mod.hpp"
#include <vector>
#include <memory>
#include <filesystem>

class ModManager {
public:
    static ModManager& GetInstance();

    bool Initialize(const std::filesystem::path& modsPath);
    void Shutdown();

    std::filesystem::path ResolveAssetPath(const std::filesystem::path& originalPath) const;

private:
    ModManager() = default;
    ~ModManager() = default;

    std::vector<std::unique_ptr<Mod>> mods_;
    std::filesystem::path modsPath_;
}; 