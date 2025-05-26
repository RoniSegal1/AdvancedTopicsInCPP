#pragma once

#include "ActionRequest.h"
#include "BattleInfo.h"
#include "TankAlgorithm.h"
#include "PlayerCell.h"
#include "Direction.h"

class MyTankAlgorithm : public TankAlgorithm {
protected:
    Board<PlayerCell> board = new Board(0,0);
    std::pair<int,int> myPosition = {-1, -1};
    Direction myDirection = Direction::U;
    void updatePostAction(ActionRequest action);
    void updateBoard(Board<PlayerCell> board, BattleInfo& info);

public:
    MyTankAlgorithm(int playerIndex, int tankIndex);
    virtual ActionRequest getAction() = 0;
	virtual void updateBattleInfo(BattleInfo& info) = 0;
};