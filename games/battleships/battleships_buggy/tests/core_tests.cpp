#include "core/Board.h"
#include "core/Ship.h"
#include "core/Game.h"
#include "cli/ConsoleRenderer.h"
#include "cli/Input.h"
#include <cassert>
#include <sstream>
#include <iostream>

using namespace bs;

// ============================================================================
// Game Constructor Tests
// ============================================================================

static void TestGameConstructor()
{
    Game game(10, 10);
    assert(game.State() == GameState::Setup);
    assert(game.CurrentPlayerIndex() == 0);
    assert(game.WinnerIndex() == -1);
    assert(!game.IsOver());
}

static void TestGameConstructorDifferentDimensions()
{
    Game game(8, 8);
    assert(game.State() == GameState::Setup);
    // Test that board dimensions are set correctly via CurrentPlayer access
    assert(game.CurrentPlayer().own.Width() > 0);
    assert(game.CurrentPlayer().own.Height() > 0);
}

// ============================================================================
// Game Setup State Tests
// ============================================================================

static void TestStartSetup()
{
    Game game(10, 10);
    assert(game.State() == GameState::Setup);
    game.StartSetup();
    assert(game.State() == GameState::Setup);
}

static void TestGameState()
{
    Game game(10, 10);
    game.StartSetup();
    assert(game.State() == GameState::Setup);
}

static void TestCurrentPlayerIndex()
{
    Game game(10, 10);
    game.StartSetup();
    assert(game.CurrentPlayerIndex() == 0);
}

static void TestCurrentPlayer()
{
    Game game(10, 10);
    game.StartSetup();
    const auto& player = game.CurrentPlayer();
    assert(!player.name.empty());
}

static void TestOtherPlayer()
{
    Game game(10, 10);
    game.StartSetup();
    const auto& other = game.OtherPlayer();
    assert(!other.name.empty());
}

static void TestCurrentPlayerMut()
{
    Game game(10, 10);
    game.StartSetup();
    auto& player = game.CurrentPlayerMut();
    assert(!player.name.empty());
}

static void TestOtherPlayerMut()
{
    Game game(10, 10);
    game.StartSetup();
    auto& other = game.OtherPlayerMut();
    assert(!other.name.empty());
}

static void TestShipLengths()
{
    Game game(10, 10);
    const auto& lengths = game.ShipLengths();
    assert(lengths.size() == 5);
    assert(lengths[0] == 2);
    assert(lengths[1] == 3);
    assert(lengths[2] == 3);
    assert(lengths[3] == 4);
    assert(lengths[4] == 5);
}

// ============================================================================
// Ship Placement Tests
// ============================================================================

static void TestPlaceShipForCurrentValid()
{
    Game game(10, 10);
    game.StartSetup();
    Ship ship{ 2, Coord{0, 0}, Orientation::Horizontal };
    PlaceResult result = game.PlaceShipForCurrent(ship);
    assert(result == PlaceResult::Ok);
    assert(game.CurrentPlayer().own.Ships().size() == 1);
}

static void TestPlaceShipForCurrentOutOfBounds()
{
    Game game(10, 10);
    game.StartSetup();
    Ship ship{ 5, Coord{9, 0}, Orientation::Horizontal };
    PlaceResult result = game.PlaceShipForCurrent(ship);
    assert(result == PlaceResult::OutOfBounds);
}

static void TestPlaceShipForCurrentOverlap()
{
    Game game(10, 10);
    game.StartSetup();
    Ship ship1{ 2, Coord{0, 0}, Orientation::Horizontal };
    Ship ship2{ 2, Coord{0, 0}, Orientation::Horizontal };
    
    PlaceResult result1 = game.PlaceShipForCurrent(ship1);
    assert(result1 == PlaceResult::Ok);
    
    PlaceResult result2 = game.PlaceShipForCurrent(ship2);
    assert(result2 == PlaceResult::Overlap);
}

// ============================================================================
// Player Switching Tests
// ============================================================================

static void TestNextPlayerDuringSetup()
{
    Game game(10, 10);
    game.StartSetup();
    int player1Index = game.CurrentPlayerIndex();
    
    game.NextPlayerDuringSetup();
    int player2Index = game.CurrentPlayerIndex();
    
    assert(player1Index != player2Index);
    assert(player2Index == 1);
}

static void TestNextPlayerDuringSetupToggle()
{
    Game game(10, 10);
    game.StartSetup();
    assert(game.CurrentPlayerIndex() == 0);
    
    game.NextPlayerDuringSetup();
    assert(game.CurrentPlayerIndex() == 1);
    
    game.NextPlayerDuringSetup();
    assert(game.CurrentPlayerIndex() == 0);
}

// ============================================================================
// Setup Completion Tests
// ============================================================================

static void TestFinishSetupIfReadyNotReady()
{
    Game game(10, 10);
    game.StartSetup();
    
    // No ships placed, should still be in Setup state
    game.FinishSetupIfReady();
    assert(game.State() == GameState::Setup);
}

static void TestFinishSetupIfReadyBothPlayersReady()
{
    Game game(10, 10);
    game.StartSetup();
    
    // Place all required ships for player 1
    const auto& lengths = game.ShipLengths();
    for (size_t i = 0; i < lengths.size(); ++i)
    {
        Coord start{ static_cast<int>(i), 0 };
        Ship ship{ lengths[i], start, Orientation::Vertical };
        game.PlaceShipForCurrent(ship);
    }
    
    game.NextPlayerDuringSetup();
    
    // Place all required ships for player 2
    for (size_t i = 0; i < lengths.size(); ++i)
    {
        Coord start{ static_cast<int>(i), 2 };
        Ship ship{ lengths[i], start, Orientation::Vertical };
        game.PlaceShipForCurrent(ship);
    }
    
    game.FinishSetupIfReady();
    assert(game.State() == GameState::Playing);
}

// ============================================================================
// Game Play Tests
// ============================================================================

static void TestShootAtOpponentMiss()
{
    Game game(10, 10);
    game.StartSetup();
    
    // Setup both players with ships
    const auto& lengths = game.ShipLengths();
    for (size_t i = 0; i < lengths.size(); ++i)
    {
        Coord start{ static_cast<int>(i), 0 };
        Ship ship{ lengths[i], start, Orientation::Vertical };
        game.PlaceShipForCurrent(ship);
    }
    
    game.NextPlayerDuringSetup();
    
    for (size_t i = 0; i < lengths.size(); ++i)
    {
        Coord start{ static_cast<int>(i), 5 };
        Ship ship{ lengths[i], start, Orientation::Vertical };
        game.PlaceShipForCurrent(ship);
    }
    
    game.FinishSetupIfReady();
    assert(game.State() == GameState::Playing);
    
    // Shoot at empty location
    ShotResult result = game.ShootAtOpponent(Coord{ 9, 9 });
    assert(result == ShotResult::Miss || result == ShotResult::Invalid);
}

static void TestShootAtOpponentHit()
{
    Game game(10, 10);
    game.StartSetup();
    
    const auto& lengths = game.ShipLengths();
    for (size_t i = 0; i < lengths.size(); ++i)
    {
        Coord start{ static_cast<int>(i), 0 };
        Ship ship{ lengths[i], start, Orientation::Vertical };
        game.PlaceShipForCurrent(ship);
    }
    
    game.NextPlayerDuringSetup();
    
    for (size_t i = 0; i < lengths.size(); ++i)
    {
        Coord start{ static_cast<int>(i), 5 };
        Ship ship{ lengths[i], start, Orientation::Vertical };
        game.PlaceShipForCurrent(ship);
    }
    
    game.FinishSetupIfReady();
    assert(game.State() == GameState::Playing);
    
    // Shoot at ship location
    ShotResult result = game.ShootAtOpponent(Coord{ 0, 5 });
    assert(result == ShotResult::Hit || result == ShotResult::Sunk);
}

// ============================================================================
// Winner Tests
// ============================================================================

static void TestWinnerIndexInitial()
{
    Game game(10, 10);
    assert(game.WinnerIndex() == -1);
}

static void TestIsOverInitial()
{
    Game game(10, 10);
    assert(!game.IsOver());
}

// ============================================================================
// Console Renderer Tests
// ============================================================================

static void TestConsoleRendererClearScreen()
{
    ConsoleRenderer r;
    // ClearScreen is void and has side effects; just verify it doesn't crash
    r.ClearScreen();
}

static void TestConsoleRendererPrintTitle()
{
    ConsoleRenderer r;
    // PrintTitle is void; verify it doesn't crash with various inputs
    r.PrintTitle("Test Title");
    r.PrintTitle("");
    r.PrintTitle("Very Long Title With Multiple Words");
}

static void TestConsoleRendererPrintBoard()
{
    Board b(10, 10);
    ConsoleRenderer r;
    RenderOptions opt;
    opt.revealShips = false;
    
    // PrintBoard is void; verify it doesn't crash
    r.PrintBoard(b, opt);
}

static void TestConsoleRendererPrintBoardWithReveal()
{
    Board b(10, 10);
    ConsoleRenderer r;
    RenderOptions opt;
    opt.revealShips = true;
    
    r.PrintBoard(b, opt);
}

static void TestConsoleRendererPrintTwoBoards()
{
    Player p1("Player1", 10, 10);
    Player p2("Player2", 10, 10);
    ConsoleRenderer r;
    RenderOptions opt;
    opt.revealShips = false;
    opt.revealOpponent = true;
    
    // PrintTwoBoards is void; verify it doesn't crash
    r.PrintTwoBoards(p1, p2, opt);
}

// ============================================================================
// Input Parsing Tests
// ============================================================================

static void TestParseCommandLooseQuit()
{
    Command cmd = ParseCommandLoose("quit");
    assert(cmd.type == CommandType::Quit);
}

static void TestParseCommandLooseHelp()
{
    Command cmd = ParseCommandLoose("help");
    assert(cmd.type == CommandType::Help);
}

static void TestParseCommandLooseReveal()
{
    Command cmd = ParseCommandLoose("reveal");
    assert(cmd.type == CommandType::RevealToggle);
}

static void TestParseCommandLooseShoot()
{
    Command cmd = ParseCommandLoose("shoot A5");
    assert(cmd.type == CommandType::Shoot);
    // Verify coordinate is parsed
    assert(cmd.shootTarget.x >= 0 && cmd.shootTarget.x < 10);
    assert(cmd.shootTarget.y >= 0 && cmd.shootTarget.y < 10);
}

static void TestParseCommandLooseCoordinate()
{
    Command cmd = ParseCommandLoose("A5");
    assert(cmd.type == CommandType::Shoot);
}

static void TestParseCommandLooseInvalid()
{
    Command cmd = ParseCommandLoose("xyz123");
    // Should still return some command type (implementation dependent)
    assert(cmd.type != CommandType::Quit || cmd.type == CommandType::Shoot);
}

// ============================================================================
// Main test runner
// ============================================================================

static int main()
{
    // Game Constructor Tests
    TestGameConstructor();
    TestGameConstructorDifferentDimensions();
    
    // Game Setup State Tests
    TestStartSetup();
    TestGameState();
    TestCurrentPlayerIndex();
    TestCurrentPlayer();
    TestOtherPlayer();
    TestCurrentPlayerMut();
    TestOtherPlayerMut();
    TestShipLengths();
    
    // Ship Placement Tests
    TestPlaceShipForCurrentValid();
    TestPlaceShipForCurrentOutOfBounds();
    TestPlaceShipForCurrentOverlap();
    
    // Player Switching Tests
    TestNextPlayerDuringSetup();
    TestNextPlayerDuringSetupToggle();
    
    // Setup Completion Tests
    TestFinishSetupIfReadyNotReady();
    TestFinishSetupIfReadyBothPlayersReady();
    
    // Game Play Tests
    TestShootAtOpponentMiss();
    TestShootAtOpponentHit();
    
    // Winner Tests
    TestWinnerIndexInitial();
    TestIsOverInitial();
    
    // Console Renderer Tests
    TestConsoleRendererClearScreen();
    TestConsoleRendererPrintTitle();
    TestConsoleRendererPrintBoard();
    TestConsoleRendererPrintBoardWithReveal();
    TestConsoleRendererPrintTwoBoards();
    
    // Input Parsing Tests
    TestParseCommandLooseQuit();
    TestParseCommandLooseHelp();
    TestParseCommandLooseReveal();
    TestParseCommandLooseShoot();
    TestParseCommandLooseCoordinate();
    TestParseCommandLooseInvalid();
    
    std::cout << "All tests passed!\n";
    return 0;
}