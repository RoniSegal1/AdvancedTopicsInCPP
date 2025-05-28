#include "Board.h"
#include "Cell.h"

/**
 * @brief Constructor: initializes the grid with Cell objects at each (x, y) location.
 */
Board::Board(size_t w, size_t h) : width(w), height(h), grid(h) {
    for (int y = 0; y < h; ++y) {
        grid[y].reserve(w);
        for (int x = 0; x < w; ++x) {
            grid[y].emplace_back(x, y);
        }
    }
}

/**
 * @brief Returns a const reference to a wrapped cell at (x, y).
 */
const Cell& Board::getCell(int x, int y) const {
    wrapPosition(x, y);
    return grid[y][x];
}

/**
 * @brief Returns a mutable reference to a wrapped cell at (x, y).
 */
Cell& Board::getCell(int x, int y) {
    wrapPosition(x, y);
    return grid[y][x];
}

/**
 * @brief Checks whether a cell is considered empty (not wall or mine).
 */
 bool Board::isEmptyCell(int x, int y) const {
    const Cell& cell = getCell(x, y);
    TerrainType terrain = cell.getTerrain();
    return !(terrain == TerrainType::Wall || terrain == TerrainType::Mine);
}

int Board::getWidth() const {
    return width;
}

int Board::getHeight() const {
    return height;
}

/**
 * @brief Wraps coordinates if they go beyond the board limits.
 */
void Board::wrapPosition(int& x, int& y) const {
    x = (x + width) % width;
    y = (y + height) % height;
}
