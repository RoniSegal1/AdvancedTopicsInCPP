#include "Tank.h"

int Tank::nextSerialNumber = 1;

Tank::Tank(Direction dir, int ammo)
    : direction(dir), ammo(ammo), serialNumber(nextSerialNumber++) {}

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

int Tank::getSerialNumber() const {
    return serialNumber;
}

Shell* Tank::shoot() {
    if (canShoot()) {
        ammo--;
        startShootCooldown();
        return new Shell(direction); // Shell gets its own serial number internally
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

bool Tank::isReadyToMoveBackward() const {
    return backwardDelay == 1;
}

void Tank::tick() {
    if (shootCooldown > 0)
        shootCooldown--;

    if (backwardDelay > 0)
        backwardDelay--;
}
