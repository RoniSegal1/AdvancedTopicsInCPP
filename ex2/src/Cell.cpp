#include "Cell.h"
#include "Tank.h"
#include "Shell.h"

/**
 * @brief Initializes a cell at given coordinates, defaulting to empty terrain.
 */
Cell::Cell(int x, int y)
    : x(x), y(y), terrain(TerrainType::Empty), wallHits(-1) {}

/**
 * @brief Returns the (x, y) position of the cell.
 */
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

/**
 * @brief Resets the wall — clears hit count and turns cell into empty terrain.
 */
void Cell::resetWall() {
    wallHits = -1;
    terrain = TerrainType::Empty;
}

/**
 * @brief Resets the mine — sets terrain back to empty.
 */
void Cell::resetMine() {
    terrain = TerrainType::Empty;
}
