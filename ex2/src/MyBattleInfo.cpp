#include "MyBattleInfo.h"
#include <iostream>

MyBattleInfo::MyBattleInfo(const SatelliteView& view, int playerIndex, size_t rows, size_t cols)
    : grid(rows, std::vector<ObjectType>(cols, ObjectType::Empty)) {
    for (size_t x = 0; x < rows; ++x) {
        for (size_t y = 0; y < cols; ++y) {
            char c = view.getObjectAt(x, y);
            ObjectType type = ObjectType::Empty;

            switch (c) {
                case '1':
                    type = (playerIndex == 1) ? ObjectType::AllyTank : ObjectType::EnemyTank;
                    break;
                case '2':
                    type = (playerIndex == 2) ? ObjectType::AllyTank : ObjectType::EnemyTank;
                    break;
                case '*':
                    type = ObjectType::Shell;
                    shellsPositions.insert({static_cast<int>(x), static_cast<int>(y)});
                    break;
                case '#':
                    type = ObjectType::Wall;
                    break;
                case '@':
                    type = ObjectType::Mine;
                    break;
                case '%':
                    myPosition = {static_cast<int>(x), static_cast<int>(y)};
                    type = ObjectType::AllyTank;
                    break;
            }
            grid[x][y] = type;
        }
    }
} 
