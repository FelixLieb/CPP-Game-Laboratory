#pragma once
#include <random>
#include "Grid.h"
#include "Snake.h"
#include "ScoreManager.h"
#include <vector>
class Game
{
public:
	Game();
	void Run();

private:
	enum class State {
		MainMenu,
		Help,
		Playing,
		Outro,
		Quit
	};

	struct GameConfig
	{
		bool wrapMode{ false };
		float speed{ 1.0f };
		int initialLength{ 3 };
	};
	
	void RenderMainMenu() const;
	void RenderHelp() const;
	void RenderOutro() const;

	void StartNewRun();
	void UpdatePlaying(char command);
	void RenderPlaying();

	char ReadCommand(const char* prompt) const;
	void ClearScreen() const;

	Position RandomEmptyCell() const;
	static bool TryCommandToDirection(char c, Direction& outDir);

private:
	State m_state{ State::MainMenu };
	
	GameConfig gameCfg{};

	Grid m_grid;
	Snake m_snake;
	Position m_food{ 0,0 };

	bool m_alive{ true };
	int m_score{ 0 };

	ScoreManager m_scores;

	mutable std::mt19937 m_rng;
};

