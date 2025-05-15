#include "TankAlgorithm.h"
#include "Board.h"
#include "Cell.h"

/**
 * @brief Returns the 3x3 neighborhood deltas including center.
 */
const std::vector<std::pair<int, int>> TankAlgorithm::getDeltas() {
    const std::vector<std::pair<int, int>> deltas = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0}, {0,  0}, {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };
    return deltas;
}

/**
 * @brief Computes all surrounding coordinates (including self).
 */
std::set<std::pair<int, int>> TankAlgorithm::doVicinity(int x, int y, const Board& board) {
    auto deltas = getDeltas();

    std::set<std::pair<int, int>> vicinity;
    for (const auto& [dx, dy] : deltas) {
        int nx = x + dx;
        int ny = y + dy;
        board.wrapPosition(nx, ny);
        vicinity.insert({nx, ny});
    }
    return vicinity;
}

/**
 * @brief Finds positions with adjacent mines or walls.
 */
std::set<std::pair<int, int>> TankAlgorithm::getAdjacentMinesAndWalls(const Board& board, int x, int y) {
    std::set<std::pair<int, int>> blockedPositions;
    auto deltas = getDeltas();

    for (const auto& [dx, dy] : deltas) {
        int nx = x + dx;
        int ny = y + dy;
        board.wrapPosition(nx, ny);

        const Cell& neighbor = board.getCell(nx, ny);
        TerrainType terrain = neighbor.getTerrain();

        if (terrain == TerrainType::Mine || terrain == TerrainType::Wall) {
            blockedPositions.emplace(nx, ny); // Mark positions as dangerous
        }
    }
    return blockedPositions;
}

/**
 * @brief Identifies nearby shell paths within threat radius.
 */
std::set<std::pair<int,int>> TankAlgorithm::shellsNextToMe(const Board& board, const std::set<Shell*>& shells, 
                                            int x, int y) {
    std::set<std::pair<int,int>> dangerZones;
    auto vicinity = doVicinity(x, y, board); // surrounding cells

    for (Shell* shell : shells) {
        auto [sx, sy] = shell->getPosition();
        Direction dir = shell->getDirection();
        auto [dx, dy] = getDelta(dir);
        // check up to 2 steps ahead in the direction of the shell
        for (int step = 0; step <= 2; ++step) {
            int tx = sx + step * dx;
            int ty = sy + step * dy;
            board.wrapPosition(tx, ty);

            if (vicinity.count({tx, ty})) {
                dangerZones.insert({tx, ty});
            }
        }
    }
    return dangerZones;
}

/**
 * @brief Locates nearby tanks in vicinity.
 */
std::set<std::pair<int,int>> TankAlgorithm::TanksNextToMe(const Board& board, std::vector<Player*>& players,
    int x, int y) {
    std::set<std::pair<int,int>> dangerZones;
    auto vicinity = doVicinity(x, y, board);

    for (Player* player : players) {
        for (Tank* tank : player->getTanks()) {
            auto [tx, ty] = tank->getPosition();
            if (vicinity.count({tx, ty})) {
                // avoid marking self
                if (tx != x && ty != y){
                    dangerZones.insert({tx, ty});
                }
            }
        }
    }
    return dangerZones;
}

/**
 * @brief Combines multiple sources of danger into one set.
 */
std::set<std::pair<int,int>> TankAlgorithm::getDangerZones(const Board& board, int x, int y, const std::set<Shell*>& shells,
    std::vector<Player*>& players) {
    std::set<std::pair<int,int>> dangerZones;
    auto blockedPositions = getAdjacentMinesAndWalls(board, x, y);
    auto shellDangerZones = shellsNextToMe(board, shells, x, y);
    auto tankDangerZones = TanksNextToMe(board, players, x, y);

    // merge all threat sources into one set
    dangerZones.insert(blockedPositions.begin(), blockedPositions.end());
    dangerZones.insert(shellDangerZones.begin(), shellDangerZones.end());
    dangerZones.insert(tankDangerZones.begin(), tankDangerZones.end());

    return dangerZones;
}

/**
 * @brief Finds all safe positions around (x, y) that are not in danger.
 */
std::set<std::pair<int, int>> TankAlgorithm::getSafeMovesAroundFull(const Board& board, int x, int y,
    const std::set<Shell*>& shells, std::vector<Player*>& players) {
    std::set<std::pair<int, int>> safeMoves;
    auto dangerZones = getDangerZones(board, x, y, shells, players);
    auto deltas = getDeltas();

    for (const auto& [dx, dy] : deltas) {
        int nx = x + dx;
        int ny = y + dy;
        board.wrapPosition(nx, ny);

        // add if not dangerous
        if (dangerZones.count({nx, ny}) == 0) {
            safeMoves.insert({nx, ny});
        }
    }
    return safeMoves;
}
