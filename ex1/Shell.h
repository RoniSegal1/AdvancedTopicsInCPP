#pragma once
#include "Direction.h"
#include "Entity.h"

class Shell : public Entity {
private:
    int x;
    int y;
    Direction direction;

public:
    Shell(int x, int y, Direction dir);

    Direction getDirection() const;
    std::pair<int,int> getPosition() const override;
    void setPosition(int newX, int newY);
    
    std::pair<int, int> getNextShellPosition() const;

};
