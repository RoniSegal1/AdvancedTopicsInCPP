#include "MyPlayer.h"

MyPlayer::MyPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : playerIndex(playerIndex), maxSteps(maxSteps), numShells(numShells) , x(x), y(y) {}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm& algorithm, SatelliteView& satellite_view) {

    std::pair<int, int> selfPosition{-1, -1};
    std::pair<int, int> enemyPosition{-1, -1};
    std::set<std::pair<int, int>> shells;
    std::vector<std::vector<ObjectType>> grid;

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            char obj = satellite_view.getObjectAt(i, j);
            std::pair<int, int> pos = {i, j};
            switch (obj) {
                case '%':
                    selfPosition = pos;
                    grid[y][x] = ObjectType::AllyTank;
                    break;
                case '1':
                    grid[y][x] = ObjectType::AllyTank;
                    break;
                case '2':
                    grid[y][x] = ObjectType::EnemyTank;
                    break;
                case '*':
                    grid[y][x] = ObjectType::Shell;
                    shells.insert(pos);
                    break;
                case '#':
                    grid[y][x] = ObjectType::Wall;
                    break;
                case '@':
                    grid[y][x] = ObjectType::Mine;
                    break;
                default:
                    grid[y][x] = ObjectType::Empty;
                    break;
            }
        }
    }

    MyBattleInfo info(
        grid,
        selfPosition,
        enemyPosition,
        shells,
    );

    algorithm.updateBattleInfo(info);
}

/**
 * @brief Returns the player's ID.
 */
int MyPlayer::getId() const {
    return playerIndex;
}

// std::pair<int, int> MyPlayer::findEnemyTank(std::vector<std::vector<Objects>> grid, int myPlayerIndex) {
//     // TODO : YUVAL
// }
