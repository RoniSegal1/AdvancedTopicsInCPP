#pragma once

#include "MyTankAlgorithm.h"
//#include "BasicTankAlgorithm.h"
#include "Board.h"
#include "Tank.h"
#include "ActionRequest.h"
#include "PlayerCell.h"
#include "BattleInfo.h"
#include <queue>
#include <vector>
#include <tuple>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <iostream>


class ChasingTankAlgorithm : public MyTankAlgorithm {
public:
    virtual ~ChasingTankAlgorithm() noexcept override = default;

    ActionRequest getAction();

    void updateBattleInfo(BattleInfo& info);

private:
    std::vector<ActionRequest> currentPath; ///< Cached path of actions to perform.
    std::vector<std::pair<int, int>> plannedPositions; ///< Planned positions associated with currentPath.
    std::pair<int, int> myPosition = { -1, -1 }; ///< My last known position.
    Direction myDirection = Direction::U; ///< My last known direction.
    std::pair<int, int> enemyPosition = { -1, -1 }; ///< Current known position of the enemy.
    std::pair<int, int> lastEnemyPosition = { -1, -1 }; ///< Last known position of the enemy.
    Board<PlayerCell> board;

    struct State {
        int x, y;
        Direction direction;

        State() : x(0), y(0), direction(Direction::U) {}
        State(int x, int y, Direction dir)
            : x(x), y(y), direction(dir) {}

        bool operator<(const State& other) const {
            return std::tie(x, y, direction) < std::tie(other.x, other.y, other.direction);
        }

        bool operator==(const State& other) const {
            return x == other.x && y == other.y && direction == other.direction;
        }

        bool operator!=(const State& other) const {
        return !(*this == other);
        }
    };

    ActionRequest tryBasicMoves();

    std::pair<std::vector<ActionRequest>, std::vector<std::pair<int, int>>> calculatePathBFS();
                                                                                    
    std::pair<int, int> moveInDirection(int x, int y, Direction dir);

    ActionRequest inferAction(const State& from, const State& to);
};
