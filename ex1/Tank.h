#pragma once
#include "Direction.h"
#include "Shell.h"
#include "Entity.h"
#include <set>
#include <utility>

class Tank : public Entity {
private:
    Direction direction;
    int x;
    int y;
    int ammo;
    int player;

    int shootCooldown = 0;  // counts down after each shot
    int backwardDelay = 0;  // 0 = not waiting, >0 = waiting for backward move

public:
    Tank(Direction dir, int x, int y, int player, int ammo = 16);

    Direction getDirection() const;
    void setDirection(Direction dir);

    std::pair<int, int> getPosition() const override;
    void setPosition(int newX, int newY);

    std::pair<int, int> Tank::getNextForwardPosition() const;
    void rotate(int place);

    int Tank::getPlayer() const;

    int getAmmo() const;
    int getSerialNumber() const;

    Shell* shoot();

    void startShootCooldown();            // set 4-step cooldown
    bool canShoot() const;                // check if shooting is allowed

    void startBackwardDelay();            // begin delay for backward movement
    bool notInBackwardDelay() const;      // ready to move on next tick

    void tick();                          // update cooldowns and delays per game step
};
