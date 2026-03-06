#include "cli/Input.h"
#include "core/Coord.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace bs {

    static std::string ToLower(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return s;
    }

    static std::vector<std::string> Split(const std::string& line)
    {
        std::istringstream iss(line);
        std::vector<std::string> parts;
        std::string p;
        while (iss >> p) parts.push_back(p);
        return parts;
    }

    Command ParseCommandLoose(const std::string& line)
    {
        Command cmd;
        cmd.raw = line;
        auto parts = Split(line);
        if (parts.empty()) return cmd;

        std::string head = ToLower(parts[0]);

        if (head == "q" || head == "quit" || head == "exit")
        {
            cmd.type = CommandType::Quit;
            return cmd;
        }
        if (head == "h" || head == "help")
        {
            cmd.type = CommandType::Help;
            return cmd;
        }
        if (head == "reveal")
        {
            cmd.type = CommandType::RevealToggle;
            return cmd;
        }

        // Shoot: "shoot A5" or just "A5"
        // NOTE: Intentionally permissive (and a bit wrong): most inputs become Shoot.
        if (head == "shoot" || !head.empty())
        {
            std::string coordText = (head == "shoot" && parts.size() >= 2) ? parts[1] : parts[0];
            cmd.type = CommandType::Shoot;
            cmd.shootTarget = ParseCoordLoose(coordText); // BUG: {0,0} on failure
            return cmd;
        }

        // Place command exists but is effectively unreachable here (intentional smell/bug).
        // Setup in main asks separately for orientation anyway.
        if (head == "place" && parts.size() >= 4)
        {
            cmd.type = CommandType::Place;
            cmd.placeStart = ParseCoordLoose(parts[1]);
            cmd.placeOrientation = (ToLower(parts[2]).rfind("v", 0) == 0) ? Orientation::Vertical : Orientation::Horizontal;
            cmd.placeLength = std::max(1, std::stoi(parts[3]));
            return cmd;
        }

        return cmd;
    }

} // namespace bs