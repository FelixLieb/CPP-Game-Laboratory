#include "ScoreManager.h"
#include <fstream>
#include <algorithm>
#include <functional>

namespace
{
	void Trim(std::string& text)
	{
		const auto first = text.find_first_not_of(" \t\r\n");
		if (first == std::string::npos)
		{
			text.clear();
			return;
		}
		const auto last = text.find_last_not_of(" \t\r\n");
		text = text.substr(first, last - first + 1);
	}

	bool TryParseEntry(const std::string& line, std::string& username, int& score)
	{
		const auto open = line.find('<');
		const auto comma = line.find(',', open == std::string::npos ? 0 : open + 1);
		const auto close = line.find('>', comma == std::string::npos ? 0 : comma + 1);
		if (open == std::string::npos || comma == std::string::npos || close == std::string::npos || open >= comma || comma >= close)
			return false;

		username = line.substr(open + 1, comma - open - 1);
		std::string scoreText = line.substr(comma + 1, close - comma - 1);

		Trim(username);
		Trim(scoreText);

		if (username.empty() || scoreText.empty())
			return false;

		try
		{
			score = std::stoi(scoreText);
		}
		catch (...)
		{
			return false;
		}

		return true;
	}
}

ScoreManager::ScoreManager(std::string highscoreFilename)
	: m_filename(std::move(highscoreFilename))
{
	Load();
}

void ScoreManager::Load()
{
	m_scores.fill(Highscore{});
	m_count = 0;
	m_lastAddedIndex = -1;

	std::ifstream highscoreFile(m_filename);
	if (!highscoreFile.is_open())
		return;
	
	std::string line;
	while (std::getline(highscoreFile, line))
	{
		std::string username;
		int score{};
		if (TryParseEntry(line, username, score))
			AddScore(score, username);
	}

	m_lastAddedIndex = -1;
}

bool ScoreManager::isATopScore(int score) const
{
	// If new score is higher than current last score
	if (m_count < MaxScores)
		return true;

	return score > m_scores[m_count - 1].score;
}

void ScoreManager::AddScore(int score, const std::string& username)
{
	const Highscore entry{ score, username };

	if (m_count == 0)
	{
		m_scores[0] = entry;
		m_count = 1;
		m_lastAddedIndex = 0;
		return;
	}

	if (m_count == MaxScores && !isATopScore(score))
	{
		m_lastAddedIndex = -1;
		return;
	}

	std::size_t insertIndex = 0;
	while (insertIndex < m_count && m_scores[insertIndex].score >= score)
	{
		++insertIndex;
	}

	// At this point we've found where to insert the new score
	if (m_count < MaxScores)
	{
		++m_count;
	}
	// Make room for the new entry
	// We move lower scores down one step in the array
	for (std::size_t i = m_count - 1; i > insertIndex; --i)
	{
		m_scores[i] = m_scores[i - 1];
	}

	m_scores[insertIndex] = entry;
	m_lastAddedIndex = static_cast<int>(insertIndex);
}

void ScoreManager::SaveAppendLastScore() const
{
	if (m_lastAddedIndex < 0) return;

	std::ofstream highscoreFile(m_filename, std::ios::app);
	if (!highscoreFile.is_open())
		return;

	const std::size_t idx = static_cast<std::size_t>(m_lastAddedIndex);
	const Highscore& hs = m_scores[idx];

	highscoreFile << "<" << hs.username << ", " << hs.score << ">\n";
}

std::array<Highscore, ScoreManager::MaxScores> ScoreManager::TopScores() const
{
	return m_scores;
}
