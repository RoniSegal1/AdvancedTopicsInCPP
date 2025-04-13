#pragma once
#include "Cell.h"
#include "Direction.h"

class Tank {
private:
    Cell* cell;
    Direction direction;
    int ammo;

public:
    Tank(Cell* startingCell, Direction dir, int ammo = 16);

    Direction getDirection() const;
    void setDirection(Direction dir);

    int getAmmo() const;
    bool shoot();

    Cell* getCell() const;
    void setCell(Cell* newCell);
};
