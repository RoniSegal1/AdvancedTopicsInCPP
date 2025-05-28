#include "MyBattleInfo.h"

MyBattleInfo::MyBattleInfo(std::vector<std::vector<ObjectType>> grid,
                           std::pair<int, int> myPos,
                           std::pair<int, int> enemyPos,
                           const std::set<std::pair<int, int>>& shells)
    : grid(grid), myPosition(myPos), enemyPosition(enemyPos), shellsPositions(shells) {}

std::vector<std::vector<ObjectType>> MyBattleInfo::getGrid() const {
    return grid;
}

std::pair<int, int> MyBattleInfo::getMyPosition() const {
    return myPosition;
}

std::pair<int, int> MyBattleInfo::getEnemyPosition() const {
    return enemyPosition;
}

const std::set<std::pair<int, int>>& MyBattleInfo::getShellsPositions() const {
    return shellsPositions;
}
