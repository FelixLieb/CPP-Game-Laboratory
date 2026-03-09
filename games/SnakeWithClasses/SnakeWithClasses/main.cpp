#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include "Game.h"

// -------------------- Small helpers --------------------

void ClearInputLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int ReadIntInRange(const std::string& prompt, int minVal, int maxVal)
{
    while (true)
    {
        std::cout << prompt;
        int x{}; // Initialize x to 0
        if (std::cin >> x && x >= minVal && x <= maxVal)
        {
            ClearInputLine();
            return x;
        }

        std::cin.clear();
        ClearInputLine();
        std::cout << "Invalid input. Enter a number " << minVal << "�" << maxVal << ".\n";
    }
}

char ReadCommand(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string s;
        if (std::getline(std::cin, s))
        {
            if (!s.empty())
                return static_cast<char>(std::toupper(static_cast<unsigned char>(s[0])));
        }
        std::cout << "Please enter a command.\n";
    }
}

// -------------------- Game types --------------------

struct Vec2
{
    int x{}; // curly brackets means value-initialization, which for int means zero-initialization
    int y{};
    // We do this to ensure that if we ever create a Vec2 without explicitly setting x and y, they will start at 0 instead of being uninitialized garbage values.
};

// enum vs enum class: enum class is scoped and doesn't implicitly convert to int, which is safer and more modern C++
// If we used plain enum, the values would be in the global namespace and could conflict with other identifiers. With enum class, we have to write GameState::MainMenu, which is clearer and prevents naming conflicts.
enum class GameState
{
    MainMenu,
    Help,
    Playing,
    Quit
};

// A struct is used to group related data, in this case config parameters for the game.
// Structs are public by default, as opposed to classes which are private by default.
struct GameConfig
{
    int width = 12;
    int height = 8;
    char emptyCell = '.';
    char playerCell = '@';
};

// Store 2D grid in 1D: index = y * width + x
int Index(int x, int y, int width)
{
    return y * width + x;
}

// -------------------- Rendering --------------------

void RenderMenu()
{
    std::cout << "\n=== SNAKE (Part 1 Prototype) ===\n";
    std::cout << "1) Play\n";
    std::cout << "2) Controls / Help\n";
    std::cout << "3) Quit\n";
}

void RenderHelp()
{
    std::cout << "\n--- Help ---\n";
    std::cout << "Move with: W A S D\n";
    std::cout << "Quit to menu during play: Q\n";
    std::cout << "This is the grid-movement prototype that becomes Snake later.\n\n";
}

void RenderGrid(const GameConfig& cfg, const std::vector<char>& grid)
{
    std::cout << "\n";
    for (int y = 0; y < cfg.height; ++y)
    {
        for (int x = 0; x < cfg.width; ++x)
        {
            std::cout << grid[Index(x, y, cfg.width)] << ' ';
        }
        std::cout << "\n";
    }
}

// -------------------- Game logic --------------------

void ClearGrid(const GameConfig& cfg, std::vector<char>& grid)
{
    // assign() is a convenient way to fill a vector with a specific value. It resizes the vector to the specified size and fills it with the given value.
    grid.assign(cfg.width * cfg.height, cfg.emptyCell);
}

void PlacePlayer(const GameConfig& cfg, std::vector<char>& grid, const Vec2& p)
{
    grid[Index(p.x, p.y, cfg.width)] = cfg.playerCell;
}

void WrapPosition(const GameConfig& cfg, Vec2& p)
{
    // Wrap-around behavior
    if (p.x < 0) p.x = cfg.width - 1;
    if (p.x >= cfg.width) p.x = 0;
    if (p.y < 0) p.y = cfg.height - 1;
    if (p.y >= cfg.height) p.y = 0;
}

// Alternative: clamp instead of wrap
// void ClampPosition(const GameConfig& cfg, Vec2& p)
// {
//     if (p.x < 0) p.x = 0;
//     if (p.x >= cfg.width) p.x = cfg.width - 1;
//     if (p.y < 0) p.y = 0;
//     if (p.y >= cfg.height) p.y = cfg.height - 1;
// }

bool UpdatePlayerFromInput(const GameConfig& cfg, Vec2& player, char cmd)
{
    // returns false if user wants to exit play state
    if (cmd == 'Q')
        return false;

    Vec2 next = player;

    switch (cmd)
    {
    case 'W': next.y -= 1; break;
    case 'S': next.y += 1; break;
    case 'A': next.x -= 1; break;
    case 'D': next.x += 1; break;
    default:
        // ignore unknown commands
        return true;
    }

    player = next;
    WrapPosition(cfg, player);   // swap to ClampPosition if desired
    return true;
}

void RunPlayLoop(const GameConfig& cfg)
{
    // Vectors are dynamic arrays that can resize themselves. They manage their own memory and provide convenient functions for adding, removing, and accessing elements. Here we use a vector of chars to represent the grid, which allows us to easily fill it with the empty cell character and update it with the player's position.
    std::vector<char> grid;
    Vec2 player{ cfg.width / 2, cfg.height / 2 };

    bool playing = true;
    while (playing)
    {
        // Render step
        ClearGrid(cfg, grid);
        PlacePlayer(cfg, grid, player);
        RenderGrid(cfg, grid);

        // Input step (simple: one command per "turn")
        char cmd = ReadCommand("Command (WASD, Q=menu): ");

        // Update step
        playing = UpdatePlayerFromInput(cfg, player, cmd);
    }
}

// -------------------- Main program --------------------

int main()
{
    Game game;
    game.Run();
    return 0;
}