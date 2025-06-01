#pragma once

#include "SatelliteView.h"
#include "Entity.h"
#include "Board.h"
#include "Tank.h"
#include "Shell.h"
#include "Cell.h"
#include <map>
#include <vector>
#include <utility>

class MySatelliteView : public SatelliteView {
private:
    const Board& board;
    const std::map<std::pair<int, int>, std::vector<Entity*>>& positionMap;
    int rows;
    int cols;
    std::pair<int, int> requestingTankPosition;

public:
    MySatelliteView(const Board& board,
                    const std::map<std::pair<int, int>, std::vector<Entity*>>& posMap,
                    std::pair<int, int> requestingTankPos);

    char getObjectAt(size_t x, size_t y) const override;

    ~MySatelliteView() override = default;
};
