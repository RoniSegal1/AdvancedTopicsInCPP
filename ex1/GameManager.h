#pragma once
#include <string>
#include "Board.h"
#include "Tank.h"
#include "player.h"
#include "TankAlgorithm.h"
#include "Entity.h"
#include <map>
#include <vector>


class GameManager {
private:
    std::map<std::pair<int, int>, std::vector<Entity*>> positionMap;
    // check whether needed:
    Board board;
    Tank tank1;
    Tank tank2;
    TankAlgorithm* algo1;
    TankAlgorithm* algo2;

    int stepCounter = 0;
    int maxStepsAfterOutOfAmmo = 40;

public:
    GameManager();
    bool loadGame(const std::string& filename);
    void run();

private:
    // loadGame() utilities
    bool readBoardDimensions(std::ifstream& file, int& width, int& height);
    bool readRawBoardLines(std::ifstream& file, std::vector<std::string>& lines, int width, int height);
    bool validateRawBoard(const std::vector<std::string>& lines, int width, int height);
    void updateBoardTerrain(const std::vector<std::string>& lines);
    void placeTanksFromRawInput(const std::vector<std::string>& lines);
    //
    void processTurn();
    void applyActions(const std::string& action1, const std::string& action2);
    void moveShells();
    void rebuildPositionMap();
    void resolveCollisions();
    bool checkWinConditions();
    void logBadAction(int playerId, const std::string& action);

    void destroyTank(Tank* tank, Player* owner);
};
