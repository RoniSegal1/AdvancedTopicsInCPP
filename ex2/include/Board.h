#pragma once
#include <vector>
#include <cstddef>
#include "Cell.h"

/**
 * @class Board<T>
 * @brief Generic 2D board that can store any type of Cell (e.g., GameCell, PlayerCell).
 */
template <typename T>
class Board {
private:
    size_t width;  ///< Width of the board
    size_t height; ///< Height of the board
    std::vector<std::vector<std::unique_ptr<T>>> grid; ///< 2D grid of Cell objects

public:
    /**
     * @brief Constructs a board with given dimensions.
     * @param width Number of columns
     * @param height Number of rows
     */
    Board(size_t width, size_t height);

    // Prevent copying
    Board(const Board&) = delete;

    /**
     * @brief Returns a const reference to a cell at given coordinates (with wrapping).
     */
    const T& getCell(int x, int y) const;

    /**
     * @brief Returns a mutable reference to a cell at given coordinates (with wrapping).
     */
    T& getCell(int x, int y);

    /**
     * @brief Checks if a cell at (x, y) is empty (not a wall or mine).
     */
    bool isEmptyCell(int x, int y) const;

    int getWidth() const;
    int getHeight() const;

    /**
     * @brief Wraps coordinates around board edges (torus-like behavior).
     *        Useful for supporting movement beyond edges.
     */
    void wrapPosition(int& x, int& y) const;
};