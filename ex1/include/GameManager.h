#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include "Board.h"
#include "Tank.h"
#include "Player.h"
#include "BasicTankAlgorithm.h"
#include "ChasingTankAlgorithm.h"
#include "Entity.h"
#include "Action.h"

/**
 * @brief Manages the overall game state, including players, board, and shells.
 */
class GameManager {
private:
    Board board; ///< The game board.

    Player player1; ///< First player.
    Player player2; ///< Second player.
    std::vector<Player*> players; ///< List of player pointers.

    std::set<Shell*> shells; ///< Set of active shells on the board.

    std::map<std::pair<int, int>, std::vector<Entity*>> positionMap; ///< Map from position to entities.

    std::vector<std::string> inputErrors; ///< Collects errors from input parsing.

    int drawCountdown = -1; ///< Countdown for declaring a draw.
    int stepCounter = 0; ///< Number of steps played.

    TankAlgorithm* algo1; ///< Tank algorithm for player 1.
    TankAlgorithm* algo2; ///< Tank algorithm for player 2.

    std::ostream* outputLog = nullptr; ///< Optional log output stream.

public:
    /**
     * @brief Constructs a new GameManager object.
     */
    GameManager();

    /**
     * @brief Destroys the GameManager object and cleans up resources.
     */
    ~GameManager();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    GameManager(const GameManager&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     */
    GameManager& operator=(const GameManager&) = delete;

    /**
     * @brief Sets the output log stream for recording actions and events.
     * @param os The output stream to log to.
     */
    void setOutputLog(std::ostream& os);

    /**
     * @brief Loads the game state from a file.
     * Initializes the board, players, and terrain based on the input file.
     * @param filename Name of the file to load.
     * @return true if loading succeeded, false otherwise.
     */
    bool loadGame(const std::string& filename);

    /**
     * @brief Starts the game loop.
     */
    void run();

private:
    /**
     * @brief Executes a full turn in the game.
     * Each player selects an action, actions are applied, and shells are moved.
     */
    void processTurn();

    /**
     * @brief Applies a single action for a specific player.
     * Handles backward state if necessary.
     * @param action The action to apply.
     * @param playerIndex 1-based index of the player (1 or 2).
     */
    void applyAction(Action action, int i);

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
     * @param playerIndex 1-based player index.
     * @return true if backward handling was performed, false otherwise.
     */
    bool handleBackwardState(Action action, Tank& tank, int playerIndex);

    /**
     * @brief Handles a normal tank action (move, rotate, shoot, wait).
     * @param action The action to perform.
     * @param tank The tank performing the action.
     * @param playerIndex 1-based player index.
     */
    void handleNormalAction(Action action, Tank& tank, int playerIndex);

    // ----------------------------------------------------------------
    //                RESOLVE COLLISIONS UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Destroys a tank and removes it from its owner player.
     */
    void destroyTank(Tank* tank, Player* owner);

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
    void logBadAction(int playerId, Action action, const std::string& note = "");

    /**
     * @brief Records a recoverable error during board parsing.
     * @param msg Error message.
     */
    void logRecoverableError(const std::string& msg);

    /**
     * @brief Writes all recorded input errors into a text file.
     */
    void writeInputErrorsToFile();

    // ----------------------------------------------------------------
    //                     LOAD GAME UTILITIES
    // ----------------------------------------------------------------

    /**
     * @brief Reads all remaining lines from the input file as raw board content.
     * Each line represents a row on the board before normalization.
     * @param file Input file stream.
     * @param lines Output vector to store the board lines.
     * @return true if lines were read successfully, false if no content was found.
     */
    bool readRawBoardLines(std::ifstream& file, std::vector<std::string>& lines);

    /**
     * @brief Reads the board width and height from the first line of the input file.
     * @param file Input file stream.
     * @param width Output parameter for board width.
     * @param height Output parameter for board height.
     * @return true if dimensions were read successfully, false otherwise.
     */
    bool readBoardDimensions(std::ifstream& file, int& width, int& height);

    /**
     * @brief Adjusts the size of each board line and the number of lines to match the specified dimensions.
     * Pads short lines with spaces or truncates long lines; adds/removes rows if needed.
     * @param lines Board lines to normalize.
     * @param width Target board width.
     * @param height Target board height.
     */
    void normalizeRawBoardLines(std::vector<std::string>& lines, int width, int height);

    /**
     * @brief Validates that there is exactly one tank for each player and only allowed symbols on the board.
     * Replaces invalid characters and extra tanks with empty spaces.
     * @param lines Board lines.
     * @param width Board width.
     * @param height Board height.
     * @return true if the board is valid, false otherwise.
     */
    bool validateRawBoard(std::vector<std::string>& lines, int width, int height);

    /**
     * @brief Sets the terrain type of each cell based on the input character ('#' for wall, '@' for mine).
     * @param lines Board lines representing the terrain layout.
     */
    void updateBoardTerrain(const std::vector<std::string>& lines);

    /**
     * @brief Places player tanks on the board based on their starting symbols ('1' and '2') in the input.
     * Sets initial direction: player 1 faces left, player 2 faces right.
     * @param lines Board lines containing initial tank positions.
     */
    void placeTanksFromRawInput(const std::vector<std::string>& lines);

};