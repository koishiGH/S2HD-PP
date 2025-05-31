#pragma once

#include <string>
#include <vector>

namespace TitleResources {
    const std::string ANIMATION_GROUP = "TITLE/ANIGROUP";
    const std::string FONT = "FONTS/HUD.font";
    const std::string SPARKLE_SOUND = "SOUND/SPARKLE";
    const std::string SHOOTING_STAR_SOUND = "SOUND/SHOOTINGSTAR";
    const std::string MUSIC = "TITLE/MUSIC";
    const std::string BACKGROUND_SKY = "TITLE/BACKGROUND/SKY.png";
    const std::string BACKGROUND_ISLAND = "TITLE/BACKGROUND/ISLAND.png";
    const std::string BACKGROUND_DEATHEGG = "TITLE/BACKGROUND/DEATHEGG.png";
    const std::string WIPE = "TITLE/WIPE.png";

    const int HD_ANIMATION = 0;
    const int THE_HEDGEHOG_ANIMATION = 1;
    const int SONIC2_ANIMATION = 2;
    const int SPINNING_STAR_ANIMATION = 3;
    const int SONIC_ANIMATION = 4;
    const int SONIC_HAND_ANIMATION = 5;
    const int TAILS_ANIMATION = 6;
    const int TAILS_TAILS_ANIMATION = 7;
    const int SPARKLE_ANIMATION = 8;
    const int SHOOTING_STAR_ANIMATION = 9;
    const int WATER_SPARKLE_ANIMATION = 10;

    std::vector<std::string> GetAllResourcePaths();
}; 