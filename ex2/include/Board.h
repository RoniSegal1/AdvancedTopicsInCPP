#pragma once
#include <vector>
#include <memory>
#include "Cell.h"

/**
 * @class Board
 * @brief Represents the game board as a 2D grid of cells.
 * 
 * The Board stores a grid of Cell objects, each representing a tile on the game board.
 * It handles access to cells, wrapping logic, and checks for empty spaces.
 */
class Board {
private:
    int width;  ///< Width of the board
    int height;  ///< Width of the board
    std::vector<std::vector<Cell>> grid; ///< 2D grid of Cell objects

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
    const Cell& getCell(int x, int y) const;

    /**
     * @brief Returns a mutable reference to a cell at given coordinates (with wrapping).
     */
    Cell& getCell(int x, int y);

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
