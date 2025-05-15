#include "GameManager.h"
#include "Board.h"
#include "Player.h"
#include "Tank.h"
#include "Entity.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>


/**
 * @brief Constructs a new GameManager object.
 * Initializes an empty board and prepares players, shells, and internal structures.
 */
GameManager::GameManager()
    : board(0, 0),
      players{ &player1, &player2 },
      shells(),
      positionMap(),
      inputErrors(),
      drawCountdown(-1),
      stepCounter(0) {}

/**
 * @brief Destroys the GameManager object.
 * Cleans up all dynamically allocated resources: shells, tanks, algorithms.
 */
GameManager::~GameManager() {
    for (Shell* shell : shells) {
        delete shell;
    }
    shells.clear();
    delete algo1;
    delete algo2;
    for (Player* player : players) {
        for (Tank* tank : player->getTanks()) {
            delete tank;
        }
        player->clearTanks();
    }

    positionMap.clear();
    inputErrors.clear();
}

/**
 * @brief Runs the main game loop.
 * Advances the game turn-by-turn until a win condition or maximum turn limit is reached.
 */
void GameManager::run() {
    int turn = 0;
    while (true && turn < 200) {
        processTurn();
        if (checkWinConditions()) break;
    }
}

/**
     * @brief Loads the game state from a file.
     * Initializes the board, players, and terrain based on the input file.
*/
bool GameManager::loadGame(const std::string& filename) {
    // Clear previous game state
    shells.clear();
    positionMap.clear();
    stepCounter = 0;
    players = { &player1, &player2 };

    // Open the input file
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open game file: " << filename << "\n";
        return false;
    }

    // Read board dimensions and raw board lines
    std::vector<std::string> rawLines;
    int width = 0, height = 0;

    if (!readBoardDimensions(file, width, height)) return false;
    if (!readRawBoardLines(file, rawLines)) return false;
    normalizeRawBoardLines(rawLines, width, height);

    // Validate the board contents
    if (!validateRawBoard(rawLines, width, height)) return false;

    // Initialize the board and place terrain and tanks
    board = Board(width, height);
    updateBoardTerrain(rawLines);
    placeTanksFromRawInput(rawLines);

    // Log any recoverable input errors
    if (!inputErrors.empty()) {
        writeInputErrorsToFile();
    }

    // Assign player algorithms
    algo1 = new BasicTankAlgorithm();
    algo2 = new ChasingTankAlgorithm();

    return true;
}

/**
     * @brief Executes a full turn in the game.
     * Each player selects an action, actions are applied, and shells are moved.
*/
void GameManager::processTurn() {
    // Get both tanks from the players (assuming one tank per player)
    Tank& tank1 = **players[0]->getTanks().begin();
    Tank& tank2 = **players[1]->getTanks().begin();

    auto [x1, y1] = tank1.getPosition();
    auto [x2, y2] = tank2.getPosition();

    // Get next action for each tank from its algorithm
    Action action1 = algo1->getNextAction(board, players, shells, 1, x1, y1);
    Action action2 = algo2->getNextAction(board, players, shells, 2, x2, y2);

    // Apply the actions
    applyAction(action1, 1);
    applyAction(action2, 2);

    // Move shells and resolve any resulting collisions
    moveShells();
    rebuildPositionMap();
    resolveCollisions();
}

/**
 * @brief Applies a single action for a specific player.
 * Handles backward state if necessary.
 */
void GameManager::applyAction(Action action, int playerIndex) {
    // Get the player's tank (assumes one tank per player)
    Player* player = players[playerIndex-1];
    Tank& tank = **player->getTanks().begin();

    // Handle backward logic (may consume the entire turn)
    if (handleBackwardState(action, tank, playerIndex)) {
        return;
    }

    // Log the action to the output stream if available
    if (outputLog != nullptr) {
        *outputLog << "Player " << playerIndex << ": " << actionToString(action) << std::endl;
    }
    
    // Apply the actual action (move, rotate, shoot, etc.)
    handleNormalAction(action, tank, playerIndex);
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
    for (Shell* shell : shells) {
        auto [nextX, nextY] = shell->getNextPosition(board);

        // Wrap around board if needed
        board.wrapPosition(nextX, nextY);

        // Update shell's internal position
        shell->setPosition(nextX, nextY); // You'll need to add this setter to Shell
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

        Cell& cell = board.getCell(pos.first, pos.second);
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
            auto [prevX, prevY] = shell->getPrevPosition(board);
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
 * @brief Checks if the game has ended with a win, tie, or ongoing.
 */
bool GameManager::checkWinConditions() {
    // Check if both players still have tanks alive
    bool p1Alive = player1.hasAliveTanks();
    bool p2Alive = player2.hasAliveTanks();

    // Tie: both destroyed
    if (!p1Alive && !p2Alive) {
        if (outputLog) *outputLog << "Tie! Both tanks destroyed.\n";
        return true;
    }

    // Win conditions
    if (!p1Alive) {
        if (outputLog) *outputLog << "Player 2 wins! Player 1 destroyed.\n";
        return true;
    }

    if (!p2Alive) {
        if (outputLog) *outputLog << "Player 1 wins! Player 2 destroyed.\n";
        return true;
    }

    // Check if both players are out of ammo
    bool p1Out = true, p2Out = true;

    for (Tank* t : player1.getTanks()) {
        if (t->getAmmo() > 0) {
            p1Out = false;
            break;
        }
    }

    for (Tank* t : player2.getTanks()) {
        if (t->getAmmo() > 0) {
            p2Out = false;
            break;
        }
    }

    // Start draw countdown if both are out of ammo
    if (p1Out && p2Out) {
        if (drawCountdown == -1) {
            drawCountdown = 40;
            if (outputLog) *outputLog << "Both players out of shells. Starting 40-turn countdown.\n";
        } else {
            drawCountdown--;
            if (drawCountdown == 0) {
                if (outputLog) *outputLog << "Tie! 40 steps passed with no winner.\n";
                return true;
            }
        }
    }

    return false; // game continues
}


/**
 * @brief Rebuilds the map linking positions to their occupying entities.
 */
void GameManager::rebuildPositionMap() {
    // Clear previous entity mapping
    positionMap.clear();

    // Map each tank to its position
    for (Player* player : players) {
        for (Tank* tank : player->getTanks()) {
            positionMap[tank->getPosition()].push_back(tank);
        }
    }    

    // Map each shell to its position
    for (Shell* shell : shells) {
        positionMap[shell->getPosition()].push_back(shell);
    }
}


// ----------------------------------------------------------------
//                RESOLVE COLLISIONS UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Removes all shells marked for deletion.
 */
void GameManager::removeMarkedShells(const std::set<Shell*>& toRemove) {
    for (Shell* shell : toRemove) {
        shells.erase(shell);
        delete shell;
    }
}

/**
 * @brief Removes all tanks marked for deletion.
 */
void GameManager::removeMarkedTanks(const std::set<Tank*>& toRemove) {
    for (Tank* tank : toRemove) {
        Player* owner = (tank->getPlayer() == 1 ? &player1 : &player2);
        destroyTank(tank, owner);
    }
}

/**
 * @brief Destroys a tank and removes it from its owner player.
 */
void GameManager::destroyTank(Tank* tank, Player* owner) {
    owner->removeTank(tank);
    delete tank;
}

// ----------------------------------------------------------------
//                  APPLY ACTION UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Handles backward movement logic for a tank.
 * Cancels illegal actions, manages delay counter, and executes movement.
 */
bool GameManager::handleBackwardState(Action action, Tank& tank, int playerIndex) {
    int backwardDelay = tank.getBackwardDelay();

    // Not in backward state – treat as normal
    if (tank.notInBackwardDelay()) {
        return false;
    }

    // Still waiting for backward delay to reach zero
    if (backwardDelay > 0) {
        tank.decreaseBackwardDelay();
        if (action == Action::MoveForward) {
            tank.resetBackwardDelay();
        }
        if (action != Action::Wait) {
            logBadAction(playerIndex, action, "Invalid Action - trying to move while in backward delay > 0");
        }
        return true;
    }

    // Delay reached zero – move backward automatically
    if (backwardDelay == 0) {
        tank.moveBackward(board);
        tank.decreaseBackwardDelay();
        if (action != Action::Wait) {
            logBadAction(playerIndex, action, "Invalid Action - trying to move while in backward delay = 0");
        }
        return true;
    }

    // Backward state not started – wait for command
    if (backwardDelay == -1) {
        if (action == Action::MoveBackward) {
            tank.moveBackward(board);
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
void GameManager::handleNormalAction(Action action, Tank& tank, int playerIndex) {
    switch (action) {
        case Action::MoveForward:
            tank.moveForward(board);
            break;
        case Action::MoveBackward:
            tank.startBackwardDelay();
            break;
        case Action::RotateLeft1_8:
            tank.rotate(-1);
            break;
        case Action::RotateRight1_8:
            tank.rotate(1);
            break;
        case Action::RotateLeft1_4:
            tank.rotate(-2);
            break;
        case Action::RotateRight1_4:
            tank.rotate(2);
            break;
        case Action::Shoot:
            if (tank.canShoot()) {
                Shell* shell = tank.shoot();
                shells.insert(shell);
            }
            else {
                std::string note = "Invalid Shoot - Ammo: " + std::to_string(tank.getAmmo()) +
                           ", ShootDelay: " + std::to_string(tank.getShootCooldown());
                logBadAction(playerIndex, action, note);
            }
            break;
        case Action::Wait:
            // Do nothing
            break;
    }
}

// ----------------------------------------------------------------
//                   PRINTING/LOGS UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Logs a bad action performed by a player.
 */
void GameManager::logBadAction(int playerId, Action action, const std::string& note) {
    if (outputLog != nullptr) {
        *outputLog << "Player " << playerId << ": Bad action - " << actionToString(action);
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

// ----------------------------------------------------------------
//                     LOAD GAME UTILITIES
// ----------------------------------------------------------------

/**
 * @brief Reads all remaining lines from the input file as raw board content.
 */
bool GameManager::readRawBoardLines(std::ifstream& file, std::vector<std::string>& lines) {
    std::string line;
    // Read each line and add to vector
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    // Check for empty file content
    if (lines.empty()) {
        std::cerr << "No board data found after dimensions line." << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Reads the board width and height from the first line of the input file.
 */
bool GameManager::readBoardDimensions(std::ifstream& file, int& width, int& height) {
    // Try reading two integers from the file (width and height)
    if (!(file >> width >> height)) {
        std::cerr << "Invalid or missing board dimensions in input file." << std::endl;
        return false;
    }
    // Skip the rest of the line after reading the dimensions
    std::string dummy;
    std::getline(file, dummy);
    return true;
}

/**
 * @brief Adjusts the size of each board line and the number of lines to match the specified dimensions.
 */
void GameManager::normalizeRawBoardLines(std::vector<std::string>& lines, int width, int height) {
    // Ensure each line has exactly 'width' characters
    for (std::string& line : lines) {
        if ((int)line.length() < width)
            line.append(width - line.length(), ' '); // pad with spaces
        else if ((int)line.length() > width)
            line = line.substr(0, width); // truncate extra characters
    }
    // Ensure total number of lines is exactly 'height'
    while ((int)lines.size() < height) {
        lines.emplace_back(width, ' '); // add empty lines
    }
    if ((int)lines.size() > height) {
        lines.resize(height); // remove extra lines
    }
}

/**
 * @brief Validates the board content: one tank per player and only allowed symbols.
 */
bool GameManager::validateRawBoard(std::vector<std::string>& lines, int width, int height) {
    int tank1Count = 0, tank2Count = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char& c = lines[y][x];

            // Count tanks and validate symbols
            if (c == '1') {
                ++tank1Count;
                if (tank1Count > 1) {
                    logRecoverableError("Extra tank for player 1 at (" + std::to_string(x) + "," + std::to_string(y) + ") ignored");
                    c = ' ';
                }
            } else if (c == '2') {
                ++tank2Count;
                if (tank2Count > 1) {
                    logRecoverableError("Extra tank for player 2 at (" + std::to_string(x) + "," + std::to_string(y) + ") ignored");
                    c = ' ';
                }
            } else if (c != '#' && c != '@' && c != ' ') {
                logRecoverableError("Unrecognized character '" + std::string(1, c) + "' at (" + std::to_string(x) + "," + std::to_string(y) + "), treating as space");
                c = ' ';
            }
        }
    }

    // Ensure each player has exactly one tank
    if (tank1Count == 0 || tank2Count == 0) {
        if (tank1Count == 0)
            std::cerr << "[LoadGame Error] Missing tank for player 1\n";
        if (tank2Count == 0)
            std::cerr << "[LoadGame Error] Missing tank for player 2\n";
        return false;
    }

    return true;
}

/**
 * @brief Sets the terrain type of each cell based on the input character ('#' for wall, '@' for mine).
 */
void GameManager::updateBoardTerrain(const std::vector<std::string>& lines) {
    for (int y = 0; y < (int)lines.size(); ++y) {
        for (int x = 0; x < (int)lines[y].size(); ++x) {
            char c = lines[y][x];
            TerrainType terrain = TerrainType::Empty;
            if (c == '#') terrain = TerrainType::Wall;
            else if (c == '@') terrain = TerrainType::Mine;
            board.getCell(x, y).setTerrain(terrain);
        }
    }
}

/**
 * @brief Places player tanks on the board based on their starting symbols ('1' and '2') in the input.
 * Sets initial direction: player 1 faces left, player 2 faces right.
 */
void GameManager::placeTanksFromRawInput(const std::vector<std::string>& lines) {
    // Scan the board input to find player starting positions
    for (int y = 0; y < (int)lines.size(); ++y) {
        for (int x = 0; x < (int)lines[y].size(); ++x) {
            char c = lines[y][x];
            if (c == '1' || c == '2') {
                int playerId = c - '0';
                Direction dir = (playerId == 1) ? Direction::L : Direction::R;

                // Create a new tank at the specified location with initial direction
                Tank* tank = new Tank(dir, x, y, playerId);

                // Add the tank to the appropriate player
                if (playerId == 1) player1.addTank(tank);
                else player2.addTank(tank);
            }
        }
    }

    // After placing tanks, update the position map
    for (Player* player : players) {
        for (Tank* tank : player->getTanks()) {
            positionMap[tank->getPosition()].push_back(tank);
        }
    }
}

