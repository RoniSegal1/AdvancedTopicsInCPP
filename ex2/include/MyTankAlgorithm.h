#pragma once

#include "TankAlgorithm.h"
#include "Board.h"
#include "PlayerCell.h"
#include "Direction.h"
#include <set>
#include <vector>
#include <utility>
#include <optional>

class MyTankAlgorithm : public TankAlgorithm {
protected:
    Board<PlayerCell> board = Board<PlayerCell>(0, 0);
    std::pair<int,int> myPosition = {-1, -1};
    Direction myDirection = Direction::U;
    //Direction myDirection = (playerIndex == 1) ? Direction::L : Direction::R;
    std::set<std::pair<int, int>> shellsPositions; // positions of shells in the last board
    int turnsSinceLastUpdate;

    void updatePostAction(ActionRequest action);
    void updateBoard(BattleInfo& info);
    std::pair<int, int> moveInDirectionD(int x, int y, int d, Direction dir);
    const std::vector<std::pair<int, int>> getXDeltas(int x);
    std::set<std::pair<int, int>> doDVicinity(int x, int y, int d, const Board& board);
    ActionRequest getThreatningNextAction(const std::set<std::pair<int, int>>& threatingShells) const;
    bool canMoveThere(int sx, int sy) const;
    std::set<std::pair<int, int>> getCurrThreatShells();
    bool hasBoard() const { return lastBoard.has_value(); }

public:
    MyTankAlgorithm(int playerIndex, int tankIndex);
    virtual ActionRequest getAction() = 0;
	virtual void updateBattleInfo(BattleInfo& info) = 0;
};