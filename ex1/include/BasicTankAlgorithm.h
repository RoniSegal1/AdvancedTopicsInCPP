#pragma once
#include "Board.h"
#include "Tank.h"
#include "Player.h"
#include "TankAlgorithm.h"
#include "Action.h"
#include <string>
#include <utility>
#include <vector>
#include <cmath>

/**
 * @class BasicTankAlgorithm
 * @brief A simple algorithm for basic tank behavior.
 * 
 * Moves tanks out of danger, shoots enemies if possible,
 * and rotates when no moves are safe.
 */
class BasicTankAlgorithm : public TankAlgorithm {
public:
    /**
     * @brief Core decision-making logic: move, shoot, or wait.
     */
    Action basicAlgorithm(std::vector<Player*>& players, const std::set<Shell*>& shells,
                          const Board& board, int playerId, int x, int y);

    /**
     * @brief Main method: decides what the tank should do next.
     */
    Action getNextAction(const Board& board, std::vector<Player*>& players,
                         const std::set<Shell*>& shells, int playerId, int x, int y);

private:
    /**
     * @brief Moves the tank away from immediate danger if possible.
     */
    Action MoveTankFromDanger(std::vector<Player*>& players, 
                              const std::set<std::pair<int,int>>& possibleMoves,
                              const Board& board, int playerId);

    /**
     * @brief Checks if the tank should shoot an enemy in line of sight.
     */
    Action CheckIfINeedToShoot(std::vector<Player*>& players, const Board& board,
                               int playerId);

    /**
     * @brief Rotates the tank toward a free cell if no safe move is possible.
     */
    Action rotateToFreeCell(std::set<std::pair<int, int>> possibleMoves, int x, int y, 
                            std::vector<Player*>& players, int playerId);
};
