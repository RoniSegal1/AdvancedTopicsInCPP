#include "Player.h"

/**
 * @brief Default constructor.
 */
Player::Player() {}

/**
 * @brief Constructor initializing player ID.
 */
Player::Player(int id) : playerId(id) {}

/**
 * @brief Returns the player's ID.
 */
int Player::getId() const {
    return playerId;
}

/**
 * @brief Adds a tank to the player.
 */
void Player::addTank(Tank* tank) {
    tanks.insert(tank);
}

/**
 * @brief Removes a tank from the player.
 */
void Player::removeTank(Tank* tank) {
    tanks.erase(tank);
}

/**
 * @brief Returns all tanks belonging to the player.
 */
const std::set<Tank*>& Player::getTanks() const {
    return tanks;
}

/**
 * @brief Checks if player has any tanks left.
 */
bool Player::hasAliveTanks() const {
    return !tanks.empty();
}

/**
 * @brief Clears all tanks associated with this player.
 */
void Player::clearTanks() {
    tanks.clear();
}




