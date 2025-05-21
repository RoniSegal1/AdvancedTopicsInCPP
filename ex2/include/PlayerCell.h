#pragma once
#include "Cell.h"

/**
 * @class PlayerCell
 * @brief Represents a cell from the player's limited point of view.
 */
class PlayerCell : public Cell {

public:
    PlayerCell(int x, int y, CellContent content)
        : Cell(x, y, content) {}

    /**
     * @brief Clears dynamic content (like tank/shells) from the cell.
     */
    void resetContent() override;

    /**
     * @brief For display/debugging
     */
    char getDisplayChar() const;
};
