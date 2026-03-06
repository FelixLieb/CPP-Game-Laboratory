#pragma once
#include <cstdint>

namespace bs {

    enum class Orientation : std::uint8_t
    {
        Horizontal,
        Vertical
    };

    enum class ShotResult : std::uint8_t
    {
        Miss,
        Hit,
        Sunk,
        AlreadyTried, // intended by spec, buggy code may not return it correctly
        Invalid
    };

    enum class PlaceResult : std::uint8_t
    {
        Ok,
        OutOfBounds,
        Overlap,
        Invalid
    };

} // namespace bs