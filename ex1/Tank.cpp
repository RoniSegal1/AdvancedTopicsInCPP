#include "Tank.h"

Tank::Tank(Direction dir, int x, int y, int player, int ammo)
    : direction(dir), ammo(ammo), player(player), x(x), y(y) {}

Tank::~Tank() {}

Direction Tank::getDirection() const {
    return direction;
}

void Tank::setDirection(Direction dir) {
    direction = dir;
}

std::pair<int, int> Tank::getPosition() const {
    return {x, y};
}

int Tank::getAmmo() const {
    return ammo;
}

int Tank::getPlayer() const {
    return player;
}

void Tank::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

std::pair<int, int> Tank::getNextForwardPosition() const {
    int dx = 0, dy = 0;

    switch (direction) {
        case Direction::U:  dy = -1; break;
        case Direction::UR: dy = -1; dx = +1; break;
        case Direction::R:  dx = +1; break;
        case Direction::DR: dy = +1; dx = +1; break;
        case Direction::D:  dy = +1; break;
        case Direction::DL: dy = +1; dx = -1; break;
        case Direction::L:  dx = -1; break;
        case Direction::UL: dy = -1; dx = -1; break;
    }

    return {x + dx, y + dy};
}

void Tank::rotate(int place){
    int dirValue = (static_cast<int>(direction) + place) % 8;
    if (dirValue < 0) dirValue += 8;
    setDirection(static_cast<Direction>(dirValue));
}

Shell* Tank::shoot() {
    if (canShoot()) {
        ammo--;
        startShootCooldown();
        return new Shell(x, y, direction); // Shell gets its own serial number internally
    }
    return nullptr;
}

void Tank::startShootCooldown() {
    shootCooldown = 4;
}

bool Tank::canShoot() const {
    return shootCooldown == 0 && ammo > 0;
}

void Tank::startBackwardDelay() {
    if (backwardDelay == 0)
        backwardDelay = 2;
}

bool Tank::notInBackwardDelay() const {
    return backwardDelay == 0;
}

void Tank::tick() {
    if (shootCooldown > 0)
        shootCooldown--;

    if (backwardDelay > 0)
        backwardDelay--;
}
