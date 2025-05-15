#pragma once
#include "Board.h"
#include "Tank.h"
#include "Player.h"
#include "Action.h"
#include <string>
#include <utility>
#include <vector>
#include <cmath>
#include <set>
#include <map>

/**
 * @class TankAlgorithm
 * @brief Abstract base class for tank decision-making algorithms.
 * 
 * Defines an interface for computing the next action a tank should take.
 * Includes helper methods to evaluate danger zones and safe positions.
 */
class TankAlgorithm {
public:
    /**
     * @brief Virtual destructor for proper cleanup.
     */
    virtual ~TankAlgorithm() = default;

    /**
     * @brief Computes the next action for a tank based on the current game state.
     * @param board The game board
     * @param players The list of players
     * @param shells All active shells
     * @param playerId The ID of the tank's owner
     * @param x Current x position of the tank
     * @param y Current y position of the tank
     * @return The chosen action
     */
    virtual Action getNextAction(const Board& board, std::vector<Player*>& players,
        const std::set<Shell*>& shells, int playerId, int x, int y) = 0;

    /**
     * @brief Computes the set of positions considered dangerous.
     */
    std::set<std::pair<int,int>> getDangerZones(const Board& board, int x, int y, const std::set<Shell*>& shells,
                                                std::vector<Player*>& players);

    /**
     * @brief Returns all safe moves around the current tank location.
     */
    std::set<std::pair<int, int>> getSafeMovesAroundFull(const Board& board,
        int x, int y,
        const std::set<Shell*>& shells,
        std::vector<Player*>& players);

private:
    /**
     * @brief Returns the list of deltas to all neighboring positions.
     */
    const std::vector<std::pair<int, int>> getDeltas();

    /**
     * @brief Detects mines and walls around the tank.
     */
    std::set<std::pair<int,int>> getAdjacentMinesAndWalls(const Board& board, int x, int y);

    /**
     * @brief Detects threatening shell paths near the tank.
     */
    std::set<std::pair<int,int>> shellsNextToMe(const Board& board, const std::set<Shell*>& shells, 
                                                int x, int y);

    /**
     * @brief Detects nearby enemy tanks.
     */
    std::set<std::pair<int,int>> TanksNextToMe(const Board& board, std::vector<Player*>& players,
                                                int x, int y);

    std::set<std::pair<int, int>> doVicinity(int x, int y, const Board& board);

};
