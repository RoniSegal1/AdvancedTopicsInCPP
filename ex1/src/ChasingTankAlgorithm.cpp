#include "ChasingTankAlgorithm.h"

/**
 * @brief Returns the next action for the tank.
 */
Action ChasingTankAlgorithm::getNextAction(const Board& board, std::vector<Player*>& players, 
                                           const std::set<Shell*>& shells, int playerId, int x, int y) {
    // Ignore unused parameters
    (void)x;
    (void)y;
    
    // Get our tank and the enemy tank
    Tank& myTank = **players[playerId - 1]->getTanks().begin();
    Tank& enemyTank = **players[(1 - (playerId - 1))]->getTanks().begin();
    
    // Try to take a basic safe action first (like moving away from a shell)
    Action action = tryBasicMoves(board, myTank, players, shells, playerId);
    if (action != Action::Wait) {
        return action; // If a valid basic move exists, take it immediately
    }

    // Otherwise, prepare to chase the enemy
    std::pair<int, int> enemyPos = enemyTank.getPosition();
    std::pair<int, int> myPos = myTank.getPosition();

    // Recalculate path if needed (first time or situation changed)
    if (currentPath.empty() || enemyPos != lastEnemyPosition || myPos != plannedPositions.front()) {
        auto pathResult = calculatePathBFS(board, myTank, enemyTank);
        currentPath = std::move(pathResult.first);
        plannedPositions = std::move(pathResult.second);
        lastEnemyPosition = enemyPos;
    }

    // Continue along the current path if it exists
    if (!currentPath.empty()) {
        Action nextAction = currentPath.front();
        currentPath.erase(currentPath.begin());
        plannedPositions.erase(plannedPositions.begin());
        return nextAction;
    }

    // No path or reached the end — fallback to default attack
    return Action::Shoot;
}

/**
 * @brief Attempts basic moves - shoot, avoid shells.
 */
Action ChasingTankAlgorithm::tryBasicMoves(const Board& board, const Tank& myTank,
                                           std::vector<Player*>& players, const std::set<Shell*>& shells, int playerId) {
    // Delegate to basic algorithm to check immediate safe moves
    BasicTankAlgorithm basicAlgo;
    auto [x, y] = myTank.getPosition();
    return basicAlgo.basicAlgorithm(players, shells, board, playerId ,x, y);
}

/**
 * @brief Computes a path to the enemy using BFS.
 */
std::pair<std::vector<Action>, std::vector<std::pair<int, int>>> ChasingTankAlgorithm::calculatePathBFS(const Board& board, const Tank& myTank, const Tank& enemyTank) {
    std::queue<State> q;
    std::map<State, State> parent;

    int width = board.getWidth();
    int height = board.getHeight();
    auto visited = std::vector(width, std::vector(height, std::vector(8, false)));

    auto [startX, startY] = myTank.getPosition();
    State start{startX, startY, myTank.getDirection()};
    q.push(start);
    visited[start.x][start.y][(int)start.direction] = true;

    while (!q.empty()) {
        State current = q.front();
        q.pop();

        // Check if we reached the enemy's position
        auto [enemyX, enemyY] = enemyTank.getPosition();
        if (current.x == enemyX && current.y == enemyY) {
            // Reconstruct path from current to start
            std::vector<Action> actions;
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
        auto [nx, ny] = moveInDirection(board, current.x, current.y, current.direction);
        if (board.isEmptyCell(nx, ny) && !visited[nx][ny][(int)current.direction]) {
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
 * @brief Moves one step in the given direction with wrapping.
 */
std::pair<int, int> ChasingTankAlgorithm::moveInDirection(const Board& board, int x, int y, Direction dir) {
    std::pair<int, int> next;
    switch (dir) {
        case Direction::U:         next = { x, y - 1 }; break;
        case Direction::UR:        next = { x + 1, y - 1 }; break;
        case Direction::R:         next = { x + 1, y }; break;
        case Direction::DR:        next = { x + 1, y + 1 }; break;
        case Direction::D:         next = { x, y + 1 }; break;
        case Direction::DL:        next = { x - 1, y + 1 }; break;
        case Direction::L:         next = { x - 1, y }; break;
        case Direction::UL:        next = { x - 1, y - 1 }; break;
    }
    board.wrapPosition(next.first, next.second); // wrap around board edges
    return next;
}

/**
 * @brief Deduces the action needed to go from one state to another.
 */
Action ChasingTankAlgorithm::inferAction(const State& from, const State& to) {
    if (from.x == to.x && from.y == to.y) {
        // Rotation only
        int diff = (int(to.direction) - int(from.direction) + 8) % 8;
        if (diff == 1) return Action::RotateRight1_8;
        if (diff == 2) return Action::RotateRight1_4;
        if (diff == 7) return Action::RotateLeft1_8;
        if (diff == 6) return Action::RotateLeft1_4;
    } else {
        // Position changed — must be a move forward
        return Action::MoveForward;
    }
    return Action::Wait; // fallback
}
