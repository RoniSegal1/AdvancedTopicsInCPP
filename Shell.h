#pragma once
#include "Direction.h"
#include "Cell.h"

class Shell {
private:
    Cell* cell;
    Direction direction;

public:
    Shell(Cell* c, Direction dir);

    Direction getDirection() const;
    void setDirection(Direction newDir);

    Cell* getCell() const;
    void setShell(Cell* newCell);
};
