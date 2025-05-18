#pragma once

#include "common/Player.h"
#include "TankAlgorithm.h"
#include "Tank.h"
#include <memory>
#include <vector>
#include <unordered_map>

class MyPlayer : public Player {
private:
    int playerIndex;
    size_t maxSteps;
    size_t numShells;

    struct TankInfo {
        int tankIndex;
        int currShells;
        Tank* tank; // not owned -> just a reference
        std::unique_ptr<TankAlgorithm> algorithm;
    };

    std::vector<TankInfo> tanks;

public:
    MyPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);

    void addTank(int tankIndex, Tank* tank, std::unique_ptr<TankAlgorithm> algorithm);

    void updateTankWithBattleInfo(TankAlgorithm& tankAlgorithm, SatelliteView& satellite_view) override;

    TankAlgorithm* getTankAlgorithm(Tank* tank) const;
};