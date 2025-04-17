#include "Shell.h"

int Shell::nextSerialNumber = 1;

Shell::Shell(Direction dir)
    : direction(dir), serialNumber(nextSerialNumber++) {}

Direction Shell::getDirection() const {
    return direction;
}

void Shell::setDirection(Direction newDir) {
    direction = newDir;
}

int Shell::getSerialNumber() const {
    return serialNumber;
}
