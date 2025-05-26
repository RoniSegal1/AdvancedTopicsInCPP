#include "MyTankAlgorithm.h"
#include "Board.h"
#include "Cell.h"

/**
 * @brief Returns the 3x3 neighborhood deltas including center.
 */
const std::vector<std::pair<int, int>> TankAlgorithm::getXDeltas(int x) {
    const std::vector<std::pair<int, int>> deltas = {
        {x*-1, x*-1}, {x*0, x*-1}, {x*1, x*-1},
        {x*-1,  x*0}, {x*0,  x*0}, {x*1,  x*0},
        {x*-1,  x*1}, {x*0,  x*1}, {x*1,  x*1}
    };
    return deltas;
}

/**
 * @brief Computes all surrounding coordinates (including self).
 */
std::set<std::pair<int, int>> TankAlgorithm::doDVicinity(int x, int y, int d, const Board& board) {
    auto deltas = getDeltas(d);

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
std::set<std::pair<int, int>> TankAlgorithm::getDangerZones(const Board& board, int x, int y) {
    std::set<std::pair<int, int>> dangerZones;
    auto vicinity1 = doDVicinity(x, y, 1, board);
    auto vicinity2 = doDVicinity(x, y, 2, board);

    for (vicinity : vicinity1) {
        const Cell& neighbor = board.getCell(nx, ny);
        TerrainType terrain = neighbor.getTerrain();

        if (terrain == TerrainType::Mine || terrain == TerrainType::Wall || terrain == TerrainType::Shell || terrain == TerrainType::EnemyTank) {
            dangerousPositions.emplace(nx, ny); // Mark positions as dangerous
        }
    }

    for (vicinity : vicinity2) {
        int nx = x + dx;
        int ny = y + dy;
        board.wrapPosition(nx, ny);

        const Cell& neighbor = board.getCell(nx, ny);
        TerrainType terrain = neighbor.getTerrain();

        if (terrain == TerrainType::Shell || terrain == TerrainType::EnemyTank) {
            blockedPositions.emplace(nx, ny); // Mark positions as dangerous
        }
    }
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
