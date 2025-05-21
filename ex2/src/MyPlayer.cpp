#include "MyPlayer.h"
#include "MyBattleInfo.h"
#include <stdexcept>

MyPlayer::MyPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : playerIndex(playerIndex), maxSteps(maxSteps), numShells(numShells) , x(x), y(y) {}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm& algorithm, SatelliteView& satellite_view) {

    std::pair<int, int> selfPosition{-1, -1};
    std::vector<std::pair<int, int>> enemyTanks;
    std::vector<std::pair<int, int>> ownTanks;
    std::vector<std::pair<int, int>> shells;
    std::vector<std::pair<int, int>> walls;
    std::vector<std::pair<int, int>> mines;

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            char obj = satellite_view.getObjectAt(i, j);
            std::pair<int, int> pos = {i, j};

            switch (obj) {
                case '%':
                    selfPosition = pos;
                    ownTanks.push_back(pos);
                    break;
                case '1':
                    ownTanks.push_back(pos);
                    break;
                case '2':
                    enemyTanks.push_back(pos);
                    break;
                case '*':
                    shells.push_back(pos);
                    break;
                case '#':
                    walls.push_back(pos);
                    break;
                case '@':
                    mines.push_back(pos);
                    break;
                default:
                    break;
            }
        }
    }

    if (selfPosition.first == -1)
        throw std::runtime_error("Self position not found in SatelliteView");

    // to change and get information from tankAlgorithm
    Direction dir = Direction::Up; 
    int shellsLeft = numShells; 

    MyBattleInfo info(
        playerIndex,
        selfPosition,
        dir,
        shellsLeft,
        maxSteps,
        0,
        enemyTanks,
        ownTanks,
        shells,
        walls,
        mines
    );

    algorithm.updateBattleInfo(info);
}

/**
 * @brief Returns the player's ID.
 */
int MyPlayer::getId() const {
    return playerIndex;
}

/**
 * @brief Checks if player has any tanks left.
 */
bool MyPlayer::hasAliveTanks() const {
    return numsTanks > 0;
}

/**
 * @brief Returns the player's number of tanks.
 */
int MyPlayer::getNumTanks() const {
    return numTanks;
}

/**
 * @brief Sets the player's number of tanks.
 */
int MyPlayer::setNumTanks(size_t num) {
    numTanks = num;
}

/**
 * @brief "removes" a tank by lowering the num of tanks for a player.
 */
int MyPlayer::removeTank() {
    numTanks = numTanks-1;
}