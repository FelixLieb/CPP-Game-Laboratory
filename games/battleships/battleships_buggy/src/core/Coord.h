#pragma once
#include <string>

namespace bs {

    struct Coord
    {
        int x{ 0 }; // column, 0-based
        int y{ 0 }; // row, 0-based
    };

    inline bool operator==(const Coord& a, const Coord& b) { return a.x == b.x && a.y == b.y; }

    // NOTE (intentionally buggy / permissive):
    // - Accepts "A5", "5A", "A,5", "A 5" etc.
    // - Returns {0,0} on failure (bug: failure not distinct).
    Coord ParseCoordLoose(const std::string& text);

    std::string ToHumanCoord(Coord c); // e.g. (0,0) -> "A1"

} // namespace bs