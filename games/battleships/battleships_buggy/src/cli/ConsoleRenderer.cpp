#include "cli/ConsoleRenderer.h"
#include <iostream>
#include <cstdlib>

namespace bs {

    static char CellChar(Cell c, bool revealShips)
    {
        switch (c)
        {
        case Cell::Empty: return '.';
        case Cell::Miss:  return 'o';
        case Cell::Hit:   return 'X';
        case Cell::Ship:  return revealShips ? '#' : '.';
        default:          return '?';
        }
    }

    void ConsoleRenderer::ClearScreen() const
    {
        // Not portable, but acceptable for the lab starter.
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    void ConsoleRenderer::PrintTitle(const std::string& title) const
    {
        std::cout << "==== " << title << " ====\n\n";
    }

    void ConsoleRenderer::PrintBoard(const Board& board, const RenderOptions& opt) const
    {
        if (opt.showLegend)
            std::cout << "Legend: . unknown/empty, o miss, X hit, # ship(reveal)\n\n";

        std::cout << "  ";
        for (int x = 0; x < board.Width(); ++x)
            std::cout << static_cast<char>('A' + x) << ' ';
        std::cout << "\n";

        for (int y = 0; y < board.Height(); ++y)
        {
            std::cout << (y + 1);
            if (y + 1 < 10) std::cout << ' ';
            for (int x = 0; x < board.Width(); ++x)
            {
                Coord c{ x,y };
                std::cout << CellChar(board.GetCell(c), opt.revealShips) << ' ';
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    void ConsoleRenderer::PrintTwoBoards(const Player& p, const Player& opponent, const RenderOptions& opt) const
    {
        std::cout << p.name << " tracking board (shots at " << opponent.name << "):\n";
        PrintBoard(p.tracking, RenderOptions{ false, false });

        std::cout << p.name << " own board:\n";
        PrintBoard(p.own, opt);
    }

} // namespace bs