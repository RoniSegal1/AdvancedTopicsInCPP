#pragma once
#include "Cell.h"

/**
 * @class GameCell
 * @brief Represents a single tile on the game board.
 *
 * Each cell stores its position, terrain type, and wall damage state.
 */
class GameCell: public Cell {
private:
    int wallHits; ///< Number of hits taken by wall (-1 if not a wall)

public:

    GameCell(int x, int y, CellContent content)
        : Cell(x, y, content),
          wallHits(content == CellContent::Wall ? 0 : -1) {}
        
    
    /**
     * @brief Returns how many times a wall was hit (or -1 if not a wall).
     */
    int getWallHits() const;

    /**
     * @brief Increments the wall hit counter.
     */
    void incrementWallHits();

    /**
     * @brief Resets the wall (turns it into empty cell and clears hits).
     */
    void resetContent() override;
};