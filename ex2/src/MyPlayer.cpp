
#include "MyPlayer.h"
#include "MyBattleInfo.h"

MyPlayer::MyPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : playerIndex(playerIndex), maxSteps(maxSteps), numShells(numShells) {
}

void MyPlayer::addTank(int tankIndex, Tank* tank, std::unique_ptr<TankAlgorithm> algorithm) {
    tanks.push_back({tankIndex, tank, numShells, std::move(algorithm)});
}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm& algorithm, SatelliteView& view) {
    for (auto& tankInfo : tanks) {
        if (tankInfo.algorithm.get() == &algorithm) {
            Tank* tank = tankInfo.tank;

            MyBattleInfo info(
                playerIndex,
                tank->getPosition(),
                tank->getDirection(),
                tank->getRemainingShells(),
                maxSteps,
            );

            algorithm.updateBattleInfo(info);
            return;
        }
    }
    throw std::runtime_error("TankAlgorithm not found in MyPlayer.");
}

TankAlgorithm* MyPlayer::getTankAlgorithm(Tank* tank) const {
    for (const auto& tankInfo : tanks) {
        if (tankInfo.tank == tank)
            return tankInfo.algorithm.get();
    }
    return nullptr;
}
