#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

#include "Board.h"
#include "Tank.h"
#include "Direction.h"
#include "Entity.h"
#include "Player.h"
#include "PlayerFactory.h"
#include "TankAlgorithmFactory.h"
#include "ActionRequest.h"
#include "ActionRequestUtils.h"
#include "MySatelliteView.h"
#include "MyPlayer.h"
#include "Cell.h"

class GameManager {
public:
    GameManager(std::unique_ptr<PlayerFactory> pf, std::unique_ptr<TankAlgorithmFactory> tf);

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    ~GameManager() = default;

    bool readBoard(const std::string& fileName);

    void run();

    /**
     * @brief Sets the output log stream for recording actions and events.
     * @param os The output stream to log to.
     */
    void setOutputLog(std::ostream& os);

private:
    std::vector<std::unique_ptr<Shell>> shells;
    std::vector<std::unique_ptr<Player>> players;
    std::map<std::pair<int, int>, std::vector<Entity*>> positionMap;
    std::vector<std::pair<std::unique_ptr<Tank>, std::unique_ptr<TankAlgorithm>>> tankPerAlgoVector;
    int stepCounter = 0;

    std::unique_ptr<PlayerFactory> playerFactory;
    std::unique_ptr<TankAlgorithmFactory> tankFactory;

    std::unique_ptr<Board> board;
    std::vector<int> playerTankCount;

    std::vector<std::string> inputErrors;
    std::ostream* outputLog = nullptr;
    std::vector<std::string> currentTurnActions;

    int drawCountdown = -1;
    int maxSteps = 0;
    int numShells = 0;
    size_t rows = 0;
    size_t cols = 0;
    int winner = -1; // 0 = tie, 1/2 = winner player

    void resetGameState();

    bool parseConfigLines(std::ifstream& file);

    void normalizeRawMap(std::vector<std::string>& rawMap);

    void placeTerrain(const std::vector<std::string>& rawMap);

    void placeTanks(const std::vector<std::string>& rawMap);

    void determineWinner();

    std::vector<std::string> readRawMap(std::ifstream& file);

    void processTurn();

    /**
     * @brief Applies a single action for a specific player.
     * Handles backward state if necessary.
     * @param action The action to apply.
     */
    void applyAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i);

    /**
     * @brief Moves all shells two steps with intermediate collision checks.
     */
    void moveShells();

    /**
     * @brief Moves all shells one step forward according to their direction.
     */
    void moveShellsOneStep();

    /**
     * @brief Rebuilds the map linking positions to their occupying entities.
     */
    void rebuildPositionMap();

    /**
     * @brief Detects and resolves collisions between entities on the board.
     * Handles walls, mines, tank-tank collisions, and shell interactions.
     */
    void resolveCollisions();

    /**
     * @brief Checks if the game has ended with a win, tie, or ongoing.
     * @return true if the game has ended, false otherwise.
     */
    bool checkWinConditions();

    // ----------------------------------------------------------------
    //                  APPLY ACTION UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Handles a tank that is in backward movement state.
     * Manages the delay and movement logic related to moving backward.
     * @param action Player's attempted action.
     * @param tank The player's tank.
     * @return true if backward handling was performed, false otherwise.
     */
    bool handleBackwardState(ActionRequest action, Tank& tank, size_t i);

    /**
     * @brief Handles a normal tank action (move, rotate, shoot, wait).
     * @param action The action to perform.
     * @param tank The tank performing the action.
     */
    void handleNormalAction(ActionRequest action, Tank& tank, TankAlgorithm& algot, size_t i);

    // ----------------------------------------------------------------
    //                RESOLVE COLLISIONS UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Destroys a tank and removes it from its owner player.
     */
    void destroyTank(Tank* tank);

    /**
     * @brief Removes all shells marked for deletion.
     * @param toRemove Set of shell pointers to remove.
     */
    void removeMarkedShells(const std::set<Shell*>& toRemove);

    /**
     * @brief Removes all tanks marked for deletion.
     * @param toRemove Set of tank pointers to remove.
     */
    void removeMarkedTanks(const std::set<Tank*>& toRemove);

    
    // ----------------------------------------------------------------
    //                   PRINTING/LOGS UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Logs a bad action performed by a player.
     * @param playerId ID of the player.
     * @param action Action attempted by the player.
     * @param note Additional note for the log.
     */
    void logBadAction(int playerId, ActionRequest action, const std::string& note = "");

    /**
     * @brief Records a recoverable error during board parsing.
     * @param msg Error message.
     */
    void logRecoverableError(const std::string& msg);

    /**
     * @brief Writes all recorded input errors into a text file.
     */
    void writeInputErrorsToFile();
    
};