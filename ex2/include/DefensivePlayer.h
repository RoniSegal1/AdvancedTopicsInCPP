#pragma once
#include "MyPlayer.h"
#include "BasicTankAlgorithm.h"
#include "MyBattleInfo.h"
#include <iostream>

class DefensivePlayer : public MyPlayer {
public:
    DefensivePlayer(int player_index, size_t rows, size_t cols, size_t max_steps, size_t num_shells);

    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;
};