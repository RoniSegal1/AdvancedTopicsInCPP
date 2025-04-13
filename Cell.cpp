#include "Cell.h"
#include "Tank.h"
#include "Shell.h"

Cell::Cell(int x, int y)
    : x(x), y(y), terrain(TerrainType::Empty), wallHits(-1), tanks(), shells() {}

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

void Cell::addTank(Tank* tank) {
    tanks.insert(tank);
}

void Cell::removeTank(Tank* tank) {
    tanks.erase(tank);
}

const std::set<Tank*>& Cell::getTanks() const {
    return tanks;
}

void Cell::addShell(Shell* shell) {
    shells.insert(shell);
}

void removeShell(Shell* shell) {
    shells.erase(shell);
}

std::set<Shell*> Cell::getShells() const {
    return shells;
}

void Cell::clearShells() {
    shells.clear();
}