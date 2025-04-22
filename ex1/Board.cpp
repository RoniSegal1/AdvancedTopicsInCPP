#include "Board.h"
#include "Cell.h"

Board::Board(int w, int h) : width(w), height(h), grid(h) {
    for (int y = 0; y < h; ++y) {
        grid[y].reserve(w);
        for (int x = 0; x < w; ++x) {
            grid[y].emplace_back(x, y);
        }
    }
}

Cell& Board::getCell(int x, int y) { // needed bhirat mehdal of cell
    wrapPosition(x, y);
    return grid[y][x];
}

int Board::getWidth() const {
  return width;
}

int Board::getHeight() const {
  return height;
}

void Board::addTank(Tank* tank) {
    tanks.insert(tank);
}

void Board::removeTank(Tank* tank) {
    tanks.erase(tank);
}

const std::set<Tank*>& Board::getTanks() const {
    return tanks;
}

void Board::clearTanks() {
    tanks.clear();
}

void Board::addShell(Shell* shell) {
    shells.insert(shell);
}

void Board::removeShell(Shell* shell) {
    shells.erase(shell);
}

const std::set<Shell*> Board::getShells() const {
    return shells;
}

void Board::clearShells() {
    shells.clear();
}

void Board::wrapPosition(int& x, int& y) { //check if needed
    x = (x + width) % width;
    y = (y + height) % height;
}
