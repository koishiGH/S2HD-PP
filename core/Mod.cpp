#include "Mod.hpp"
#include <fstream>
#include <iostream>

Mod::Mod(const std::filesystem::path& modPath)
    : modPath_(modPath), priority_(0), enabled_(true) {
}

bool Mod::Load() {
    std::filesystem::path modInfoPath = modPath_ / "mod.json";
    if (!std::filesystem::exists(modInfoPath)) {
        std::cerr << "Mod info file not found: " << modInfoPath << std::endl;
        return false;
    }

    try {
        std::ifstream file(modInfoPath);
        nlohmann::json json;
        file >> json;

        name_ = json["name"].get<std::string>();
        version_ = json["version"].get<std::string>();
        author_ = json["author"].get<std::string>();
        description_ = json["description"].get<std::string>();
        priority_ = json["priority"].get<int>();
        enabled_ = json.value("enabled", true);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading mod info: " << e.what() << std::endl;
        return false;
    }
} 