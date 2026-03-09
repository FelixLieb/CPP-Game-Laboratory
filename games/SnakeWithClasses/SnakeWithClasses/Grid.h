#pragma once
#include <vector>
#include "Position.h"
#include <string>

class Grid
{
public:
	Grid(int width, int height, char emptyChar = '.');

	int Width() const { return m_width; }
	int Height() const { return m_height; }

	bool InBounds(Position p) const;
	int ToIndex(Position p) const;

	void Clear(); // grid.Clear();
	void SetCell(Position p, char c);
	char GetCell(Position p) const;

	// Render to a string
	std::string ToString() const; // std::cout << grid.ToString


private:
	int m_width{ 10 };
	int m_height{ 10 };
	char m_empty{ '.' };
	std::vector<char> m_cells;
};

