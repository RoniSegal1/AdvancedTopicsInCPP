#pragma once
#include "Direction.h"
#include "Shell.h"

class Tank {
private:
    Direction direction;
    int ammo;
    int serialNumber;

    int shootCooldown = 0;  // counts down after each shot
    int backwardDelay = 0;  // 0 = not waiting, >0 = waiting for backward move

    static int nextSerialNumber;

public:
    Tank(Direction dir, int ammo = 16);
    ~Tank();

    Direction getDirection() const;
    void setDirection(Direction dir);

    int getAmmo() const;
    int getSerialNumber() const;

    Shell* shoot();

    void startShootCooldown();            // set 4-step cooldown
    bool canShoot() const;                // check if shooting is allowed

    void startBackwardDelay();            // begin delay for backward movement
    bool isReadyToMoveBackward() const;   // ready to move on next tick

    void tick();                          // update cooldowns and delays per game step
};
