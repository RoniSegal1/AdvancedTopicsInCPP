#include "ChasingTankAlgorithm.h"

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


    // for first turn \ search for new target - get information about the game
    if(enemyPosition.first == -1 && enemyPosition.second == -1){
        turnsSinceLastUpdate = 1;
        return ActionRequest::GetBattleInfo;
    }

    // Try to take a basic safe action first (like moving away from a shell)
    auto threatPlaces = getThreatsAroundMe();
    ActionRequest action = getThreatningNextAction(threatPlaces); // TODO: maybe different name
    if (action != ActionRequest::DoNothing) {
        updatePostAction(action);
        turnsSinceLastUpdate++;
        return action; // If a valid basic move exists, take it immediately
    }

    // Otherwise, prepare to chase the enemy
    // Recalculate path if needed (first time or situation changed)
    if (currentPath.empty() || enemyPosition != lastEnemyPosition || myPosition != plannedPositions.front()) {
        auto pathResult = calculatePathBFS();
        currentPath = std::move(pathResult.first);
        plannedPositions = std::move(pathResult.second);
        lastEnemyPosition = enemyPosition;
    }

    // Continue along the current path if it exists
    if (!currentPath.empty()) {
        ActionRequest nextAction = currentPath.front();
        currentPath.erase(currentPath.begin());
        plannedPositions.erase(plannedPositions.begin());
        updatePostAction(action);
        turnsSinceLastUpdate++;
        return nextAction;
    }

    // No path or reached the end — fallback to default attack
    enemyPosition = {-1, -1}; // reached the enemy - next turn ask for battleInfo
    updatePostAction(action);
    turnsSinceLastUpdate++;
    // TODO: chack if can shoot
    return ActionRequest::Shoot;
}

/**
 * @brief Computes a path to the enemy using BFS.
 */
std::pair<std::vector<ActionRequest>, std::vector<std::pair<int, int>>> ChasingTankAlgorithm::calculatePathBFS() {

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
            return {actions, positions};
        }

        // Try moving forward
        auto [nx, ny] = moveInDirectionD(current.x, current.y, 1, current.direction);
        if (grid[nx][ny] == ObjectType::Empty && !visited[nx][ny][(int)current.direction]) {
            visited[nx][ny][(int)current.direction] = true;
            State next{nx, ny, current.direction};
            q.push(next);
            parent[next] = current;
        }
        else{
            // Try turning directions
            for (auto turnFunc : {&turnLeft, &turnRight,
                                &turnLeftQuarter, &turnRightQuarter}) {
                Direction newDir = (*turnFunc)(current.direction);
                if (!visited[current.x][current.y][(int)newDir]) {
                    visited[current.x][current.y][(int)newDir] = true;
                    State next{current.x, current.y, newDir};
                    q.push(next);
                    parent[next] = current;
                }
            }
        }
    }
    // No path found
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
