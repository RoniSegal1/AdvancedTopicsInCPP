#include "Entity.h"
#include <iostream>

/**
 * @brief Gets the current direction of the entity.
 */
Direction Entity::getDirection() const {
    return direction;
}

/**
 * @brief Returns the current position (x, y) of the entity.
 */
std::pair<int, int> Entity::getPosition() const {
    return {x, y};
}

/**
 * @brief Sets the position of the entity.
 */
void Entity::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

/**
 * @brief Computes the next position of the entity in its current direction, with wrapping.
 */
std::pair<int, int> Entity::getNextPosition(const Board& board) const {
    auto [dx, dy] = getDelta(direction);
    std::cout << "Entity " << getType() << " moving from (" << x << ", " << y << ") in direction " << dx << dy << "\n";
    int nx = x + dx;
    int ny = y + dy;
    std::cout << "Next position before wrapping: (" << nx << ", " << ny << ")\n";
    board.wrapPosition(nx, ny);
    std::cout << "Next position after wrapping: (" << nx << ", " << ny << ")\n";
    return {nx, ny};
}

/**
 * @brief Calculates the previous position of the entity in its current direction, with wrapping.
 */
 std::pair<int, int> Entity::getPrevPosition(const Board& board) const {
    auto [dx, dy] = getDelta(direction);
    int nx = x - dx;
    int ny = y - dy;
    board.wrapPosition(nx, ny);
    return {nx, ny};
}
