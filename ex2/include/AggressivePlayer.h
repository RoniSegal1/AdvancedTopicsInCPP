#pragma once
#include "MyPlayer.h"
#include "MyBattleInfo.h"
#include "SatelliteView.h"
#include <utility>

class AggressivePlayer : public MyPlayer {
public:
    AggressivePlayer(int player_index, size_t rows, size_t cols, size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;

private:
    std::pair<int, int> sharedEnemyLocation = {-1, -1};
    std::pair<int, int> findEnemy(const SatelliteView& view) const;
};