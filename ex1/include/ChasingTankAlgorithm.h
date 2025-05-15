#pragma once

#include "TankAlgorithm.h"
#include "BasicTankAlgorithm.h"
#include "Board.h"
#include "Tank.h"
#include "Action.h"
#include <queue>
#include <vector>
#include <tuple>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <iostream>

/**
 * @brief An advanced tank algorithm that chases the enemy using BFS pathfinding.
 * 
 * This class implements a smart strategy for a tank to pursue its enemy on the board.
 * It uses a breadth-first search (BFS) algorithm to plan a path, considers turning costs,
 * and avoids dangerous positions by falling back to the basic algorithm when needed.
 * The algorithm maintains internal state to track the current path, enemy position,
 * and planned movement steps.
 */
class ChasingTankAlgorithm : public TankAlgorithm {
public:
/**
     * @brief Constructs a ChasingTankAlgorithm object.
     */
    ChasingTankAlgorithm() = default;

    /**
     * @brief Destroys the ChasingTankAlgorithm object.
     */
    ~ChasingTankAlgorithm() override = default;
    
    /**
     * @brief Decides the next action for the tank.
     * 
     * Tries to avoid immediate threats using the basic algorithm.
     * If no action is taken, calculates a new path to the enemy using BFS.
     * Stores the planned path and executes it step-by-step.
     * 
     * @param board The current game board.
     * @param players The list of all players.
     * @param shells All active shells on the board.
     * @param playerId The ID of the current player.
     * @param x Current X position (ignored).
     * @param y Current Y position (ignored).
     * @return The next action to perform.
     */
    Action getNextAction(const Board& board, std::vector<Player*>& players, const std::set<Shell*>& shells, int playerId, int x, int y);

private:
    /**
     * @brief Represents a full state of a tank: position and direction.
     * Used as nodes in BFS for accurate pathfinding.
     */
    struct State {
        int x, y;
        Direction direction;

        State() : x(0), y(0), direction(Direction::U) {}
        State(int x, int y, Direction dir)
            : x(x), y(y), direction(dir) {}

        bool operator<(const State& other) const {
            return std::tie(x, y, direction) < std::tie(other.x, other.y, other.direction);
        }

        bool operator==(const State& other) const {
            return x == other.x && y == other.y && direction == other.direction;
        }
    };

    std::vector<Action> currentPath; ///< Cached path of actions to perform.
    std::vector<std::pair<int, int>> plannedPositions; ///< Planned positions associated with currentPath.
    std::pair<int, int> lastEnemyPosition = { -1, -1 }; ///< Last known position of the enemy.

    /**
     * @brief Attempts simple moves to avoid danger using the basic algorithm.
     * 
     * Delegates to BasicTankAlgorithm to decide if a safe move is available (e.g. avoiding shells).
     * 
     * @param board The game board.
     * @param myTank The current player's tank.
     * @param players All players.
     * @param shells All shells on the board.
     * @param playerId The current player ID.
     * @return The selected action, or Action::Wait if no immediate move is needed.
     */
    Action tryBasicMoves(const Board& board, const Tank& myTank,
                         std::vector<Player*>& players, const std::set<Shell*>& shells, int playerId);
    
    /**
     * @brief Calculates a path to the enemy tank using breadth-first search.
     * 
     * Explores possible sequences of moves and turns to reach the enemy's position.
     * Returns both the list of actions and the corresponding intermediate positions.
     * 
     * @param board The game board.
     * @param myTank The current player's tank.
     * @param enemyTank The enemy tank to chase.
     * @return A pair of (actions, positions).
     */
    std::pair<std::vector<Action>, std::vector<std::pair<int, int>>> calculatePathBFS(const Board& board, const Tank& myTank, 
                                                                                      const Tank& enemyTank); //stays
    
    /**
     * @brief Computes the next board position when moving one step in a given direction.
     * 
     * Wraps around the board edges if necessary.
     * 
     * @param board The game board.
     * @param x Current x position.
     * @param y Current y position.
     * @param dir The direction to move in.
     * @return The new (x, y) position after movement.
     */                                                                                  
    std::pair<int, int> moveInDirection(const Board& board, int x, int y, Direction dir);

    /**
     * @brief Determines which action transforms one state into another.
     * 
     * Used during path reconstruction to identify rotations or movement.
     * 
     * @param from The starting state.
     * @param to The resulting state.
     * @return The corresponding Action to go from 'from' to 'to'.
     */
    Action inferAction(const State& from, const State& to); //stays
};
