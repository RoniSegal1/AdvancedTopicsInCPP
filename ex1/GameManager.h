#pragma once
#include <string>
#include "Board.h"
#include "Tank.h"
#include "TankAlgorithm.h"

class GameManager {
private:
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
    void processTurn();
    void applyActions(const std::string& action1, const std::string& action2);
    void moveShells();
    void resolveCollisions();
    bool checkWinConditions();
    void logBadAction(int playerId, const std::string& action);
};
