#pragma once
#include "BattleInfo.h"
#include "SatelliteView.h"
#include "ObjectType.h"
#include <vector>
#include <set>
#include <utility>

class MyBattleInfo : public BattleInfo {
private:
    std::vector<std::vector<ObjectType>> grid;
    std::pair<int, int> myPosition = {-1, -1};
    std::pair<int, int> enemyPosition = {-1, -1};
    std::set<std::pair<int, int>> shellsPositions;

public:
    MyBattleInfo(const SatelliteView& view, int playerIndex, size_t rows, size_t cols);
    ~MyBattleInfo() override = default;

    std::pair<int, int> getMyPosition() const { return myPosition; }
    std::pair<int, int> getEnemyPosition() const { return enemyPosition; }
    std::set<std::pair<int, int>> getShellsPositions() const { return shellsPositions; }
    std::vector<std::vector<ObjectType>> getGrid() const { return grid; }

    void setEnemyPosition(const std::pair<int, int>& pos) { enemyPosition = pos; }
};