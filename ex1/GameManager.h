#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "Board.h"
#include "Tank.h"
#include "Player.h"
#include "TankAlgorithm.h"
#include "Entity.h"

class GameManager {
private:
    Board board;
    Player player1;
    Player player2;

    std::vector<Player*> players;
    std::vector<Shell*> shells;
    std::map<std::pair<int, int>, std::vector<Entity*>> positionMap;

    std::vector<std::string> inputErrors;

    int stepCounter = 0;
    int maxStepsAfterOutOfAmmo = 40;

    TankAlgorithm* algo1;
    TankAlgorithm* algo2;

public:
    GameManager();
    bool loadGame(const std::string& filename);
    void run();

private:
    // loadGame() utilities
    bool readRawBoardLines(std::ifstream& file, std::vector<std::string>& lines);
    bool readBoardDimensions(const std::vector<std::string>& lines, int& width, int& height);
    void normalizeRawBoardLines(std::vector<std::string>& lines, int width, int height);
    void validateRawBoard(std::vector<std::string>& lines, int width, int height);
    void updateBoardTerrain(const std::vector<std::string>& lines);
    void placeTanksFromRawInput(const std::vector<std::string>& lines);
    void logRecoverableError(const std::string& msg);
    void writeInputErrorsToFile();

    void processTurn();
    void applyActions(const std::string& action1, const std::string& action2);
    void moveShells();
    void rebuildPositionMap();
    void resolveCollisions();
    bool checkWinConditions();
    void logBadAction(int playerId, const std::string& action);
    void destroyTank(Tank* tank, Player* owner);
};
