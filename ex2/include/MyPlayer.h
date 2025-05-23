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
    size_t numTanks = 0;
    size_t maxSteps;
    size_t numShells;
    
public:
    MyPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells);

    void updateTankWithBattleInfo(TankAlgorithm& tankAlgorithm, SatelliteView& satellite_view) override;

    int getId() const;

    bool hasAliveTanks() const;

    void setNumTanks(size_t num);

    void removeTank();

    size_t getNumTanks() const; 
};