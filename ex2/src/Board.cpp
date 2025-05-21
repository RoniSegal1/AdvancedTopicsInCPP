#include "Board.h"


template <typename T>
/**
 * @brief Constructor: initializes the grid with Cell objects at each (x, y) location.
 */
Board<T>::Board(size_t w, size_t h) : width(w), height(h), grid(h) {
    for (int y = 0; y < h; ++y) {
        grid[y].reserve(w);
        for (int x = 0; x < w; ++x) {
            grid[y].emplace_back(std::make_unique<T>(x, y, CellContent::Empty));
        }
    }
}

/**
 * @brief Returns a const reference to a wrapped cell at (x, y).
 */
template <typename T>
const T& Board<T>::getCell(int x, int y) const {
    wrapPosition(x, y);
    return grid[y][x];
}

/**
 * @brief Returns a mutable reference to a wrapped cell at (x, y).
 */
template <typename T>
T& Board<T>::getCell(int x, int y) {
    wrapPosition(x, y);
    return grid[y][x];
}

/**
 * @brief Checks whether a cell is considered empty (not wall or mine).
 */
template <typename T>
bool Board<T>::isEmptyCell(int x, int y) const {
    const T& cell = getCell(x, y);
    return cell.getContent() != CellContent::Wall && cell.getContent() != CellContent::Mine;
}

template <typename T>
int Board<T>::getWidth() const {
    return width;
}

template <typename T>
int Board<T>::getHeight() const {
    return height;
}

/**
 * @brief Wraps coordinates if they go beyond the board limits.
 */
template <typename T>
void Board<T>::wrapPosition(int& x, int& y) const {
    x = (x + width) % width;
    y = (y + height) % height;
}
