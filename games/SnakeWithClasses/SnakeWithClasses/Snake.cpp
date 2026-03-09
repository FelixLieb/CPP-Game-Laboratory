#include "Snake.h"

void Snake::Reset(Position startPos, int initialLength, Direction startDir)
{
	if (initialLength < 1) initialLength = 1;

	m_body.clear();
	m_dir = startDir;
	m_nextDir = startDir;

	// Build initial body trailing behind the head
	// Example: dir Right -> body extends left
	Position p = startPos;
	m_body.push_front(p);

    for (int i = 1; i < initialLength; ++i)
    {
        switch (startDir)
        {
        case Direction::Right: p.x -= 1; break;
        case Direction::Left:  p.x += 1; break;
        case Direction::Up:    p.y += 1; break;
        case Direction::Down:  p.y -= 1; break;
        }
        m_body.push_back(p);
    }
}

Position Snake::Head() const
{
    return m_body.empty() ? Position{} : m_body.front();
}

void Snake::SetNextDirection(Direction d)
{
    // No 180-degree reversal
    const bool reversing =
        (m_dir == Direction::Up && d == Direction::Down) ||
        (m_dir == Direction::Down && d == Direction::Up) ||
        (m_dir == Direction::Left && d == Direction::Right) ||
        (m_dir == Direction::Right && d == Direction::Left);

    if (!reversing)
        m_nextDir = d;
}

Position Snake::StepFrom(Position p, Direction d) const
{
    switch (d)
    {
    case Direction::Up:     p.y -= 1; break;
    case Direction::Down:  p.y += 1; break;
    case Direction::Left:  p.x -= 1; break;
    case Direction::Right: p.x += 1; break;
    }
    return p;
}

void Snake::Move(bool grow)
{
    m_dir = m_nextDir;

    const Position newHead = StepFrom(Head(), m_dir);
    m_body.push_front(newHead);

    if (!grow)
    {
        m_body.pop_back();
    }
}

bool Snake::Occupies(Position p) const
{
    for (const Position& bodyPart : m_body)
        if (bodyPart == p) return true;
    return false;
}

bool Snake::HitSelf() const
{
    if (m_body.size() < 4) return false;

    const Position h = Head();
    bool first = true;
    for (const Position& bodyPart : m_body)
    {
        if (first) { first = false; continue; }
        if (bodyPart == h) return true;
    }
    return false;
}