#pragma once
#include "core/Coord.h"
#include "core/Enums.h"
#include <string>

namespace bs {

    enum class CommandType : std::uint8_t
    {
        Unknown,
        Place,
        Shoot,
        Help,
        Quit,
        RevealToggle
    };

    struct Command
    {
        CommandType type{ CommandType::Unknown };

        // Place
        Coord placeStart{ 0,0 };
        Orientation placeOrientation{ Orientation::Horizontal };
        int placeLength{ 0 };

        // Shoot
        Coord shootTarget{ 0,0 };

        std::string raw;
    };

    Command ParseCommandLoose(const std::string& line);

} // namespace bs