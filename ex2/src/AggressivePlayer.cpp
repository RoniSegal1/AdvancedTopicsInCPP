#include "AggressivePlayer.h"
#include <cstddef>
#include <iostream>

AggressivePlayer::AggressivePlayer(int player_index, size_t rows, size_t cols, size_t max_steps, size_t num_shells)
    : MyPlayer(player_index, rows, cols, max_steps, num_shells)
{}


void AggressivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    MyBattleInfo info(satellite_view, playerIndex, rows, cols);

    auto newEnemy = findEnemy(satellite_view);
    if (newEnemy != std::make_pair(-1, -1)) {
        sharedEnemyLocation = newEnemy;
    }

    info.setEnemyPosition(sharedEnemyLocation);
    tank.updateBattleInfo(info);
}

std::pair<int, int> AggressivePlayer::findEnemy(const SatelliteView& view) const {
    for (size_t x = 0; x < rows; ++x) {
        for (size_t y = 0; y < cols; ++y) {
            char c = view.getObjectAt(x, y);
            if ((playerIndex == 1 && c == '2') || (playerIndex == 2 && c == '1')) {
                return {static_cast<int>(x), static_cast<int>(y)};
            }
        }
    }
    return {-1, -1};
}