#include "GameManager.h"

/**
 * @brief Parses a key=value line and returns the integer value.
 * @param line The line to parse.
 * @param key The expected key.
 * @return Integer value if parsing succeeds.
 * @throws std::invalid_argument if the line is malformed or value is not an integer.
 */
int parseIntFromLine(const std::string& line, const std::string& key) {
    auto pos = line.find('=');
    if (pos == std::string::npos)
        throw std::invalid_argument("Line missing '=': " + line);

    std::string lhs = line.substr(0, pos);
    std::string rhs = line.substr(pos + 1);

    // Remove spaces
    lhs.erase(remove_if(lhs.begin(), lhs.end(), ::isspace), lhs.end());
    rhs.erase(remove_if(rhs.begin(), rhs.end(), ::isspace), rhs.end());

    if (lhs != key)
        throw std::invalid_argument("Expected key '" + key + "', found '" + lhs + "'");

    try {
        return std::stoi(rhs);
    } catch (...) {
        throw std::invalid_argument("Value is not an integer: '" + rhs + "'");
    }
}

/**
 * @brief Resets the game state, clearing the board, players, shells, and counters.
 */
void GameManager::resetGameState() {
    shells.clear();
    //positionMap.clear();
    stepCounter = 0;
    players.clear();
    players.resize(2);
    playerTankCount = {0, 0};
    winner = -1;
    board.reset();
}

bool GameManager::parseConfigLines(std::ifstream& file) {
    std::string line;
    std::getline(file, line); maxSteps = parseIntFromLine(line, "MaxSteps");
    std::getline(file, line); numShells = parseIntFromLine(line, "NumShells");
    std::getline(file, line); rows = parseIntFromLine(line, "Rows");
    std::getline(file, line); cols = parseIntFromLine(line, "Cols");
    return true;
}

void GameManager::normalizeRawMap(std::vector<std::string>& rawMap) {
    while (rawMap.size() < static_cast<size_t>(rows))
        rawMap.emplace_back(cols, ' ');
    if (rawMap.size() > static_cast<size_t>(rows))
        rawMap.resize(rows);

    for (std::string& row : rawMap) {
        if (row.size() < static_cast<size_t>(cols)) row.resize(cols, ' ');
        else if (row.size() > static_cast<size_t>(cols)) row = row.substr(0, cols);
    }
}

void GameManager::placeTerrainAndTanks(const std::vector<std::string>& rawMap) {
    for (size_t y = 0; y < static_cast<size_t>(rows); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(cols); ++x) {
            char c = rawMap[y][x];

            if (c == '#') board->getCell(x, y).setTerrain(TerrainType::Wall);
            else if (c == '@') board->getCell(x, y).setTerrain(TerrainType::Mine);
            else if (std::isdigit(c)) {
                // TODO: RONI YOU GO
            }
        }
    }
}

void GameManager::determineImmediateWinner() {
    if ((!players[0] || players[0]->getTankCount() == 0) &&
        (!players[1] || players[1]->getTankCount() == 0)) {
        winner = 0;
    } else if (!players[0] || players[0]->getTankCount() == 0) {
        winner = 2;
    } else if (!players[1] || players[1]->getTankCount() == 0) {
        winner = 1;
    }
}


/**
 * @brief Reads and parses a board file, initializes the board and players.
 * @param fileName The name of the input file.
 * @return true if successful, false if there was a parsing or logic error.
 */
bool GameManager::readBoard(const std::string& fileName) {
    resetGameState();

    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Failed to open board file: " << fileName << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip description

    try {
        if (!parseConfigLines(file))
            return false;
    } catch (const std::exception& ex) {
        std::cerr << "Error parsing config line: " << ex.what() << std::endl;
        return false;
    }

    std::vector<std::string> rawMap;
    std::string mapLine;
    while (std::getline(file, mapLine)) {
        rawMap.push_back(mapLine);
    }

    normalizeRawMap(rawMap);
    board = std::make_unique<Board>(cols, rows);
    placeTerrainAndTanks(rawMap);
    determineImmediateWinner();

    return true;
}