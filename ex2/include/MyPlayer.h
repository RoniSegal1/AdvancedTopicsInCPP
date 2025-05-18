// MyPlayer.h
#pragma once

#include "common/Player.h"
#include "TankAlgorithm.h"
#include "Direction.h"
#include "SatelliteView.h"
#include <vector>
#include <utility>

class MyPlayer : public Player {
private:
    int playerIndex;
    size_t maxSteps;
    size_t numShells;

    struct TankInfo {
        int tankIndex;
        std::pair<int, int> lastKnownPosition;
        Direction direction;
        int remainingShells;
        bool isAlive = true;
    };

    std::vector<TankInfo> tanks;

public:
    MyPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);

    void registerTank(int tankIndex);

    void updateTankInfo(int tankIndex, const std::pair<int, int>& pos,
                        Direction dir, int shells);

    void updateTankWithBattleInfo(TankAlgorithm& tankAlgorithm, SatelliteView& satellite_view) override;
};