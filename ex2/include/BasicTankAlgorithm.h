#pragma once

#include "MyTankAlgorithm.h"
#include "Board.h"
#include "Direction.h"
#include "PlayerCell.h"
#include <optional>

class BasicTankAlgorithm : public MyTankAlgorithm {
private:
    int turnsSinceLastUpdate;
    std::optional<Board<PlayerCell>> lastBoard; // change the name of the cell type

    // אם יש טנק לירות
    // להעביר את THRET למיי אלגוריתם
    ActionRequest getThreatningNextAction();
    ActionRequest getScaryNextAction();

    ActionRequest shouldGetBattleInfo() const;
    bool detectThreateningShell();
    bool hasBoard() const { return lastBoard.has_value(); }

public:
    BasicTankAlgorithm(int playerIndex, int tankIndex);

    void updateBattleInfo(BattleInfo& info) override; // not has to be implemented in this class, can be in parent class
    ActionRequest getAction() override;
};
