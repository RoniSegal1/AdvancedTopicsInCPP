#include "Player.h"

Player::Player() {}

Player::Player(int id) : playerId(id) {}

int Player::getId() const {
    return playerId;
}

void Player::addTank(Tank* tank) {
    tanks.insert(tank);
}

void Player::removeTank(Tank* tank) {
    tanks.erase(tank);
}

const std::set<Tank*>& Player::getTanks() const {
    return tanks;
}

bool Player::hasAliveTanks() const {
    return !tanks.empty();
}
