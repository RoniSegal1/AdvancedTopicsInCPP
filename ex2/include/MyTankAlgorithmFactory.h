#pragma once

#include "common/TankAlgorithmFactory.h"
#include "common/TankAlgorithm.h"
#include "MyTankAlgorithm.h"
#include "BasicTankAlgorithm.h"
#include "ChasingTankAlgorithm.h"
#include <memory>

class MyTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    // std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
    //     if (player_index == 1){
    //         return std::make_unique<BasicTankAlgorithm>(player_index, tank_index);
    //     }
    //     if (player_index == 2){
    //         return std::make_unique<ChasingTankAlgorithm>(player_index, tank_index);
    //     }
    //     return nullptr;
    // }
    std::unique_ptr<TankAlgorithm> create(int player_index, int tank_index) const override {
        return std::make_unique<MyTankAlgorithm>(player_index, tank_index);
    }
};
