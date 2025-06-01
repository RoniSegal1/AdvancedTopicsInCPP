#include "Board.h"
#include "Cell.h"

/**
 * @brief Constructor: initializes the grid with Cell objects at each (x, y) location.
 */
Board::Board(size_t rows, size_t cols) : rows(rows), cols(cols), grid(rows) {
    for (size_t x = 0; x < rows; ++x) {
        grid[x].reserve(cols);
        for (size_t y = 0; y < cols; ++y) {
            grid[x].emplace_back(x, y);
        }
    }
}

/**
 * @brief Returns a const reference to a wrapped cell at (x, y).
 */
const Cell& Board::getCell(int x, int y) const {
    wrapPosition(x, y);
    return grid[x][y];
}

/**
 * @brief Returns a mutable reference to a wrapped cell at (x, y).
 */
Cell& Board::getCell(int x, int y) {
    wrapPosition(x, y);
    return grid[x][y];
}

/**
 * @brief Checks whether a cell is considered empty (not wall or mine).
 */
 bool Board::isEmptyCell(int x, int y) const {
    const Cell& cell = getCell(x, y);
    TerrainType terrain = cell.getTerrain();
    return !(terrain == TerrainType::Wall || terrain == TerrainType::Mine);
}

int Board::getCols() const {
    return cols;
}

int Board::getRows() const {
    return rows;
}

/**
 * @brief Wraps coordinates if they go beyond the board limits.
 */
void Board::wrapPosition(int& x, int& y) const {
    x = (x + rows) % rows;
    y = (y + cols) % cols;
}
