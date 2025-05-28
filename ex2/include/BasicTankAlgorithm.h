#pragma once

#include "MyTankAlgorithm.h"
#include "Board.h"
#include "Direction.h"
#include "PlayerCell.h"

class BasicTankAlgorithm : public MyTankAlgorithm {
private:
    ActionRequest getScaryNextAction(std::set<std::pair<int, int>> threatPlaces);

    ActionRequest BasicTankAlgorithm::rotateToFreeCell(std::set<std::pair<int, int>> threatPlaces);
    ActionRequest shouldGetBattleInfo();
    bool BasicTankAlgorithm::isEmpty() const;

public:
    BasicTankAlgorithm(int playerIndex, int tankIndex);

    void updateBattleInfo(BattleInfo& info) override; // not has to be implemented in this class, can be in parent class
    ActionRequest getAction() override;
};
