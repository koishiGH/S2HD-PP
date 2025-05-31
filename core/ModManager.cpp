#include "ModManager.hpp"
#include <algorithm>
#include <iostream>

ModManager& ModManager::GetInstance() {
    static ModManager instance;
    return instance;
}

bool ModManager::Initialize(const std::filesystem::path& modsPath) {
    modsPath_ = modsPath;
    
    if (!std::filesystem::exists(modsPath_)) {
        std::filesystem::create_directories(modsPath_);
    }

    for (const auto& entry : std::filesystem::directory_iterator(modsPath_)) {
        if (entry.is_directory()) {
            auto mod = std::make_unique<Mod>(entry.path());
            if (mod->Load()) {
                std::cout << "Loaded mod: " << mod->GetName() << " (enabled: " << mod->IsEnabled() << ")" << std::endl;
                mods_.push_back(std::move(mod));
            }
        }
    }

    std::sort(mods_.begin(), mods_.end(), 
        [](const auto& a, const auto& b) {
            return a->GetPriority() > b->GetPriority();
        });

    return true;
}

void ModManager::Shutdown() {
    mods_.clear();
}

std::filesystem::path ModManager::ResolveAssetPath(const std::filesystem::path& originalPath) const {
    std::string pathStr = originalPath.string();
    
    for (const auto& mod : mods_) {
        if (mod->IsEnabled()) {
            std::filesystem::path modPath = mod->GetPath() / "data" / "SONICORCA" / pathStr;
            
            if (std::filesystem::exists(modPath)) {
                std::cout << "Using mod '" << mod->GetName() << "' at: " << modPath.string() << std::endl;
                return modPath;
            }
        }
    }
    
    return originalPath;
} 