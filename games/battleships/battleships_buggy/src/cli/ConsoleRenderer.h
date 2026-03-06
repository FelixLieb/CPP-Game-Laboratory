#pragma once
#include "core/Game.h"
#include <string>

namespace bs {

    struct RenderOptions
    {
        bool revealShips{ false };
        bool showLegend{ true };
    };

    class ConsoleRenderer
    {
    public:
        void ClearScreen() const;
        void PrintTitle(const std::string& title) const;
        void PrintBoard(const Board& board, const RenderOptions& opt) const;
        void PrintTwoBoards(const Player& p, const Player& opponent, const RenderOptions& opt) const;
    };

} // namespace bs