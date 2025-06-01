#include "MySatelliteView.h"

MySatelliteView::MySatelliteView(const Board& board,
                                 const std::map<std::pair<int, int>, std::vector<Entity*>>& posMap,
                                 std::pair<int, int> requestingTankPos)
    : board(board), positionMap(posMap),
      rows(board.getRows()), cols(board.getCols()),
      requestingTankPosition(requestingTankPos) {}

char MySatelliteView::getObjectAt(size_t x, size_t y) const {
    if (x >= (size_t)rows || y >= (size_t)cols){
        return '&';
    }

    auto it = positionMap.find({(int)x, (int)y});
    if (it != positionMap.end()) {
        for (Entity* e : it->second) {
            if (e->getType() == "Tank") {
                if (e->getPosition() == requestingTankPosition){
                    return '%';
                }

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
