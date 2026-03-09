#include "Grid.h"
#include <iostream>

Grid::Grid(int width, int height, char emptyChar)
	: m_width(width), m_height(height), m_empty(emptyChar)
{
	if (m_width <= 0) m_width = 10;
	if (m_height <= 0) m_height = 10;

	m_cells.assign(static_cast<size_t>(m_width * m_height), m_empty);

	std::cout << "Grid initialized." << std::endl;
};

bool Grid::InBounds(Position p) const
{
	return p.x >= 0 && p.x < m_width && p.y >= 0 && p.y < m_height;
}

int Grid::ToIndex(Position p) const
{
	// Caller should ensure InBounds(p) == true
	return p.y * m_width + p.x;
}

void Grid::Clear()
{
	std::fill(m_cells.begin(), m_cells.end(), m_empty);
}

void Grid::SetCell(Position p, char c)
{
	if (!InBounds(p)) return;
	m_cells[static_cast<size_t>(ToIndex(p))] = c;
}

char Grid::GetCell(Position p) const
{
	if (!InBounds(p)) return m_empty;
	return m_cells[static_cast<size_t>(ToIndex(p))];
}

std::string Grid::ToString() const
{
	std::string s;
	s.reserve(static_cast<size_t>((m_width * 2 + 1) * m_height + 64));

	s += "\n";
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			const char c = m_cells[static_cast<size_t>(y * m_width + x)];
			s += c;
			s += ' ';
		}
		s += '\n';
	}
	return s;
}

