#pragma once

#include "TankAlgorithm.h"
#include "Direction.h"
#include "ActionRequest.h"
#include "BattleInfo.h"
#include "ObjectType.h"
#include "MyBattleInfo.h"
#include <set>
#include <vector>
#include <utility>

class MyTankAlgorithm : public TankAlgorithm {
protected:
    int playerIndex;
    int tankIndex;
    int rows;
    int cols;
    std::vector<std::vector<ObjectType>> grid;
    std::pair<int,int> myPosition;
    Direction myDirection;
    std::set<std::pair<int, int>> shellsPositions; // positions of shells in the last grid
    int turnsSinceLastUpdate;

    void updatePostAction(ActionRequest action);
    void updateGrid(BattleInfo& info);
    std::pair<int, int> moveInDirectionD(int x, int y, int d, Direction dir);
    
    ActionRequest getThreatningNextAction(std::set<std::pair<int, int>> threatPlaces);
    ActionRequest MoveTankFromDanger(std::set<std::pair<int, int>> threatPlaces);
    ActionRequest CheckIfINeedToShootX(int x);
    std::set<std::pair<int, int>> getThreatsAroundMe();
    std::set<std::pair<int, int>> getThreatCellsAroundMe() const;
    std::set<std::pair<int, int>> getThreatTanksAroundMe() const;
    std::set<std::pair<int, int>> getCurrThreatShells();
    std::vector<std::pair<int, int>> getXDeltas(int x) const;
    std::set<std::pair<int, int>> doDVicinity(int x, int y, int d) const;
    void wrapPosition(int& x, int& y) const;

public:
    MyTankAlgorithm(int playerIndex, int tankIndex);
    virtual ActionRequest getAction() override = 0;
    virtual void updateBattleInfo(BattleInfo& info) override = 0;
    ~MyTankAlgorithm() override = default;
};