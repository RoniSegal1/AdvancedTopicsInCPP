#include "DefensivePlayer.h"

#include "DefensivePlayer.h"

DefensivePlayer::DefensivePlayer(int player_index, size_t rows, size_t cols, size_t max_steps, size_t num_shells)
    : MyPlayer(player_index, rows, cols, max_steps, num_shells)
{}

void DefensivePlayer::updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) {
    MyBattleInfo info(satellite_view, playerIndex, rows, cols);
    tank.updateBattleInfo(info);
} 
