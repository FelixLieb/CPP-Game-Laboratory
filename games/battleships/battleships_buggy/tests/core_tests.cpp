#include "core/Board.h"
#include "core/Ship.h"
#include <cassert>

using namespace bs;

int main()
{
    Board b(10, 10);

    // Basic sanity: placing a horizontal ship should succeed.
    PlaceResult r = b.PlaceShip(Ship{ 3, Coord{1,1}, Orientation::Horizontal });
    assert(r == PlaceResult::Ok);

    // Bug showcase: shooting same cell twice should NOT erase hit, but buggy code does.
    ShotResult first = b.Shoot(Coord{ 1,1 });
    assert(first == ShotResult::Hit || first == ShotResult::Sunk);

    ShotResult second = b.Shoot(Coord{ 1,1 });
    // In a correct implementation we'd expect AlreadyTried.
    // In the buggy version, it's Miss (and it can even overwrite a hit).
    assert(second == ShotResult::Miss);

    return 0;
}