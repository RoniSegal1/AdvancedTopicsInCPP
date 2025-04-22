#include "Shell.h"
#include <utility>

Shell::Shell(int x, int y, Direction dir)
    : x(x), y(y), direction(dir) {}

Direction Shell::getDirection() const {
    return direction;
}

std::pair<int, int> Shell::getPosition() const {
    return {x,y};
}

void Shell::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

std::pair<int, int> Shell::getNextShellPosition() const {
    int dx = 0, dy = 0;

    switch (direction) {
        case Direction::U:  dy = -2; break;
        case Direction::UR: dy = -2; dx = +2; break;
        case Direction::R:  dx = +2; break;
        case Direction::DR: dy = +2; dx = +2; break;
        case Direction::D:  dy = +2; break;
        case Direction::DL: dy = +2; dx = -2; break;
        case Direction::L:  dx = -2; break;
        case Direction::UL: dy = -2; dx = -2; break;
    }

    return {x + dx, y + dy};
}
