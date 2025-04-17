#pragma once
#include "Direction.h"

class Shell {
private:
    Direction direction;
    int serialNumber;

    static int nextSerialNumber;

public:
    Shell(Direction dir);

    Direction getDirection() const;
    void setDirection(Direction newDir);

    int getSerialNumber() const;
};
