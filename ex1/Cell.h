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

public:
    Cell(int x, int y);

    std::pair<int, int> getPoint() const;

    TerrainType getTerrain() const;
    void setTerrain(TerrainType t);

    int getWallHits() const;
    void incrementWallHits();
    void resetWall();
};
