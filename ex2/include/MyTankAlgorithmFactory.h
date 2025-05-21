#pragma once

#include "common/TankAlgorithmFactory.h"
#include "common/TankAlgorithm.h"
#include <memory>

class MyTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
        return std::make_unique<MyTankAlgorithm>(player_index, tank_index);
    }
};
