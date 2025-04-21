#pragma once
#include <vector>
#include "Cell.h"
#include "Tank.h"

class Board {
private:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;
    std::set<Tank*> tanks;

public:
    Board(int width, int height);

    Cell& getCell(int x, int y); //בכוונה עשיתי לא כונסטי כדי לשנות, אפשר לשנות את זה

    void update(); // for what?
    int getWidth() const;
    int getHeight() const;


private:
    void wrapPosition(int& x, int& y) const;
    void addWall(int row, int col);
    void addMine(int row, int col);
    void addTank(int row, int col, int playerId);
};
