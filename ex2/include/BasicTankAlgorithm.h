#pragma once

#include "MyTankAlgorithm.h"
#include "BattleInfo.h"
#include "MyBattleInfo.h"
#include "ObjectType.h"
#include "Direction.h"

class BasicTankAlgorithm : public MyTankAlgorithm {
private:
    ActionRequest getScaryNextAction(std::set<std::pair<int, int>> threatPlaces);
    ActionRequest rotateToFreeCell(std::set<std::pair<int, int>> threatPlaces);
    ActionRequest rotateToShootTank(std::set<std::pair<int, int>> threatTanks);
    ActionRequest shouldGetBattleInfo();
    bool isEmpty() const;

public:
    BasicTankAlgorithm(int playerIndex, int tankIndex);

    void updateBattleInfo(BattleInfo& info) override; // not has to be implemented in this class, can be in parent class
    ActionRequest getAction() override;
};
