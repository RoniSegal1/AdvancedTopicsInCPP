#include "Board.h"
#include "Cell.h"
#include "Tank.h"

Board::Board(int w, int h) : width(w), height(h), grid(h) {
    for (int y = 0; y < h; ++y) {
        grid[y].reserve(w);
        for (int x = 0; x < w; ++x) {
            grid[y].emplace_back(x, y);
        }
    }
}

Cell& Board::getCell(int x, int y) { // needed bhirat mehdal of cell
    wrapPosition(x, y);
    return grid[y][x];
}

void Board::update() {
    // for what?
}

int Board::getWidth() const {
  return width;
}

int Board::getHeight() const {
  return height;
}

void Board::wrapPosition(int& x, int& y) const { //check if needed
    x = (x + width) % width;
    y = (y + height) % height;
}

void Board::addWall(int row, int col) {
    getCell(col, row).setTerrain(TerrainType::Wall);
}

void Board::addMine(int row, int col) {
    getCell(col, row).setTerrain(TerrainType::Mine);
}

void Board::addTank(int row, int col, int playerId) {
    Direction dir = (playerId == 1 ? Direction::LEFT : Direction::RIGHT);
    auto tank = std::make_shared<Tank>(col, row, dir, playerId);
    getCell(col, row).setTank(tank);
    tanks.insert(tank);
}
