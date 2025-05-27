#include "BasicTankAlgorithm.h"

BasicTankAlgorithm::BasicTankAlgorithm(int playerIndex, int tankIndex)
    : MyTankAlgorithm(playerIndex, tankIndex) {}


void BasicTankAlgorithm::updateBattleInfo(BattleInfo& info){
    myinfo = static_cast<MyBattleInfo&>(info);
    myPosition = myinfo.getMyPosition();
    shellsPositions = myinfo.getShellsPositions();
    board = updateBoard(info);
}

ActionRequest BasicTankAlgorithm::getAction() {
    if (board.isEmpty()) { // don't have any information about the board, have to get it
        return shouldGetBattleInfo();
    }

    auto threatingShells = getCurrThreatShells();
    ActionRequest action = getThreatningNextAction(threatingShells);
    if (action == ActionRequest::DoNothing && turnsSinceLastUpdate >= 4) {
        return shouldGetBattleInfo();
    }

    ActionRequest action = getScaryNextAction(threatingShells);
    if (action == ActionRequest::DoNothing) {
        return shouldGetBattleInfo();
    }

    turnsSinceLastUpdate++;
    return action;
}






// fix scary next action
ActionRequest BasicTankAlgorithm::getScaryNextAction(threatingShells) const {
    auto [dx, dy] = getDelta(mydirection);
    for (int j = 3; j <= 6; j++) {
        sx = x + j*dx;
        sy = y + J*dy;
        lastBoard->wrapPosition(sx, sy);
        const PlayerCell& cell = lastBoard->getCell(sx, sy);
        if (cell.getContent() == CellContent::EnemyTank) {
            return ActionRequest::Shoot;
        }
    }
    forward = 0;
    backward = 0;
    for (int j = 0; j <= 4; j++) {
        sx = x + j*dx;
        sy = y + J*dy;
        mx = x - j*dx;
        my = y - J*dy;
        lastBoard->wrapPosition(sx, sy);
        const PlayerCell& cellF = lastBoard->getCell(sx, sy);
        const PlayerCell& cellB = lastBoard->getCell(mx, my);
        if (threatingShells.count({sx, sy}) == 1) {
            return forward == false;
        }
    }
    return ActionRequest::DoNothing;
}



ActionRequest BasicTankAlgorithm::getScaryNextAction() const {
    auto [x, y] = position;
    std::map<Direction, int> shellCount;
    std::vector<Direction> directions = {
        Direction::U, Direction::UR, Direction::R, Direction::DR,
        Direction::D, Direction::DL, Direction::L, Direction::UL
    };

    for (Direction d : directions) {
        int count = 0;
        auto [dx, dy] = getDelta(d);
        for (int i = 1; i <= 4; ++i) {
            int sx = x + i * dx;
            int sy = y + i * dy;
            board.wrapPosition(sx, sy);
            const PlayerCell& cell = board.getCell(sx, sy);
            if (cell.getContent() == CellContent::Shell) {
                count++;
            }
        }
        shellCount[d] = count;
    }

    Direction minDir = mydirection;
    int minShells = 5;
    for (const auto& [dir, count] : shellCount) {
        if (count < minShells) {
            minShells = count;
            minDir = dir;
        }
    }

    // נסה לנוע קדימה או אחורה אם אפשר לכיוון בטוח
    auto [fx, fy] = getNextPosition(position, mydirection);
    board.wrapPosition(fx, fy);
    const PlayerCell& frontCell = board.getCell(fx, fy);
    if (frontCell.getContent() == CellContent::Empty && shellCount[mydirection] == 0) {
        return ActionRequest::MoveForward;
    }

    auto [bx, by] = getPrevPosition(position, mydirection);
    board.wrapPosition(bx, by);
    const PlayerCell& backCell = board.getCell(bx, by);
    if (backCell.getContent() == CellContent::Empty && shellCount[oppositeDirection(mydirection)] == 0) {
        return ActionRequest::MoveBackward;
    }

    if (minDir != mydirection) {
        int diff = (8 + static_cast<int>(minDir) - static_cast<int>(mydirection)) % 8;
        if (diff == 1) return ActionRequest::RotateRight45;
        if (diff == 2) return ActionRequest::RotateRight90;
        if (diff == 7) return ActionRequest::RotateLeft45;
        if (diff == 6) return ActionRequest::RotateLeft90;
    }

    return ActionRequest::DoNothing;
}












ActionRequest BasicTankAlgorithm::shouldGetBattleInfo() {
    turnsSinceLastUpdate = 1;
    return ActionRequest::GetBattleInfo;
}

bool BasicTankAlgorithm::isEmpty() const {
    return width == 0 || height == 0;
}