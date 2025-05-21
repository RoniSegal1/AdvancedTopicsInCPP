#include "GameCell.h"

/**
 * @brief Resets the content of cell — sets terrain back to empty.
 */
void GameCell::resetContent() {
    if (content == CellContent::Wall) { wallHits = -1; }
    content = CellContent::Empty;
}

 /**
 * @brief Returns how many times a wall was hit (or -1 if not a wall).
 */
int GameCell::getWallHits() const{
    return wallHits;
}

/**
 * @brief Increments the wall hit counter.
 */
void GameCell::incrementWallHits(){
    if (content == CellContent::Wall) {
        ++wallHits;
    }
}

/**
 * @brief Resets the content (turns it into empty cell and clears hits).
 */
void GameCell::resetContent() {
    if (content == CellContent::Wall) {
        wallHits = -1;
    }
    content = CellContent::Empty;
}