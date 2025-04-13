#include "Tank.h"

Tank::Tank(Cell* startingCell, Direction dir, int ammo=16)
    : cell(startingCell), direction(dir), ammo(ammo) {}

Tank::~Tank() {}

Direction Tank::getDirection() const {
    return direction;
}

void Tank::setDirection(Direction dir) {
    direction = dir;
}

int Tank::getAmmo() const {
    return ammo;
}

bool Tank::shoot() {
    if (ammo > 0){
        ammo--;
        return true;
    }
    return false;
}

Cell* Tank::getCell() const {
    return cell;
}

void Tank::setCell(Cell* newCell) {
    cell = newCell;
}