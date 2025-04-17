#pragma once
#include <set>
#include <utility>

class Tank; // forward declaration
class Shell; // forward declaration

enum class TerrainType {
    Empty,
    Wall,
    Mine
};

class Cell {
private:
    int x;
    int y;
    TerrainType terrain;
    int wallHits; // -1 if no wall
    std::set<Tank*> tanks;
    std::set<Shell*> shells; // number of shells in Cell

public:
    Cell(int x, int y);

    std::pair<int, int> getPoint() const;

    TerrainType getTerrain() const;
    void setTerrain(TerrainType t);

    int getWallHits() const;
    void incrementWallHits();
    void resetWall();

    void addTank(Tank* tank);
    void removeTank(Tank* tank);
    const std::set<Tank*>& getTanks() const;
    void clearTanks();

    void addShell(Shell* shell);
    void removeShell(Shell* shell);
    const std::set<Shell*> getShells() const;
    void clearShells();
};
