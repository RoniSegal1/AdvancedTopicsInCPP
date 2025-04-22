#include "GameManager.h"
#include <iostream>
#include <fstream>

GameManager::GameManager() : board(0, 0), algo1(nullptr), algo2(nullptr) {}

bool GameManager::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open game file: " << filename << "\n";
        return false;
    }

    int width = 0, height = 0;
    std::vector<std::string> rawLines;

    if (!readBoardDimensions(file, width, height)) return false;
    if (!readRawBoardLines(file, rawLines, width, height)) return false;
    if (!validateRawBoard(rawLines, width, height)) return false;

    board = Board(width, height);

    updateBoardTerrain(rawLines);
    placeTanksFromRawInput(rawLines);

    return true;
}

void GameManager::run() {
    while (true) {
        processTurn();
        if (checkWinConditions()) break;
    }
}

void GameManager::processTurn() {
    std::string action1 = algo1->getNextAction(board);
    std::string action2 = algo2->getNextAction(board);

    applyActions(action1, action2);
    moveShells();
    rebuildPositionMap(); //אם משתמשים בו רק בריסולב כוליסיון - אין צורך לקרוא לו גם אחריו
    resolveCollisions();
    checkWinConditions();
    board.update();
}

void GameManager::applyActions(const std::string& action1, const std::string& action2) {
    // TODO:  טיפול בפעולות ותזוזת טנקים
}

void GameManager::moveShells() {
    for (Shell* shell : board.getShells()) {
        auto [nextX, nextY] = shell->getNextShellPosition();

        // Wrap around board if needed
        board.wrapPosition(nextX, nextY);

        // Update shell's internal position
        shell->setPosition(nextX, nextY); // You'll need to add this setter to Shell
    }
}

void GameManager::resolveCollisions() {
    // TODO: טיפול בהתנגשויות
}

bool GameManager::checkWinConditions() {
    // TODO: בדיקת סיום משחק
    return false;
}

void GameManager::logBadAction(int playerId, const std::string& action) {
    std::cerr << "Player " << playerId << " made invalid move: " << action << "\n";
}

void GameManager::rebuildPositionMap() {
    positionMap.clear();

    for (Tank* tank : board.getTanks()) {
        positionMap[tank->getPosition()].push_back(tank);
    }

    for (Shell* shell : board.getShells()) {
        positionMap[shell->getPosition()].push_back(shell);
    }
}

void GameManager::destroyTank(Tank* tank, Player* owner) {
    board.removeTank(tank);
    owner->removeTank(tank);
    delete tank;
}
