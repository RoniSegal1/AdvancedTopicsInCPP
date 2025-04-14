#include "GameManager.h"
#include <iostream>
#include <fstream>

GameManager::GameManager() : board(0, 0), algo1(nullptr), algo2(nullptr) {}

bool GameManager::loadGame(const std::string& filename) {
    // TODO: קוד לטעינת קובץ המשחק
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
    resolveCollisions();
    board.update();
}

void GameManager::applyActions(const std::string& action1, const std::string& action2) {
    // TODO: טיפול בפעולות
}

void GameManager::moveShells() {
    // TODO: קידום פגזים לפי כיוון
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
