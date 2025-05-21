#pragma once
#include <utility>

/**
 * @enum CellContent
 * @brief Represents the content of a cell.
 */
enum class CellContent {
    Empty,  ///< No obstacle
    Wall,   ///< A wall that can be hit and destroyed
    Mine,    ///< A mine that explodes on tank entry
    MyTank,
    EnemyTank,
    Shell
};

/**
 * @class Cell
 * @brief Common interface for both GameManager and Player board cells.
 */
class Cell {
protected:
    int x;
    int y;
    CellContent content;

public:
    Cell(int x, int y, CellContent content) : x(x), y(y), content(content) {}

    std::pair<int, int> getPoint() const { return {x, y}; }
    int getX() const { return x; }
    int getY() const { return y; }

    CellContent getContent() const {
        return content;
    }

    void setContent(CellContent c) {
        content = c;
    }

    virtual void resetContent() = 0;
};