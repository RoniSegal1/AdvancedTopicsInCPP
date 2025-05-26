#include "BasicTankAlgorithm.h"

BasicTankAlgorithm::BasicTankAlgorithm(int playerIndex, int tankIndex)
    : MyTankAlgorithm(playerIndex, tankIndex) {}

ActionRequest BasicTankAlgorithm::getAction() {
    if (!hasBoard()) { // don't have any information about the board, have to get it
        return shouldGetBattleInfo();
    }

    ActionRequest action = getThreatningNextAction();
    if (action == ActionRequest::DoNothing && turnsSinceLastUpdate >= 3) {
        return shouldGetBattleInfo();
    }

    ActionRequest action = getScaryNextAction();
    if (action == ActionRequest::DoNothing) {
        return shouldGetBattleInfo();
    }

    turnsSinceLastUpdate++;
    return action;
}

ActionRequest BasicTankAlgorithm::shouldGetBattleInfo() const {
    turnsSinceLastUpdate = 0;
    return ActionRequest::GetBattleInfo;
}

ActionRequest BasicTankAlgorithm::getThreatningNextAction() const {
    if (!canMoveForward()) {
        return ActionRequest::DoNothing;
    }
    for (int i = 0; i < 2; i++){
        ActionRequest act = MoveifShellsInRange(i);
        if (act == ActionRequest::MoveForward){
            ActionRequest::MoveForward;
        }
    }
    Direction dir = direction;
    auto [x, y] = position;
    auto [dx, dy] = getDelta(dir);
    for (int j = 0; j < 2; j++) {
        x += dx;
        y += dy;
        lastBoard->wrapPosition(x, y);
        const PlayerCell& cell = lastBoard->getCell(x, y);
        if (cell.getContent() == CellContent::EnemyTank) {
            return ActionRequest::Shoot;
        }
    }
    return ActionRequest::DoNothing;
}

ActionRequest BasicTankAlgorithm::getScaryNextAction() const {





    if (!canMoveForward()) {
        return ActionRequest::DoNothing;
    }

    for (int i = 0; i < 2; i++){
        ActionRequest act = MoveifShellsInRange(i);
        if (act == ActionRequest::MoveForward){
            ActionRequest::MoveForward;
        }
    }

    Direction dir = direction;
    auto [x, y] = position;
    auto [dx, dy] = getDelta(dir);

    for (int j = 0; j < 2; j++) {
        x += dx;
        y += dy;
        lastBoard->wrapPosition(x, y);
        const PlayerCell& cell = lastBoard->getCell(x, y);
        if (cell.getContent() == CellContent::EnemyTank) {
            return ActionRequest::Shoot;
        }
    }

    return ActionRequest::DoNothing;
}

ActionRequest BasicTankAlgorithm::MoveifShellsInRange(i) const{
    auto [x, y] = position;
    const auto& board = *lastBoard;
    auto vicinity = doDVicinity(x, y, distance, board);
    for (const auto& [sx, sy] : vicinity) {
        const PlayerCell& cell = board.getCell(sx, sy);
        if (cell.getContent() == CellContent::Shell) {
            return ActionRequest::MoveForward;
        }
    }
    return ActionRequest::DoNothing;
}

bool BasicTankAlgorithm::canMoveForward() const{
    auto dir = getMyDirection();
    auto [sx, sy] = [x, y] + direction.getDelta(dir);
    const Cell& neighbor = board.getCell(sx, sy);
    TerrainType terrain = neighbor.getTerrain();
    if (terrain == TerrainType::Empty) {
            return True;
    }
    return False;
}



#include "BasicTankAlgorithm.h"
#include "TankAlgorithm.h"

/**
 * @brief Core logic deciding whether to move, shoot, or wait.
 * 
 * First checks if the tank is standing in a danger zone.
 * If yes, attempts to move to a safer location.
 * Otherwise, considers shooting if an enemy is in sight.
 */
Action BasicTankAlgorithm::basicAlgorithm(std::vector<Player*>& players, const std::set<Shell*>& shells,
                                           const Board& board, int playerId, int x, int y) {
    // Identify all safe neighboring cells
    auto possibleMoves = getSafeMovesAroundFull(board, x, y, shells, players);
    Action act = Action::Wait;

    // If we're currently standing in a danger zone, try to escape
    if (possibleMoves.count({x, y}) == 0) {
        act = MoveTankFromDanger(players, possibleMoves, board, playerId);
    }

    // If we chose to stay, maybe we can shoot instead
    if (act == Action::Wait) {
        act = CheckIfINeedToShoot(players, board, playerId);
    }

    return act;
}

/**
 * @brief Rotates the tank toward a free adjacent cell, if any exist.
 * 
 * Tries small rotations first (1/8), then larger ones (1/4).
 */
Action BasicTankAlgorithm::rotateToFreeCell(std::set<std::pair<int, int>> possibleMoves, int x, int y,
                                             std::vector<Player*>& players, int playerId) {
    Player* player = players[playerId-1];
    Tank* tank = *player->getTanks().begin();
    Direction dir = tank->getDirection();

    // Try all four rotation options
    std::vector<std::pair<Action, Direction>> rotations = {
        {Action::RotateLeft1_8, static_cast<Direction>((static_cast<int>(dir) + 7) % 8)},
        {Action::RotateRight1_8, static_cast<Direction>((static_cast<int>(dir) + 1) % 8)},
        {Action::RotateLeft1_4, static_cast<Direction>((static_cast<int>(dir) + 6) % 8)},
        {Action::RotateRight1_4, static_cast<Direction>((static_cast<int>(dir) + 2) % 8)},
    };

    // Check which direction leads to a safe cell
    for (const auto& [action, newDir] : rotations) {
        auto delta = getDelta(newDir);
        int newX = x + delta.first;
        int newY = y + delta.second;
        if (possibleMoves.count({newX, newY})) {
            return action;
        }
    }

    return Action::Wait;
}


/**
 * @brief Attempts to move the tank away from danger, either forward or backward.
 */
Action BasicTankAlgorithm::MoveTankFromDanger(std::vector<Player*>& players,
                                               const std::set<std::pair<int,int>>& possibleMoves,
                                               const Board& board, int playerId) {

    Player* player = players[playerId-1];
    Tank* tank = *player->getTanks().begin();
    auto [newX, newY] = tank->getNextPosition(board);

    // Try moving forward if the next cell is safe
    if (possibleMoves.count({newX, newY}) == 1) {
        return Action::MoveForward;
    } else {
        // If not, try moving backward
        auto [oldX, oldY] = tank->getPrevPosition(board);
        if (possibleMoves.count({oldX, oldY}) == 1) {
            return Action::MoveBackward;
        }
    }
    return Action::Wait;
}

/**
 * @brief Checks if the tank should fire at an enemy tank in its line of sight.
 */
Action BasicTankAlgorithm::CheckIfINeedToShoot(std::vector<Player*>& players, const Board& board,
                                                int playerId) {


    Player* player = players[playerId-1];
    Tank* myTank = *player->getTanks().begin();

    // Check cooldown and ammo
    if (!myTank->canShoot()) {
        return Action::Wait;
    }

    Direction dir = myTank->getDirection();
    auto [dx, dy] = getDelta(dir);
    auto [newX, newY] = myTank->getNextPosition(board);

    Player* otherPlayer = players[1 - (playerId-1)];

    // Look ahead up to 6 steps
    for (int step = 1; step <= 6; ++step) {
        newX += dx;
        newY += dy;
        board.wrapPosition(newX, newY);
        const Cell& cell = board.getCell(newX, newY);
        // If we hit a wall, line of sight is blocked
        if (cell.getTerrain() == TerrainType::Wall) {
            break; // cannot shoot through walls
        }

        // Shoot if enemy tank is spotted
        for (Tank* enemy : otherPlayer->getTanks()) {
            if (enemy->getPosition() == std::pair{newX, newY}) {
                return Action::Shoot;
            }
        }
    }

    return Action::Wait;
}

/**
 * @brief Main method: delegates to basicAlgorithm and rotation if needed.
 */
Action BasicTankAlgorithm::getNextAction(const Board& board, std::vector<Player*>& players,
                                         const std::set<Shell*>& shells, int playerId, int x, int y) {
    Action action = basicAlgorithm(players, shells, board, playerId, x, y);

    // If no move or shoot chosen, try rotating
    if (action == Action::Wait) {
        std::set<std::pair<int, int>> possibleMoves = getSafeMovesAroundFull(board, x, y, shells, players);
        if (possibleMoves.empty()) {
            return Action::Wait;
        }
        return rotateToFreeCell(possibleMoves, x, y, players, playerId);
    }

    return action;
}
