#pragma once
#include <vector>
#include "Cell.h"

class Board {
private:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;
    std::set<Tank*> tanks;
    std::set<Shell*> shells; // number of shells in Cell

public:
    Board(int width, int height);

    Cell& getCell(int x, int y); //בכוונה עשיתי לא כונסטי כדי לשנות, אפשר לשנות את זה

    int getWidth() const;
    int getHeight() const;

    void addTank(Tank* tank);
    void removeTank(Tank* tank);
    const std::set<Tank*>& getTanks() const;
    void clearTanks();

    void addShell(Shell* shell);
    void removeShell(Shell* shell);
    const std::set<Shell*> getShells() const;
    void clearShells();

    void wrapPosition(int& x, int& y);
};
