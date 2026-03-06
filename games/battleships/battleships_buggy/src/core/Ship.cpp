#include "core/Ship.h"

namespace bs {

    Ship::Ship(int len, Coord s, Orientation o)
        : length(len), start(s), orientation(o), hits(static_cast<std::size_t>(len), false)
    {
    }

    bool Ship::IsSunk() const
    {
        for (bool h : hits) if (!h) return false;
        return true;
    }

    bool Ship::Covers(Coord c) const
    {
        return SegmentIndex(c) != -1;
    }

    int Ship::SegmentIndex(Coord c) const
    {
        if (orientation == Orientation::Horizontal)
        {
            if (c.y != start.y) return -1;
            int dx = c.x - start.x;
            if (dx < 0 || dx >= length) return -1;
            return dx;
        }

        // Vertical
        if (c.x != start.x) return -1;
        int dy = c.y - start.y;
        if (dy < 0 || dy >= length) return -1;
        return dy;
    }

} // namespace bs