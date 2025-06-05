#include "ChasingTankAlgorithm.h"



// #include <string>
// // להסיררררררררררררררררררררררררררררררררררררררררררררררררררררררררררררררררר!
// std::string toString(ActionRequest action) {
//     switch (action) {
//         case ActionRequest::MoveForward:     return "MoveForward";
//         case ActionRequest::MoveBackward:    return "MoveBackward";
//         case ActionRequest::RotateLeft90:    return "RotateLeft90";
//         case ActionRequest::RotateRight90:   return "RotateRight90";
//         case ActionRequest::RotateLeft45:    return "RotateLeft45";
//         case ActionRequest::RotateRight45:   return "RotateRight45";
//         case ActionRequest::Shoot:           return "Shoot";
//         case ActionRequest::GetBattleInfo:   return "GetBattleInfo";
//         case ActionRequest::DoNothing:       return "DoNothing";
//         default:                             return "Unknown";
//     }
// }

// void printGrid(const std::vector<std::vector<ObjectType>>& grid, int rows, int cols, int tankIndex) {
//     std::cerr << "[Tank " << tankIndex << "] Grid view:\n";
//     for (int x = 0; x < rows; ++x) {
//         for (int y = 0; y < cols; ++y) {
//             char symbol = '?';
//             switch (grid[x][y]) {
//                 case ObjectType::Empty: symbol = '.'; break;
//                 case ObjectType::Wall:  symbol = '#'; break;
//                 case ObjectType::Mine:  symbol = '@'; break;
//                 case ObjectType::AllyTank:  symbol = 'A'; break;
//                 case ObjectType::EnemyTank:  symbol = 'E'; break;
//                 case ObjectType::Shell: symbol = '*'; break;
//                 default: symbol = 'N'; break;
//             }
//             std::cerr << symbol;
//         }
//         std::cerr << "\n";
//     }
//     std::cerr << std::endl;
// }

// void printVisited(const std::vector<std::vector<std::vector<bool>>>& visited, int rows, int cols, int tankIndex) {
//     std::cerr << "[Tank " << tankIndex << "] Visited states:\n";
//     for (int x = 0; x < rows; ++x) {
//         for (int y = 0; y < cols; ++y) {
//             std::cerr << "(" << x << "," << y << "): ";
//             bool found = false;
//             for (int d = 0; d < 8; ++d) {
//                 if (visited[x][y][d]) {
//                     std::cerr << d << " ";
//                     found = true;
//                 }
//             }
//             if (found)
//                 std::cerr << "\n";
//         }
//     }
// }



ChasingTankAlgorithm::ChasingTankAlgorithm(int playerIndex, int tankIndex)
    : MyTankAlgorithm(playerIndex, tankIndex) {}

// TODO: update according to MyBattleInfo
void ChasingTankAlgorithm::updateBattleInfo(BattleInfo& info){
    auto myinfo = static_cast<MyBattleInfo&>(info);
    myPosition = myinfo.getMyPosition();
    lastEnemyPosition = enemyPosition;
    enemyPosition = myinfo.getEnemyPosition();
    shellsPositions = myinfo.getShellsPositions(); 
    updateGrid(info);
}

/**
 * @brief Returns the next action for the tank.
 **/
ActionRequest ChasingTankAlgorithm::getAction() {

    // std::cerr << "[Tank " << tankIndex << "]  My position: (" << myPosition.first << ", " << myPosition.second << ")" << std::endl;
    // std::cerr << "[Tank " << tankIndex << "]  Enemy position: (" << enemyPosition.first << ", " << enemyPosition.second << ")" << std::endl;

    // for first turn \ search for new target - get information about the game
    if(enemyPosition.first == -1 && enemyPosition.second == -1){
        // std::cerr << "[Tank " << tankIndex << "] No enemy known — requesting info." << std::endl;
        turnsSinceLastUpdate = 1;
        return ActionRequest::GetBattleInfo;
    }

    // Try to take a basic safe action first (like moving away from a shell)
    auto threatPlaces = getThreatsAroundMe();
    ActionRequest action = getThreatningNextAction(threatPlaces); // TODO: maybe different name
    if (action != ActionRequest::DoNothing) {
        // std::cerr << "[Tank " << tankIndex << "] Avoiding threat — action: "
        //       << toString(action) << std::endl;
        updatePostAction(action);
        turnsSinceLastUpdate++;
        return action; // If a valid basic move exists, take it immediately
    }

    // Otherwise, prepare to chase the enemy
    // Recalculate path if needed (first time or situation changed)
    if (currentPath.empty() || enemyPosition != lastEnemyPosition || myPosition != plannedPositions.front()) {
        // std::cerr << "[Tank " << tankIndex << "] Recalculating path..." << std::endl;
        auto pathResult = calculatePathBFS();
        currentPath = std::move(pathResult.first);
        plannedPositions = std::move(pathResult.second);
        lastEnemyPosition = enemyPosition;
    }

    // Continue along the current path if it exists
    if (!currentPath.empty()) {
        ActionRequest nextAction = currentPath.front();
        // std::cerr << "[Tank " << tankIndex << "] Following path — next action: "
        //       << toString(nextAction) << std::endl;
        currentPath.erase(currentPath.begin());
        plannedPositions.erase(plannedPositions.begin());
        // std::cerr << "תדפיסי פה " << std::endl;
        updatePostAction(nextAction);
        turnsSinceLastUpdate++;
        return nextAction;
    }

    // No path or reached the end — fallback to default attack
    // std::cerr << "[Tank " << tankIndex << "] No path — defaulting to SHOOT." << std::endl;
    updatePostAction(ActionRequest::Shoot);
    turnsSinceLastUpdate++;
    enemyPosition = {-1, -1};
    // TODO: chack if can shoot
    return ActionRequest::Shoot;
}

/**
 * @brief Computes a path to the enemy using BFS.
 */
std::pair<std::vector<ActionRequest>, std::vector<std::pair<int, int>>> ChasingTankAlgorithm::calculatePathBFS() {

    // printGrid(grid, rows, cols, tankIndex);

    std::queue<State> q;
    std::map<State, State> parent;

    auto visited = std::vector(rows, std::vector(cols, std::vector(8, false)));

    auto [startX, startY] = myPosition;
    State start{startX, startY, myDirection};
    q.push(start);
    visited[start.x][start.y][(int)start.direction] = true;

    while (!q.empty()) {
        State current = q.front();
        q.pop();
        // std::cerr << "[Tank " << tankIndex << "] Visiting (" << current.x << ", " << current.y
        //   << ") facing " << (int)current.direction << std::endl;
        // Check if we reached the enemy's position
        auto [enemyX, enemyY] = enemyPosition;
        if (current.x == enemyX && current.y == enemyY) {
            // Reconstruct path from current to start
            std::vector<ActionRequest> actions;
            std::vector<std::pair<int, int>> positions;
            State node = current;
            while (node != start) {
                State prev = parent[node];
                actions.push_back(inferAction(prev, node));
                positions.push_back({node.x, node.y});
                node = prev;
            }
            positions.push_back({start.x, start.y});
            std::reverse(actions.begin(), actions.end());
            std::reverse(positions.begin(), positions.end());
            // std::cerr << "[Tank " << tankIndex << "] Final planned positions (BFS path):" << std::endl;
            // for (const auto& pos : positions) {
            //     // std::cerr << "  (" << pos.first << "," << pos.second << ")" << std::endl;
            // }
            return {actions, positions};
        }

        // Try moving forward
        auto [nx, ny] = moveInDirectionD(current.x, current.y, 1, current.direction);
        bool isTarget = (nx == enemyPosition.first && ny == enemyPosition.second);
        // std::cerr << "[Tank " << tankIndex << "] Trying to move from (" << current.x << "," << current.y
        //   << ") to (" << nx << "," << ny << ") facing " << (int)current.direction << "... ";
        if ((grid[nx][ny] == ObjectType::Empty || isTarget)  && !visited[nx][ny][(int)current.direction]) {
            // std::cerr << "Success\n";
            visited[nx][ny][(int)current.direction] = true;
            State next{nx, ny, current.direction};
            q.push(next);
            parent[next] = current;
        }
        else{
            // if (nx < 0 || ny < 0 || nx >= cols || ny >= rows)
            //     // std::cerr << "Blocked: out of bounds\n";
            // else if (grid[nx][ny] != ObjectType::Empty)
            //     // std::cerr << "Blocked: not empty (grid = " << (int)grid[nx][ny] << ")\n";
            // else if (visited[nx][ny][(int)current.direction])
            //     // std::cerr << "Blocked: already visited\n";
            // else
            //     std::cerr << "Blocked: unknown reason\n";
            // Try turning directions
            for (auto turnFunc : {&turnLeft, &turnRight,
                                &turnLeftQuarter, &turnRightQuarter}) {
                Direction newDir = (*turnFunc)(current.direction);
                if (!visited[current.x][current.y][(int)newDir]) {
                    // std::cerr << "  Turning from " << (int)current.direction
                    // << " to " << (int)newDir << ", staying at ("
                    // << current.x << ", " << current.y << ")\n";
                    visited[current.x][current.y][(int)newDir] = true;
                    State next{current.x, current.y, newDir};
                    q.push(next);
                    parent[next] = current;
                }
            }
        }
    }
    // No path found
    // std::cerr << "[Tank " << tankIndex << "] No path found to enemy!" << std::endl;
    return {{}, {}};
}

/**
 * @brief Deduces the action needed to go from one state to another.
 */
ActionRequest ChasingTankAlgorithm::inferAction(const State& from, const State& to) {
    if (from.x == to.x && from.y == to.y) {
        // Rotation only
        int diff = (int(to.direction) - int(from.direction) + 8) % 8;
        if (diff == 1) return ActionRequest::RotateRight45;
        if (diff == 2) return ActionRequest::RotateRight90;
        if (diff == 7) return ActionRequest::RotateLeft45;
        if (diff == 6) return ActionRequest::RotateLeft90;
    } else {
        // Position changed — must be a move forward
        return ActionRequest::MoveForward;
    }
    return ActionRequest::DoNothing; // fallback
}
