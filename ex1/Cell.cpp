#include "Cell.h"
#include "Tank.h"
#include "Shell.h"

Cell::Cell(int x, int y)
    : x(x), y(y), terrain(TerrainType::Empty), wallHits(-1) {}

std::pair<int, int> Cell::getPoint() const {
    return {x, y};
}

TerrainType Cell::getTerrain() const {
    return terrain;
}

void Cell::setTerrain(TerrainType t) {
    terrain = t;
}

int Cell::getWallHits() const {
    return wallHits;
}

void Cell::incrementWallHits() {
    wallHits++;
}

void Cell::resetWall() {
    wallHits = -1;
    terrain = TerrainType::Empty;
}
