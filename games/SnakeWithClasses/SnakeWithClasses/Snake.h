#pragma once
#include <deque>
#include "Position.h"
#include "Direction.h"

class Snake
{
public:
	Snake() = default;

	Position Head() const;
	Direction Dir() const { return m_dir; }

	// The direction that wil be applied on next Move()
	Direction NextDir() const { return m_dir; }

	void SetNextDirection(Direction d);

	// Moves snake one step. If grow==true, it keeps the tail
	void Move(bool grow);

	void Reset(Position start, int initialLength, Direction initialDirection = Direction::Right);

	bool Occupies(Position p) const;
	bool HitSelf() const;

	const std::deque<Position>& Body() const { return m_body; }

private:
	Position StepFrom(Position p, Direction d) const;

private:
	std::deque<Position> m_body; // front = head
	Direction m_dir{ Direction::Right };
	Direction m_nextDir{ Direction::Right };

};

