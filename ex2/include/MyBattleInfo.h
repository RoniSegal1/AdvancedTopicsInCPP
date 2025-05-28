#pragma once

#include "BattleInfo.h"
#include "Board.h"
#include "ObjectType.h"
#include <utility>
#include <set>
#include <memory>

class MyBattleInfo : public BattleInfo {
private:
    std::vector<std::vector<ObjectType>> grid;
    std::pair<int, int> myPosition;
    std::pair<int, int> enemyPosition;
    std::set<std::pair<int, int>> shellsPositions;

public:
    MyBattleInfo(std::vector<std::vector<ObjectType>> grid,
                 std::pair<int, int> myPos,
                 std::pair<int, int> enemyPos,
                 const std::set<std::pair<int, int>>& shells);

    std::vector<std::vector<ObjectType>> getGrid() const;
    std::pair<int, int> getMyPosition() const;
    std::pair<int, int> getEnemyPosition() const;
    const std::set<std::pair<int, int>>& getShellsPositions() const;
};
