#pragma once

#include "MyTankAlgorithm.h"
#include "Board.h"
#include "Direction.h"
#include "PlayerCell.h"
#include <optional>

class BasicTankAlgorithm : public MyTankAlgorithm {
private:
    ActionRequest getScaryNextAction();

    std::vector<std::pair<int, int>> getCurrThreatShells();
    ActionRequest shouldGetBattleInfo() const;
    bool detectThreateningShell();
    

public:
    BasicTankAlgorithm(int playerIndex, int tankIndex);

    void updateBattleInfo(BattleInfo& info) override; // not has to be implemented in this class, can be in parent class
    ActionRequest getAction() override;
};
