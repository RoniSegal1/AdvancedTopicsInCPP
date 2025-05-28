#include "MySatelliteView.h"

MySatelliteView::MySatelliteView(const Board<GameCell>& board,
                                 const std::map<std::pair<int, int>, std::vector<Entity*>>& posMap,
                                 std::pair<int, int> requestingTankPos)
    : board(board), positionMap(posMap),
      width(board.getWidth()), height(board.getHeight()),
      requestingTankPosition(requestingTankPos) {}

char MySatelliteView::getObjectAt(size_t x, size_t y) const {
    if (x >= (size_t)width || y >= (size_t)height)
        return '&';

    auto it = positionMap.find({(int)x, (int)y});
    if (it != positionMap.end()) {
        for (Entity* e : it->second) {
            if (e->getType() == "Tank") {
                if (e->getPosition() == requestingTankPosition)
                    return '%';

                Tank* tank = dynamic_cast<Tank*>(e);
                if (tank) {
                    int owner = tank->getPlayer();
                    return owner == 1 ? '1' : '2';
                }
            }
            if (e->getType() == "Shell") {
                return '*';
            }
        }
    }

    const Cell& cell = board.getCell((int)x, (int)y);
    TerrainType terrain = cell.getTerrain();

    if (terrain == TerrainType::Wall)
        return '#';
    if (terrain == TerrainType::Mine)
        return '@';

    return ' ';
}
