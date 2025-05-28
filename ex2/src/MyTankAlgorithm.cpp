#include "MyTankAlgorithm.h"

MyTankAlgorithm::MyTankAlgorithm(int playerIndex, int tankIndex)
    : playerIndex(playerIndex),
      tankIndex(tankIndex),
      myPosition({-1, -1}),
      myDirection(playerIndex == 1 ? Direction::L : Direction::R),
      turnsSinceLastUpdate(0) // TODO: check if needed here. only Basic uses?
{
    // אפשרות לאתחול עתידי נוסף אם צריך
}


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

void MyTankAlgorithm::updateGrid(BattleInfo& info){
    // TODO: לעדכן אחרי בניית באטל אינפו
    // לעדכן גם רוחב ואורך
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
    wrapPosition(next.first, next.second); // wrap around edges
    return next;
}


/**
 * @brief Core logic deciding whether to move, shoot, or wait.
 * 
 * First checks if the tank is standing in a danger zone.
 * If yes, attempts to move to a safer location.
 * Otherwise, considers shooting if an enemy is in sight.
 */
ActionRequest MyTankAlgorithm::getThreatningNextAction(std::set<std::pair<int, int>> threatPlaces) {
    auto act = ActionRequest::DoNothing;

    int x = myPosition.first;
    int y = myPosition.second;

    // If we're currently standing in a danger zone, try to escape
    if (threatPlaces.count({x, y}) == 1) {
        act = MoveTankFromDanger(threatPlaces);
    }

    // If we chose to stay, maybe we can shoot instead
    if (act == ActionRequest::DoNothing) {
        act = CheckIfINeedToShootX(2);
    }

    return act;
}

/**
 * @brief Attempts to move the tank away from danger, either forward or backward.
 */
ActionRequest MyTankAlgorithm::MoveTankFromDanger(std::set<std::pair<int, int>> threatPlaces) {
    Direction dir = myDirection;
    auto [dx, dy] = getDelta(dir);
    int x = myPosition.first;
    int y = myPosition.second;

    // Try moving forward if the next cell is safe
    int newX = x + dx;
    int newY = x + dy;
    if (threatPlaces.count({newX, newY}) == 0) {
        return ActionRequest::MoveForward;
    } else {
        // If not, try moving backward
        int oldX = x - dx;
        int oldY = x - dy;
        if (threatPlaces.count({oldX, oldY}) == 0) {
            return ActionRequest::MoveBackward;
        }
    }
    return ActionRequest::DoNothing;
}

/**
 * @brief Checks if the tank should fire at an enemy tank in its line of sight.
 */
ActionRequest MyTankAlgorithm::CheckIfINeedToShootX(int x) {
    Direction dir = myDirection;
    auto [dx, dy] = getDelta(dir);
    int newX = myPosition.first;
    int newY = myPosition.second;

    // Look ahead up to x steps
    for (int step = 1; step <= x; ++step) {
        newX += dx;
        newY += dy;
        wrapPosition(newX, newY);
        auto content = grid[ny][nx];
        // If we hit a wall, line of sight is blocked
        if (cell.getContent() == CellContent::Wall) {
            break;
        }
        if (cell.getContent() == CellContent::EnemyTank) {
            ActionRequest::Shoot;
        }
    }
    return ActionRequest::DoNothing;
}

std::set<std::pair<int, int>> MyTankAlgorithm::getThreatsAroundMe(){
    auto shells = getCurrThreatShells();
    auto cells = getThreatCellsAroundMe();
    shells.insert(cells.begin(), cells.end());
    return shells;
}

std::set<std::pair<int, int>> MyTankAlgorithm::getThreatCellsAroundMe() const {
    int x = myPosition.first;
    int y = myPosition.second;
    std::set<std::pair<int, int>> threatPlaces;
    auto vicinity = doDVicinity(x, y, 1);
    for (const auto& pos : vicinity) {
        int nx = pos.first;
        int ny = pos.second;
        wrapPosition(nx, ny);
        auto content = grid[ny][nx]
        if (content == ObjectType::Wall || content == ObjectType::Mine || content == ObjectType::AllyTank || content == ObjectType::EnemyTank) {
            threatPlaces.insert({nx, ny});
        }
    }
    return threatPlaces;
}

std::set<std::pair<int, int>> MyTankAlgorithm::getCurrThreatShells() {
    std::set<std::pair<int, int>> threatShells;
    for (const auto& [x, y] : shellsPositions) {
        // Calculate the number of steps based on turns since last update for both blocks the shell move in a single turn
        int stepsSecondCell = 2 * (turnsSinceLastUpdate + 1); 
        int stepsFirstCell = stepsSecondCell - 1; // first cell is one step behind the second cell
        auto currScaryStepOne = doDVicinity(x, y, stepsFirstCell);
        auto currScaryStepTwo = doDVicinity(x, y, stepsSecondCell);
        threatShells.insert(currScaryStepOne.begin(), currScaryStepOne.end());
        threatShells.insert(currScaryStepTwo.begin(), currScaryStepTwo.end());
    }
    return threatShells;
}


/**
 * @brief Returns the 3x3 neighborhood deltas including center.
 */
std::vector<std::pair<int, int>> MyTankAlgorithm::getXDeltas(int x) const {
    return {
        {x * -1, x * -1}, {0, x * -1}, {x, x * -1},
        {x * -1, 0},                   {x, 0},
        {x * -1, x},      {0, x},      {x, x}
    };
}

/**
 * @brief Computes all surrounding coordinates (including self).
 */
std::set<std::pair<int, int>> MyTankAlgorithm::doDVicinity(int x, int y, int d) const{
    auto deltas = getXDeltas(d);
    std::set<std::pair<int, int>> vicinity;
    for (const auto& [dx, dy] : deltas) {
        int nx = x + dx;
        int ny = y + dy;
        wrapPosition(nx, ny);
        vicinity.insert({nx, ny});
    }
    return vicinity;
}

/**
 * @brief Wraps coordinates if they go beyond the limits.
 */
void MyTankAlgorithm::wrapPosition(int& x, int& y) const {
    x = (x + width) % width;
    y = (y + height) % height;
}