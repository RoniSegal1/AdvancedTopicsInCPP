#include "GameManager.h"
#include "Board.h"
#include "Player.h"
#include "Tank.h"
#include "Entity.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>


// Constructor
GameManager::GameManager()
    : board(0, 0),
      algo1(nullptr),
      algo2(nullptr),
      stepCounter(0),
      drawCountdown(-1),
      players{ &player1, &player2 },
      shells(),
      positionMap(),
      inputErrors() {}


bool GameManager::loadGame(const std::string& filename) {
    // Reset game state for new load
    shells.clear();
    positionMap.clear();
    stepCounter = 0;
    players = { &player1, &player2 };

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open game file: " << filename << "\n";
        return false;
    }

    std::vector<std::string> rawLines;
    int width = 0, height = 0;

    if (!readRawBoardLines(file, rawLines)) return false;
    if (!readBoardDimensions(rawLines, width, height)) return false;
    normalizeRawBoardLines(rawLines, width, height);

    try {
        validateRawBoard(rawLines, width, height);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    board = Board(width, height);
    updateBoardTerrain(rawLines);
    placeTanksFromRawInput(rawLines);

    if (!inputErrors.empty()) {
        writeInputErrorsToFile();
    }

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
    //board.update();
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
    for (auto& [pos, entities] : positionMap) {
        bool removeTanks = false;
        bool removeShells = false;

        if (entities.size() < 2)
            continue;

        std::vector<Tank*> localTanks;
        std::vector<Shell*> localShells;

        for (Entity* ent : entities) {
            switch (ent->getType()) {
                case EntityType::Tank:
                    localTanks.push_back(static_cast<Tank*>(ent));
                    break;
                case EntityType::Shell:
                    localShells.push_back(static_cast<Shell*>(ent));
                    break;
                default:
                    break;
            }
        }

        Cell& cell = board.getCell(pos.first, pos.second);
        if (cell.getTerrain() == TerrainType::Wall) {
            cell.incrementWallHits();
            if (cell.getWallHits() >= 2) {
                cell.resetWall();
            }
            removeShells = true;
            continue;
        }

        if (cell.getTerrain() == TerrainType::Mine) {
            removeShells = true;
            removeTanks = true;
            continue;
        }

        if (localShells.size() > 1)
            removeShells = true;

        if (!localShells.empty() && !localTanks.empty()) {
            removeShells = true;
            removeTanks = true;
        }

        if (localTanks.size() > 1)
            removeTanks = true;

        for (Tank* tank : localTanks) {
            if (player1.getTanks().count(tank)) {
                destroyTank(tank, &player1);
            } else {
                destroyTank(tank, &player2);
            }
        }

        for (Shell* shell : localShells) {
            board.removeShell(shell);
            delete shell;
        }
    }
}



bool GameManager::checkWinConditions() {
    bool p1Alive = player1->hasAliveTanks();
    bool p2Alive = player2->hasAliveTanks();

    if (!p1Alive && !p2Alive) {
        std::cout << "Tie! Both tanks destroyed.\n";
        return true;
    }

    if (!p1Alive) {
        std::cout << "Player 2 wins! Player 1 destroyed.\n";
        return true;
    }

    if (!p2Alive) {
        std::cout << "Player 1 wins! Player 2 destroyed.\n";
        return true;
    }

    // Check if both players are out of shells
    bool p1Out = true, p2Out = true;

    for (Tank* t : player1->getTanks()) {
        if (t->getRemainingShells() > 0) {
            p1Out = false;
            break;
        }
    }

    for (Tank* t : player2->getTanks()) {
        if (t->getRemainingShells() > 0) {
            p2Out = false;
            break;
        }
    }

    if (p1Out && p2Out) {
        if (drawCountdown == -1) {
            drawCountdown = 40;
            std::cout << "Both players out of shells. Starting 40-turn countdown.\n";
        } else {
            drawCountdown--;
            if (drawCountdown == 0) {
                std::cout << "Tie! 40 steps passed with no winner.\n";
                return true;
            }
        }
    }

    return false; // game continues
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

// ---------------------------------------------------------------------------------------------------
// LoadGame() utilities:

bool GameManager::readRawBoardLines(std::ifstream& file, std::vector<std::string>& lines) {
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return !lines.empty();
}

bool GameManager::readBoardDimensions(const std::vector<std::string>& lines, int& width, int& height) {
    height = lines.size();
    width = lines[0].size();
    return true;
}

void GameManager::normalizeRawBoardLines(std::vector<std::string>& lines, int width, int height) {
    for (std::string& line : lines) {
        if ((int)line.length() < width)
            line.append(width - line.length(), ' ');
        else if ((int)line.length() > width)
            line = line.substr(0, width);
    }
    while ((int)lines.size() < height) {
        lines.emplace_back(width, ' ');
    }
    if ((int)lines.size() > height) {
        lines.resize(height);
    }
}

void GameManager::validateRawBoard(std::vector<std::string>& lines, int width, int height) {
    int tank1Count = 0, tank2Count = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char& c = lines[y][x];
            if (c == '1') {
                ++tank1Count;
                if (tank1Count > 1) {
                    logRecoverableError("Extra tank for player 1 at (" + std::to_string(x) + "," + std::to_string(y) + ") ignored");
                    c = ' ';
                }
            } else if (c == '2') {
                ++tank2Count;
                if (tank2Count > 1) {
                    logRecoverableError("Extra tank for player 2 at (" + std::to_string(x) + "," + std::to_string(y) + ") ignored");
                    c = ' ';
                }
            } else if (c != '#' && c != '@' && c != ' ') {
                logRecoverableError("Unrecognized character '" + std::string(1, c) + "' at (" + std::to_string(x) + "," + std::to_string(y) + "), treating as space");
                c = ' ';
            }
        }
    }

    if (tank1Count == 0 || tank2Count == 0) {
        if (tank1Count == 0)
            std::cerr << "[LoadGame Error] Missing tank for player 1\n";
        if (tank2Count == 0)
            std::cerr << "[LoadGame Error] Missing tank for player 2\n";
        throw std::runtime_error("Unrecoverable board error: missing player tank(s)");
    }
}

void GameManager::updateBoardTerrain(const std::vector<std::string>& lines) {
    for (int y = 0; y < (int)lines.size(); ++y) {
        for (int x = 0; x < (int)lines[y].size(); ++x) {
            char c = lines[y][x];
            TerrainType terrain = TerrainType::Empty;
            if (c == '#') terrain = TerrainType::Wall;
            else if (c == '@') terrain = TerrainType::Mine;
            board.getCell(x, y).setTerrain(terrain);
        }
    }
}

void GameManager::placeTanksFromRawInput(const std::vector<std::string>& lines) {
    for (int y = 0; y < (int)lines.size(); ++y) {
        for (int x = 0; x < (int)lines[y].size(); ++x) {
            char c = lines[y][x];
            if (c == '1' || c == '2') {
                int playerId = c - '0';
                Direction dir = (playerId == 1) ? Direction::L : Direction::R;
                Tank* tank = new Tank(dir, x, y, playerId);
                // board.addTank(tank); REMOVE?????
                if (playerId == 1) player1.addTank(tank);
                else player2.addTank(tank);
            }
        }
    }
}

void GameManager::logRecoverableError(const std::string& msg) {
    inputErrors.push_back(msg);
}

void GameManager::writeInputErrorsToFile() {
    std::ofstream out("input_errors.txt");
    for (const std::string& msg : inputErrors) {
        out << msg << "\n";
    }
}
