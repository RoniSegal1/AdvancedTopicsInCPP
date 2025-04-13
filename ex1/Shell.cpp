#include "Shell.h"

Shell::Shell(Cell* c, Direction dir)
    : cell(c), direction(dir) {}

Direction Shell::getDirection() const {
    return direction;
}

void Shell::setDirection(Direction newDir) {
    direction = newDir;
}

Cell* Shell::getCell() const {
    return cell;
}

void Shell::setShell(Cell* newCell) {
    cell = newCell;
}
