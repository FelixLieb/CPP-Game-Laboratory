#include "Game.h"
#include <iostream>

Game::Game()
	: m_grid(16, 12, '.')
	, m_scores("scores.txt") // Creates a ScoreManager instance
	, m_rng(std::random_device{}())
{
	std::cout << "Game instance initialized." << std::endl;
}
void Game::Run()
{
	GameConfig gameCfg;

	while (m_state != State::Quit)
	{
		switch (m_state)
		{
		case State::MainMenu:
			RenderMainMenu();
			// We put the below code in brackets in order to 
			// create a small local scope for the cmd variable,
			// so it doesn't leak out into other parts of code outside this case.
			{
				const char cmd = ReadCommand("Choice: ");
				if (cmd == '1')
				{
					StartNewRun();
					m_state = State::Playing;
				}
				else if (cmd == '2')
					m_state = State::Help;
				else if (cmd == '3' || cmd == 'Q')
					m_state = State::Quit;
			}
			break;

		case State::Help:
			RenderHelp();
			(void)ReadCommand("Press Enter to return to menu...");
			m_state = State::MainMenu;
			break;

		case State::Playing:
			RenderPlaying();
			{
				const char cmd = ReadCommand("Command (WASD/Q): ");
				UpdatePlaying(cmd);

				if (!m_alive)
					m_state = State::Outro;
				else if (std::tolower(static_cast<unsigned char>(cmd)) == 'q')
					m_state = State::MainMenu;
			}
			break;

		case State::Outro:
			RenderOutro();
			{
				const char cmd = ReadCommand("Choice: ");
				if (cmd == '1')
				{
					StartNewRun();
					m_state = State::Playing;
				}
				else if (cmd == '2')
					m_state = State::MainMenu;
				else if (cmd == '3')
					m_state = State::Quit;
			}
			break;

		case State::Quit:
			break;
		}
	}
	ClearScreen();
}

void Game::StartNewRun()
{
	m_alive = true;
	m_score = 0;
	const Position start{ m_grid.Width() / 2, m_grid.Height() / 2 };

	m_snake.Reset(start, gameCfg.initialLength, Direction::Right);
	m_food = RandomEmptyCell(); // Food is represented by its coordinate.
}

void Game::UpdatePlaying(char command)
{
	const char currentCmd = static_cast<char>(std::tolower(static_cast<unsigned char> (command)));

	// menu escape handled by state machine after UpdatePlaying
	if (currentCmd == 'q')
		return;

	Direction d{}; // We create an "empty" direction
	if (TryCommandToDirection(currentCmd, d))
		// If the command was translated to a direction
		m_snake.SetNextDirection(d);

	// Predict next head position, to decide growth and check wall collision
	Position next = m_snake.Head();
	switch (m_snake.Dir())
	{
	case Direction::Up:		next.y -= 1; break;
	case Direction::Down:	next.y += 1; break;
	case Direction::Left:	next.x -= 1; break;
	case Direction::Right:	next.x += 1; break;
	}

	// Wall collision = death in classic snake
	if (!gameCfg.wrapMode && !m_grid.InBounds(next))
	{
		m_alive = false;
	}
	else
	{
		const bool willGrow = (next == m_food);
		m_snake.Move(willGrow);

		// Self collision
		if (m_snake.HitSelf())
		{
			m_alive = false;
		}

		// Eat food
		if (willGrow && m_alive)
		{
			m_score += 10;
			m_food = RandomEmptyCell();
		}
	}

	if (!m_alive)
	{
		if (m_scores.isATopScore(m_score))
		{
			std::string username = "Unknown player";
			std::cout << "Your score is in the top 10. Enter your name: \n";
			std::getline(std::cin, username);

			m_scores.AddScore(m_score, username);
			m_scores.SaveAppendLastScore();
			m_scores.Load(); // Reload so menu leaderboard includes new score
		}

	}
}

void Game::RenderPlaying()
{
	ClearScreen();

	m_grid.Clear();

	// Food
	m_grid.SetCell(m_food, '*');

	// Snake: head then body
	const auto& body = m_snake.Body();
	bool first = true;
	for (const Position& bodyPart : body)
	{
		m_grid.SetCell(bodyPart, first ? '@' : 'o');
		first = false;
	}

	std::cout << "SNAKE | Score: " << m_score << "\n";
	std::cout << m_grid.ToString() << "\n";
}

void Game::RenderMainMenu() const
{
	ClearScreen();

	std::cout << "=== SNAKE (Terminal) ===\n\n";
	std::cout << "1) Play\n";
	std::cout << "2) Controls / Help\n";
	std::cout << "3) Quit\n\n";

	std::cout << "Top scores:\n";
	const auto top = m_scores.TopScores();
	if (top.empty())
	{
		std::cout << "  (no scores yet)\n";
	}
	else
	{
		for (size_t i = 0; i < top.size(); ++i)
			std::cout << "  " << (i + 1) << ") " << top[i].score << " - " <<top[i].username << "\n";
	}
	std::cout << "\n";
}

void Game::RenderHelp() const
{
	ClearScreen();
	std::cout << "=== Controls / Help ===\n\n";
	std::cout << "Move: W A S D\n";
	std::cout << "Quit to menu during play: Q\n\n";
	std::cout << "Rules:\n";
	std::cout << "- Eat '*' to gain score and grow.\n";
	std::cout << "- Hitting walls or your own body ends the run.\n\n";
}

void Game::RenderOutro() const
{
	ClearScreen();
	std::cout << "=== Game Over ===\n\n";
	std::cout << "Final score: " << m_score << "\n\n";

	std::cout << "Top scores:\n";
	const auto top = m_scores.TopScores();
	if (top.empty())
	{
		std::cout << "  (no scores yet)\n";
	}
	else
	{
		for (size_t i = 0; i < top.size(); ++i)
			std::cout << "  " << (i + 1) << ") " << top[i].score << " - " << top[i].username << "\n";
	}

	std::cout << "\n";
}

char Game::ReadCommand(const char* prompt) const
{
	std::cout << prompt;

	// We use getline so ENTER works consistently, and to avoid leftover newlines.
	std::string line;
	std::getline(std::cin, line);

	if (!std::cin) // handle EOF / failure
	{
		std::cin.clear();
		return '\0';
	}

	if (line.empty())
		return '\n';

	return line[0];
}

/**
* Clears
* 
*/
void Game::ClearScreen() const
{
	// Simple + portable-ish: push old content away
	for (int i = 0; i < 40; ++i)
		std::cout << "\n";
}

Position Game::RandomEmptyCell() const
{
	// Very beginner-friendly approach: random attempts then fallback scan.
	std::uniform_int_distribution<int> dx(0, m_grid.Width() - 1);
	std::uniform_int_distribution<int> dy(0, m_grid.Height() - 1);

	for (int tries = 0; tries < 200; ++tries)
	{
		Position p{ dx(m_rng), dy(m_rng) };
		if (!m_snake.Occupies(p))
			return p;
	}

	// Fallback: find first free cell
	for (int y = 0; y < m_grid.Height(); ++y)
	{
		for (int x = 0; x < m_grid.Width(); ++x)
		{
			Position p{ x,y };
			if (!m_snake.Occupies(p))
				return p;
		}
	}

	return Position{ 0,0 }; // should never happen unless grid is full
}

bool Game::TryCommandToDirection(char c, Direction& outDir)
{
	switch (c)
	{
	case 'w': outDir = Direction::Up; return true;
	case 's': outDir = Direction::Down; return true;
	case 'a': outDir = Direction::Left; return true;
	case 'd': outDir = Direction::Right; return true;
	default: return false;
	}
}

