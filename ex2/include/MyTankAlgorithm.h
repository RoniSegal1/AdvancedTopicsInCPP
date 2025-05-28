#pragma once

#include "TankAlgorithm.h"
#include "Board.h"
#include "PlayerCell.h"
#include "Direction.h"
#include "ActionRequest.h"
#include "BattleInfo.h"
#include "MyBattleInfo.h"
#include <set>
#include <vector>
#include <utility>

class MyTankAlgorithm : public TankAlgorithm {
protected:
    int playerIndex;
    int tankIndex;
    int width;
    int height;
    std::vector<std::vector<ObjectType>> grid;
    std::pair<int,int> myPosition;
    Direction myDirection;
    std::set<std::pair<int, int>> shellsPositions; // positions of shells in the last board
    int turnsSinceLastUpdate;

    void updatePostAction(ActionRequest action);
    void updateBoard(BattleInfo& info);
    std::pair<int, int> moveInDirectionD(int x, int y, int d, Direction dir);
    
    ActionRequest getThreatningNextAction(std::set<std::pair<int, int>> threatPlaces);
    ActionRequest MoveTankFromDanger(std::set<std::pair<int, int>> threatPlaces);
    ActionRequest CheckIfINeedToShootX(int x);
    std::set<std::pair<int, int>> MyTankAlgorithm::getThreatsAroundMe();
    std::set<std::pair<int, int>> MyTankAlgorithm::getThreatCellsAroundMe() const;
    std::set<std::pair<int, int>> MyTankAlgorithm::getCurrThreatShells();
    std::vector<std::pair<int, int>> MyTankAlgorithm::getXDeltas(int x) const;
    std::set<std::pair<int, int>> MyTankAlgorithm::doDVicinity(int x, int y, int d) const;

public:
    MyTankAlgorithm(int playerIndex, int tankIndex);
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};