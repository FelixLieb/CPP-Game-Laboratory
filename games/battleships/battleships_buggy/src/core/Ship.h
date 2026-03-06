#pragma once
#include "core/Coord.h"
#include "core/Enums.h"
#include <vector>

namespace bs {

    struct Ship
    {
        int length{ 2 };
        Coord start{ 0,0 };
        Orientation orientation{ Orientation::Horizontal };

        // Buggy design (intentional): hits tracked on ship, but board also tracks hit/miss in grid.
        // This double-bookkeeping is error-prone and intended for students to identify/refactor.
        std::vector<bool> hits; // size = length

        Ship() = default;
        Ship(int len, Coord s, Orientation o);

        bool IsSunk() const;
        bool Covers(Coord c) const;
        int  SegmentIndex(Coord c) const; // -1 if not covered
    };

} // namespace bs