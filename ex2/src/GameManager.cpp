#include "GameManager.h"

/**
 * @brief Reads and parses a board file, initializes the board and players.
 * @param fileName The name of the input file.
 * @return true if successful, false if there was a parsing or logic error.
 */
bool GameManager::readBoard(const std::string& fileName) {
    resetGameState();

    // Open the input file
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Failed to open board file: " << fileName << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip description

    if (!parseConfigLines(file)) return false;

    auto rawMap = readRawMap(file);
    normalizeRawMap(rawMap);
    board = std::make_unique<Board>(cols, rows);
    placeTerrain(rawMap);
    placeTanks(rawMap);
    if (!inputErrors.empty()) {
        writeInputErrorsToFile();
    }
    checkWinConditions();

    return true;
}

/**
 * @brief Runs the main game loop.
 * Advances the game turn-by-turn until a win condition or maximum turn limit is reached.
 */
void GameManager::run(){
    while (true && stepCounter < maxSteps) {
        processTurn();
        if (checkWinConditions()) break;
    }
}

/**
     * @brief Executes a full turn in the game.
     * Each player selects an action, actions are applied, and shells are moved.
*/
void GameManager::processTurn() {
    currentTurnActions.clear();
    // Get next action for each tank from its algorithm and apply the actions
    for (size_t i = 0; i < algoTanks.size(); ++i) {
        TankAlgorithm* algot = algoTanks[i].get();
        Tank* t = tanks[i].get();

        if (!t->getIsAlive()) {
            currentTurnActions.push_back("killed");
            continue;
        }

        ActionRequest action = algot->getAction();
        currentTurnActions.push_back(toString(action));
        applyAction(action, *t, *algot, i);
    }

    // Move shells and resolve any resulting collisions
    moveShells();
    rebuildPositionMap();
    resolveCollisions();

    if (outputLog) {
        for (size_t i = 0; i < currentTurnActions.size(); ++i) {
            *outputLog << currentTurnActions[i];
            if (i + 1 < currentTurnActions.size())
                *outputLog << ", ";
        }
        *outputLog << std::endl;
    }
}

/**
 * @brief Applies a single action for a specific player.
 * Handles backward state if necessary.
 */
void GameManager::applyAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i) {

    // Handle backward logic (may consume the entire turn)
    if (handleBackwardState(action, tank, i)) {
        return;
    }

    // TODO: Log the action to the output stream if available - HANDLE OUTPUT!!!
    
    // Apply the actual action (move, rotate, shoot, etc.)
    handleNormalAction(action, tank, algot, i);
    // Update shooting cooldown at the end of the turn
    tank.decreaseShootCooldown();
}

/**
 * @brief Moves all shells two steps with intermediate collision checks.
 */
void GameManager::moveShells() {
    moveShellsOneStep();
    rebuildPositionMap();
    resolveCollisions();
    moveShellsOneStep();
}

/**
 * @brief Moves all shells one step forward according to their direction.
 */
void GameManager::moveShellsOneStep() {
    for (const auto& shellPtr : shells) {
        Shell* shell = shellPtr.get();
        auto [nextX, nextY] = shell->getNextPosition(*board);

        // Update shell's internal position
        shell->setPosition(nextX, nextY);
    }
}

/**
 * @brief Detects and resolves collisions between entities on the board.
 * Handles walls, mines, tank-tank collisions, and shell interactions.
 */
void GameManager::resolveCollisions() {
    std::set<Shell*> shellsToRemove;
    std::set<Tank*> tanksToRemove;
    
    // Iterate over all board positions with entities
    for (auto& [pos, entities] : positionMap) {
        if (entities.empty()) continue;

        bool removeTanks = false;
        bool removeShells = false;

        std::vector<Tank*> localTanks;
        std::vector<Shell*> localShells;

        // Separate tanks and shells
        for (Entity* ent : entities) {
            if (ent->getType() == "Tank") {
                localTanks.push_back(static_cast<Tank*>(ent));
            } else if (ent->getType() == "Shell") {
                localShells.push_back(static_cast<Shell*>(ent));
            }
        }

        Cell& cell = board->getCell(pos.first, pos.second);
        // Wall hit logic: 2 hits destroy the wall
        if (cell.getTerrain() == TerrainType::Wall) {
            cell.incrementWallHits();
            if (cell.getWallHits() >= 2) {
                cell.resetWall();
            }
            removeShells = true;
        }
        
        // Mine logic: destroy any tank stepping on a mine
        if (cell.getTerrain() == TerrainType::Mine && !localTanks.empty()) {
            removeTanks = true;
            cell.resetMine();
        }

        // Shell and tank at same location: both destroyed
        if (!localShells.empty() && !localTanks.empty()) {
            removeShells = true;
            removeTanks = true;
        }

        // Multiple shells or tanks in the same cell: destroy them
        if (localShells.size() > 1)
            removeShells = true;

        if (localTanks.size() > 1)
            removeTanks = true;

        // Head-on shell collision (coming from opposite directions)
        if (localShells.size() == 1 && !removeShells){
            Shell* shell = localShells[0];
            Direction dir = shell->getDirection();
            auto [prevX, prevY] = shell->getPrevPosition(*board);
            auto it = positionMap.find({prevX, prevY});

            if (it != positionMap.end()) {
                for (Entity* ent : it->second) {
                    if (Shell* otherShell = dynamic_cast<Shell*>(ent)) {
                        Direction oppDir = otherShell->getDirection();
                        if (areOpposite(dir, oppDir)) {
                            removeShells = true;
                        }
                    }
                }
            }
        }

        // Mark entities for removal
        if (removeShells) {
            for (Shell* shell : localShells) {
                shellsToRemove.insert(shell);
            }
        }

        if (removeTanks) {
            for (Tank* tank : localTanks) {
                tanksToRemove.insert(tank);
            }
        }
    }

    // Remove all marked entities
    removeMarkedTanks(tanksToRemove);
    removeMarkedShells(shellsToRemove);
}

/**
 * @brief Rebuilds the map linking positions to their occupying entities.
 */
void GameManager::rebuildPositionMap() {
    positionMap.clear();

    for (const auto& tankPtr : tanks) {
        Tank* tank = tankPtr.get();
        if (tank->getIsAlive()){
            positionMap[tank->getPosition()].push_back(tank);
        }
    }

    for (const auto& shellPtr : shells) {
        Shell* shell = shellPtr.get();
        positionMap[shell->getPosition()].push_back(shell);
    }
}


/**
 * @brief Checks if the game has ended with a win, tie, or ongoing.
 */
bool GameManager::checkWinConditions(){
    determineWinner();
    switch (winner){
    case -1:
        return false;

    case 0:
        if (outputLog) *outputLog << "Tie! Both tanks destroyed.\n";
        return true;

    case 1:
        if (outputLog) *outputLog << "Player 1 wins! Player 2 destroyed.\n";
        return true;

    case 2:
        if (outputLog) *outputLog << "Player 2 wins! Player 1 destroyed.\n";
        return true;
    
    default:
        return true;
    }
    
    bool HasNoAmmo = false;
    for (const auto& tankPtr : tanks) {
        const Tank* t = tankPtr.get();
        if (t->getAmmo() > 0) {
            HasNoAmmo = true;
            break;
        }
    }

    // Start draw countdown if both are out of ammo
    if (!HasNoAmmo) {
        if (drawCountdown == -1) {
            drawCountdown = 40;
            if (maxSteps - stepCounter < 40){
                if (outputLog) *outputLog << "Both players out of shells.\n";
            }
            else{
                if (outputLog) *outputLog << "Both players out of shells. Starting 40-turn countdown.\n";
            }
        } else {
            drawCountdown--;
            if (drawCountdown == 0) {
                if (outputLog) *outputLog << "Tie! 40 steps passed with no winner.\n";
                return true;
            }
        }
    }
}

// ----------------------------------------------------------------
//                RESOLVE COLLISIONS UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Removes all shells marked for deletion.
 */
void GameManager::removeMarkedShells(const std::set<Shell*>& toRemove) {
    shells.erase(
        std::remove_if(shells.begin(), shells.end(),
            [&](const std::unique_ptr<Shell>& shellPtr) {
                return toRemove.count(shellPtr.get()) > 0;
            }),
        shells.end()
    );
}

/**
 * @brief Removes all tanks marked for deletion.
 */
void GameManager::removeMarkedTanks(const std::set<Tank*>& toRemove) {
    for (Tank* tank : toRemove) {
        tank->markAsDead();
        int id = tank->getPlayer();

        auto it = std::find_if(tanks.begin(), tanks.end(), [&](const std::unique_ptr<Tank>& ptr) {
            return ptr.get() == tank;
        });
        if (it != tanks.end()) {
            size_t index = std::distance(tanks.begin(), it);
            if (index < currentTurnActions.size()) {
                currentTurnActions[index] += " (killed)";
            }
        }

        auto* myPlayer = dynamic_cast<MyPlayer*>(players[id - 1].get());
        if (myPlayer) {
            myPlayer->removeTank();
        }
    }
}




// ----------------------------------------------------------------
//                  APPLY ACTION UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Handles backward movement logic for a tank.
 * Cancels illegal actions, manages delay counter, and executes movement.
 */
bool GameManager::handleBackwardState(ActionRequest action, Tank& tank, size_t i) {
    int backwardDelay = tank.getBackwardDelay();

    // Not in backward state – treat as normal
    if (tank.notInBackwardDelay()) {
        return false;
    }

    // Still waiting for backward delay to reach zero
    if (backwardDelay > 0) {
        tank.decreaseBackwardDelay();
        if (action == ActionRequest::MoveForward) {
            tank.resetBackwardDelay();
        }
        if (action != ActionRequest::DoNothing) {
            currentTurnActions[i] += " (ignored)";
            logBadAction(tank.getPlayer(), action, "Invalid Action - trying to move while in backward delay > 0");
        }
        return true;
    }

    // Delay reached zero – move backward automatically
    if (backwardDelay == 0) {
        tank.moveBackward(*board);
        tank.decreaseBackwardDelay();
        if (action != ActionRequest::DoNothing) {
            currentTurnActions[i] += " (ignored)";
            logBadAction(tank.getPlayer(), action, "Invalid Action - trying to move while in backward delay = 0");
        }
        return true;
    }

    // Backward state not started – wait for command
    if (backwardDelay == -1) {
        if (action == ActionRequest::MoveBackward) {
            tank.moveBackward(*board);
        } else {
            tank.decreaseBackwardDelay();
            return false;
        }
        return true;
    }

    // Any other unexpected state
    return false;
}


/**
 * @brief Handles a normal tank action (move, rotate, shoot, wait).
 */
void GameManager::handleNormalAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i) {
    switch (action) {
        case ActionRequest::GetBattleInfo: {
            int playerIndex = tank.getPlayer();
            Player& player = *players[playerIndex - 1];
            MySatelliteView view(*board, positionMap, tank.getPosition());
            player.updateTankWithBattleInfo(algot, view);
            break;
        }
        case ActionRequest::MoveForward:
            tank.moveForward(*board);
            break;
        case ActionRequest::MoveBackward:
            tank.startBackwardDelay();
            break;
        case ActionRequest::RotateLeft45:
            tank.rotate(-1);
            break;
        case ActionRequest::RotateRight45:
            tank.rotate(1);
            break;
        case ActionRequest::RotateLeft90:
            tank.rotate(-2);
            break;
        case ActionRequest::RotateRight90:
            tank.rotate(2);
            break;
        case ActionRequest::Shoot:
            if (tank.canShoot()) {
                Shell* shell = tank.shoot();
                shells.push_back(std::unique_ptr<Shell>(shell));
            }
            else {
                currentTurnActions[i] += " (ignored)";
                std::string note = "Invalid Shoot - Ammo: " + std::to_string(tank.getAmmo()) +
                           ", ShootDelay: " + std::to_string(tank.getShootCooldown());
                logBadAction(tank.getPlayer(), action, note);
            }
            break;
        case ActionRequest::DoNothing:
            // Do nothing
            break;
    }
}

// ----------------------------------------------------------------
//                     READ BOARD UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Parses a key=value line and returns the integer value.
 * @param line The line to parse.
 * @param key The expected key.
 * @return Integer value if parsing succeeds.
 * @throws std::invalid_argument if the line is malformed or value is not an integer.
 */
int parseIntFromLine(const std::string& line, const std::string& key) {
    auto pos = line.find('=');
    if (pos == std::string::npos)
        throw std::invalid_argument("Line missing '=': " + line);

    std::string lhs = line.substr(0, pos);
    std::string rhs = line.substr(pos + 1);

    // Remove spaces
    lhs.erase(remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());
    rhs.erase(remove_if(rhs.begin(), rhs.end(), ::isspace), rhs.end());

    if (lhs != key)
        throw std::invalid_argument("Expected key '" + key + "', found '" + lhs + "'");

    try {
        return std::stoi(rhs);
    } catch (...) {
        throw std::invalid_argument("Value is not an integer: '" + rhs + "'");
    }
}

/**
 * @brief Resets the game state, clearing the board, players, shells, and counters.
 */
void GameManager::resetGameState() {
    shells.clear();
    positionMap.clear();
    stepCounter = 0;
    players.clear();
    players.resize(2);
    //playerTankCount = {0, 0};
    winner = -1;
    board.reset();
}

bool GameManager::parseConfigLines(std::ifstream& file) {
    std::string line;
    std::getline(file, line); maxSteps = parseIntFromLine(line, "MaxSteps");
    std::getline(file, line); numShells = parseIntFromLine(line, "NumShells");
    std::getline(file, line); rows = parseIntFromLine(line, "Rows");
    std::getline(file, line); cols = parseIntFromLine(line, "Cols");
    return true;
}

void GameManager::normalizeRawMap(std::vector<std::string>& rawMap) {
    while (rawMap.size() < static_cast<size_t>(rows))
        rawMap.emplace_back(cols, ' ');
    if (rawMap.size() > static_cast<size_t>(rows))
        rawMap.resize(rows);

    for (std::string& row : rawMap) {
        if (row.size() < static_cast<size_t>(cols)) row.resize(cols, ' ');
        else if (row.size() > static_cast<size_t>(cols)) row = row.substr(0, cols);
    }
}

void GameManager::placeTerrain(const std::vector<std::string>& rawMap) {
    for (size_t y = 0; y < static_cast<size_t>(rows); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(cols); ++x) {
            char c = rawMap[y][x];
            if (c == '#')
                board->getCell(x, y).setTerrain(TerrainType::Wall);
            else if (c == '@')
                board->getCell(x, y).setTerrain(TerrainType::Mine);
        }
    }
}


void GameManager::placeTanks(const std::vector<std::string>& rawMap) {
    std::vector<int> playerTankCount(2, 0);

    for (size_t y = 0; y < static_cast<size_t>(rows); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(cols); ++x) {
            char c = rawMap[y][x];

            if (std::isdigit(c)) {
                int playerIndex = c - '0';

                if (playerIndex != 1 && playerIndex != 2) {
                    logRecoverableError("Unrecognized character '" + std::string(1, c) + "' at (" + std::to_string(x) + "," + std::to_string(y) + "), treating as space");
                    continue;
                }

                // create the player of he hasn't been created yet
                if (!players[playerIndex - 1]) {
                    players[playerIndex - 1] = playerFactory->create(playerIndex, x, y, maxSteps, numShells);
                }

                // adjust the direction of the tank
                Direction cannonDir = (playerIndex == 1) ? Direction::L : Direction::R;

                // create the tank
                auto tank = std::make_unique<Tank>(cannonDir, x, y, playerIndex, numShells, playerTankCount[playerIndex - 1]);
                Tank* tankPtr = tank.get(); 

                // create the tank algorithm
                auto algorithm = tankFactory->create(playerIndex, playerTankCount[playerIndex - 1]);

                // add to all relevent places
                tanks.push_back(std::move(tank));
                algoTanks.push_back(std::move(algorithm));
                positionMap[{x, y}].push_back(tankPtr);

                // update the tank count
                ++playerTankCount[playerIndex - 1];
            }
        }
    }
    if (players[0]) {
        auto* myPlayer0 = dynamic_cast<MyPlayer*>(players[0].get());
        if (myPlayer0) myPlayer0->setNumTanks(playerTankCount[0]);
    }
    if (players[1]) {
        auto* myPlayer1 = dynamic_cast<MyPlayer*>(players[1].get());
        if (myPlayer1) myPlayer1->setNumTanks(playerTankCount[1]);
    }

}


void GameManager::determineWinner() {
    bool p0_dead = !players[0];
    bool p1_dead = !players[1];

    if (!p0_dead) {
        auto* myPlayer0 = dynamic_cast<MyPlayer*>(players[0].get());
        p0_dead = !myPlayer0 || !myPlayer0->hasAliveTanks();
    }

    if (!p1_dead) {
        auto* myPlayer1 = dynamic_cast<MyPlayer*>(players[1].get());
        p1_dead = !myPlayer1 || !myPlayer1->hasAliveTanks();
    }

    if (p0_dead && p1_dead) {
        winner = 0;
    } else if (p0_dead) {
        winner = 2;
    } else if (p1_dead) {
        winner = 1;
    }
}

std::vector<std::string> GameManager::readRawMap(std::ifstream& file) {
    std::vector<std::string> rawMap;
    std::string line;
    while (std::getline(file, line)) {
        rawMap.push_back(line);
    }
    return rawMap;
}


// ----------------------------------------------------------------
//                   PRINTING/LOGS UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Logs a bad action performed by a player.
 */
void GameManager::logBadAction(int playerId, ActionRequest action, const std::string& note) {
    if (outputLog != nullptr) {
        *outputLog << "Player " << playerId << ": Bad action - " << toString(action);
        if (!note.empty()) {
            *outputLog << " (" << note << ")";
        }
        *outputLog << std::endl;
    }
}

/**
 * @brief Sets the output log stream for recording actions and events.
 */
void GameManager::setOutputLog(std::ostream& os) {
    outputLog = &os;
}

/**
 * @brief Records a recoverable error during board parsing.
 */
void GameManager::logRecoverableError(const std::string& msg) {
    inputErrors.push_back(msg);
}

/**
 * @brief Writes all recorded input errors into a text file.
 */
void GameManager::writeInputErrorsToFile() {
    std::ofstream out("input_errors.txt");
    for (const std::string& msg : inputErrors) {
        out << msg << "\n";
    }
}