#pragma once
#include "Player.h"
#include <cstddef>
#include <iostream>

class MyPlayer : public Player {
protected:
    int playerIndex;
    size_t rows;
    size_t cols;
    size_t maxSteps;
    size_t numShells;

public:
    MyPlayer(int player_index, size_t rows, size_t cols, size_t max_steps, size_t num_shells)
    : Player(player_index, rows, cols, max_steps, num_shells),
      playerIndex(player_index), rows(rows), cols(cols), maxSteps(max_steps), numShells(num_shells) 
      {}

    ~MyPlayer() override = default;
    virtual void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override = 0;
};