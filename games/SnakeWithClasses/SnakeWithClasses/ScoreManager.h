#pragma once
#include <array>
#include <string>

struct Highscore
{
	int score{ 0 };
	std::string username{ "Unknown" };

	bool operator==(const Highscore& other) const
	{
		return score == other.score && username == other.username;
	}

	bool operator>(const Highscore& other) const
	{
		return score > other.score;
	}
};

class ScoreManager
{
public:
	static constexpr std::size_t MaxScores = 10;

	ScoreManager(std::string highscoreFilename);
	void Load();

	void AddScore(int score, const std::string& username);
	void SaveAppendLastScore() const;

	bool isATopScore(int score) const;

	std::array<Highscore, MaxScores> TopScores() const;

	std::size_t Count() const { return m_count; }

private:
	std::string m_filename;
	std::array<Highscore, MaxScores> m_scores{};
	std::size_t m_count{ 0 };
	int m_lastAddedIndex{ -1 };
};

