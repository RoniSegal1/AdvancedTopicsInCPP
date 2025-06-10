#include "BasicTankAlgorithm.h"
#include "TankAlgorithm.h"
#include <iostream>

BasicTankAlgorithm::BasicTankAlgorithm(int playerIndex, int tankIndex)
    : MyTankAlgorithm(playerIndex, tankIndex) {}

    
void BasicTankAlgorithm::updateBattleInfo(BattleInfo& info) {
    auto myinfo = static_cast<MyBattleInfo&>(info);
    myPosition = myinfo.getMyPosition();
    shellsPositions = myinfo.getShellsPositions();
    updateGrid(info);
}

/**
 * @brief Main method: delegates to basicAlgorithm and rotation if needed.
 */
ActionRequest BasicTankAlgorithm::getAction() {

    if (isEmpty()) { // don't have any information about the grid, have to get it
        return shouldGetBattleInfo();
    }
    
    auto threatPlaces = getThreatsAroundMe();
    auto action = getThreatningNextAction(threatPlaces); //  check if i am threatend

    // If there is no threat, get battle info (if i haven't gotten it in a while)
    if (action == ActionRequest::DoNothing) {
        if (turnsSinceLastUpdate >= 4) {
            return shouldGetBattleInfo();
        }

        else {
            // else - try to rotate or still get battle info
            action = getScaryNextAction(threatPlaces);
            if (action == ActionRequest::DoNothing) {
                return shouldGetBattleInfo();
            }
        }
    }

    turnsSinceLastUpdate++;
    updatePostAction(action);
    return action;
}


ActionRequest BasicTankAlgorithm::getScaryNextAction(std::set<std::pair<int, int>> threatPlaces) {
    auto act = ActionRequest::DoNothing;

    auto threatTanks = getThreatTanksAroundMe();
    act = rotateToShootTank(threatTanks);
    if (act != ActionRequest::DoNothing) {
        return act; // If we can shoot a tank, do it
    }
    // check maybe i can turn to a safe place
    act = rotateToFreeCell(threatPlaces);

    // If we chose to stay, maybe we can shoot instead
    if (act == ActionRequest::DoNothing) {
        act = CheckIfINeedToShootX(6);
    }

    return act;
}

/**
 * @brief Rotates the tank toward a free adjacent cell, if any exist.
 * 
 * Tries small rotations first (1/8), then larger ones (1/4).
 */
ActionRequest BasicTankAlgorithm::rotateToFreeCell(std::set<std::pair<int, int>> threatPlaces) {
    // Try all four rotation options
    std::vector<std::pair<ActionRequest, Direction>> rotations = {
        {ActionRequest::RotateLeft45, static_cast<Direction>((static_cast<int>(myDirection) + 7) % 8)},
        {ActionRequest::RotateRight45, static_cast<Direction>((static_cast<int>(myDirection) + 1) % 8)},
        {ActionRequest::RotateLeft90, static_cast<Direction>((static_cast<int>(myDirection) + 6) % 8)},
        {ActionRequest::RotateRight90, static_cast<Direction>((static_cast<int>(myDirection) + 2) % 8)},
    };
    int x = myPosition.first;
    int y = myPosition.second;
    
    // Check which direction leads to a safe cell
    for (const auto& [action, newDir] : rotations) {
        auto delta = getDelta(newDir);
        int newX = x + delta.first;
        int newY = y + delta.second;
        wrapPosition(newX, newY);
        if (threatPlaces.count({newX, newY}) == 0) {
            return action;
        }
    }

    return ActionRequest::DoNothing;
}

/**
 * @brief Rotates the tank toward a tank see if i can shoot him.
 * 
 * Tries small rotations first (1/8), then larger ones (1/4).
 */
ActionRequest BasicTankAlgorithm::rotateToShootTank(std::set<std::pair<int, int>> threatTanks) {
    // Try all four rotation options
    std::vector<std::pair<ActionRequest, Direction>> rotations = {
        {ActionRequest::RotateLeft45, static_cast<Direction>((static_cast<int>(myDirection) + 7) % 8)},
        {ActionRequest::RotateRight45, static_cast<Direction>((static_cast<int>(myDirection) + 1) % 8)},
        {ActionRequest::RotateLeft90, static_cast<Direction>((static_cast<int>(myDirection) + 6) % 8)},
        {ActionRequest::RotateRight90, static_cast<Direction>((static_cast<int>(myDirection) + 2) % 8)},
    };
    int x = myPosition.first;
    int y = myPosition.second;
    
    // Check which direction leads to a safe cell
    for (const auto& [action, newDir] : rotations) {
        auto delta = getDelta(newDir);
        int newX = x + delta.first;
        int newY = y + delta.second;
        wrapPosition(newX, newY);
        if (threatTanks.count({newX, newY}) == 0) {
            return action;
        }
    }

    return ActionRequest::DoNothing;
}

ActionRequest BasicTankAlgorithm::shouldGetBattleInfo() {
    turnsSinceLastUpdate = 1;
    updatePostAction(ActionRequest::GetBattleInfo);
    return ActionRequest::GetBattleInfo;
}

bool BasicTankAlgorithm::isEmpty() const {
    for (const auto& row : grid) {
        for (ObjectType obj : row) {
            if (obj != ObjectType::Empty) {
                return false;
            }
        }
    }
    return true;
}
