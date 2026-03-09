#pragma once

struct Position
{
	int x{ 0 };
	int y{ 0 };
};

inline bool operator==(const Position& a, const Position& b)
{
	return a.x == b.x && a.y == b.y; // True AND/&& True = True
	// True AND False = False
	// False AND False = False
}