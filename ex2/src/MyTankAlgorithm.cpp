#include "MyTankAlgorithm.h"
#include "MyBattleInfo.h"



void MyTankAlgorithm::updatePostAction(ActionRequest action) {
    switch (action) {
        case ActionRequest::MoveForward:
            myPosition = moveInDirectionD(myPosition.first, myPosition.second, 1, myDirection);
            break;
        case ActionRequest::MoveBackward:
            myPosition = moveInDirectionD(myPosition.first, myPosition.second, -1, myDirection);
            break;
        case ActionRequest::RotateLeft90:
            myDirection = turnLeftQuarter(myDirection);
            break;
        case ActionRequest::RotateRight90:
            myDirection = turnRightQuarter(myDirection);
            break;
        case ActionRequest::RotateLeft45:
            myDirection = turnLeft(myDirection);
            break;
        case ActionRequest::RotateRight45:
            myDirection = turnRight(myDirection);
            break;
    }
}

void MyTankAlgorithm::updateBoard(BattleInfo& info){
    MyBattleInfo* myinfo = dynamic_cast<MyBattleInfo*>(&info);
    if (!myinfo) return;

    boardRows = myinfo->getBoardRows();
    boardCols = myinfo->getBoardCols();
    board = Board<PlayerCell>(boardRows, boardCols);
    for (int i = 0; i < boardRows; ++i) {
        for (int j = 0; j < boardCols; ++j) {
            CellContent content = myinfo->getCellContent(i, j);
            board.setCell(i, j, PlayerCell(i, j, content));
        }
    }
}

/**
 * @brief Moves one step in the given direction with wrapping.
 */
std::pair<int, int> MyTankAlgorithm::moveInDirectionD(int x, int y, int d, Direction dir) {
    std::pair<int, int> next;
    switch (dir) {
        case Direction::U:         next = { x, y - 1*d }; break;
        case Direction::UR:        next = { x + 1*d, y - 1*d }; break;
        case Direction::R:         next = { x + 1*d, y }; break;
        case Direction::DR:        next = { x + 1*d, y + 1*d }; break;
        case Direction::D:         next = { x, y + 1*d }; break;
        case Direction::DL:        next = { x - 1*d, y + 1*d }; break;
        case Direction::L:         next = { x - 1*d, y }; break;
        case Direction::UL:        next = { x - 1*d, y - 1*d }; break;
    }
    board.wrapPosition(next.first, next.second); // wrap around board edges
    return next;
}


/**
 * @brief Returns the 3x3 neighborhood deltas including center.
 */
const std::vector<std::pair<int, int>> MyTankAlgorithm::getXDeltas(int x) const {
    return {
        {x * -1, x * -1}, {0, x * -1}, {x, x * -1},
        {x * -1, 0},                   {x, 0},
        {x * -1, x},      {0, x},      {x, x}
    };
}

/**
 * @brief Computes all surrounding coordinates (including self).
 */
std::set<std::pair<int, int>> MyTankAlgorithm::doDVicinity(int x, int y, int d) const {
    auto deltas = getXDeltas(d);
    std::set<std::pair<int, int>> vicinity;
    for (const auto& [dx, dy] : deltas) {
        int nx = x + dx;
        int ny = y + dy;
        board.wrapPosition(nx, ny);
        vicinity.insert({nx, ny});
    }
    return vicinity;
}


// basic movement algorithm 
ActionRequest MyTankAlgorithm::getThreatningNextAction(threatingShells) const {
    Direction dir = myDirection;
    auto [dx, dy] = getDelta(dir);
    int x = myPosition.first;
    int y = myPosition.second;
    if (canMoveThere(x+dx, y+dy) && threatingShells.count({x, y}) == 1) {
        return ActionRequest::MoveForward;
    }
    for (int j = 1; j <= 2; j++) {
        int sx = x + j * dx;
        int sy = y + j * dy;
        board.wrapPosition(sx, sy);
        const PlayerCell& cell = board.getCell(sx, sy);
        if (cell.getContent() == CellContent::EnemyTank) {
            return ActionRequest::Shoot;
        }
    }
    return ActionRequest::DoNothing;
}

bool MyTankAlgorithm::canMoveThere(int sx, int sy) const {
    if (board.getWidth() == 0 || board.getHeight() == 0) return false;
    const PlayerCell& neighbor = board.getCell(sx, sy);
    return neighbor.getContent() == CellContent::Empty;
}

std::set<std::pair<int, int>> MyTankAlgorithm::getCurrThreatShells() {
    std::set<std::pair<int, int>> threatShells;
    for (const auto& [x, y] : shellsPositions) {
        // Calculate the number of steps based on turns since last update for both blocks the shell move in a single turn
        int stepsSecondCell = 2 * (turnsSinceLastUpdate + 1); 
        int stepsFirstCell = stepsSecondCell - 1; // first cell is one step behind the second cell
        auto currScaryStepOne = doDVicinity(x, y, stepsFirstCell);
        auto currScaryStepTwo = doDVicinity(x, y, stepsSecondCell);
        threatShells.insert(threatShells.end(), currScaryStepOne.begin(), currScaryStepOne.end());
        threatShells.insert(threatShells.end(), currScaryStepTwo.begin(), currScaryStepTwo.end());
    }
}