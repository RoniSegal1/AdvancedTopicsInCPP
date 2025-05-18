#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

#include "Board.h"
#include "Direction.h"
//#include "Entity.h"
#include "Player.h"
#include "PlayerFactory.h"
#include "TankAlgorithmFactory.h"

class GameManager {
public:
    GameManager(std::unique_ptr<PlayerFactory> playerFactory,
                std::unique_ptr<TankAlgorithmFactory> tankFactory);

    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    ~GameManager() = default;

    bool readBoard(const std::string& fileName);

private:
    std::vector<std::unique_ptr<Shell>> shells;
    //std::map<std::pair<int, int>, std::vector<Entity*>> positionMap;
    int stepCounter = 0;
    std::unique_ptr<PlayerFactory> playerFactory;
    std::unique_ptr<TankAlgorithmFactory> tankFactory;

    std::unique_ptr<Board> board;
    std::vector<std::unique_ptr<Player>> players; // size 2
    std::vector<int> playerTankCount = {0, 0};

    int maxSteps = 0;
    int numShells = 0;
    int rows = 0;
    int cols = 0;
    int winner = -1; // 0 = tie, 1/2 = winner player

    /**
     * @brief Resets the game state, clearing the board, players, shells, and counters.
     */
    void resetGameState();

    bool parseConfigLines(std::ifstream& file);

    void normalizeRawMap(std::vector<std::string>& rawMap);

    void placeTerrainAndTanks(const std::vector<std::string>& rawMap);

    void GameManager::determineImmediateWinner();

    
};