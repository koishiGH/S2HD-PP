#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>

class Mod {
public:
    Mod(const std::filesystem::path& modPath);
    ~Mod() = default;

    bool Load();
    const std::string& GetName() const { return name_; }
    const std::string& GetVersion() const { return version_; }
    const std::string& GetAuthor() const { return author_; }
    const std::string& GetDescription() const { return description_; }
    int GetPriority() const { return priority_; }
    bool IsEnabled() const { return enabled_; }
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    const std::filesystem::path& GetPath() const { return modPath_; }

private:
    std::filesystem::path modPath_;
    std::string name_;
    std::string version_;
    std::string author_;
    std::string description_;
    int priority_;
    bool enabled_;
}; 