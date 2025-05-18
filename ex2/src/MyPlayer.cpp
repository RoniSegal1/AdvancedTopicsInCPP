#include "MyPlayer.h"
#include "MyBattleInfo.h"
#include <stdexcept>

MyPlayer::MyPlayer(int playerIndex, size_t x, size_t y, size_t maxSteps, size_t numShells)
    : playerIndex(playerIndex), maxSteps(maxSteps), numShells(numShells) , x(x), y(y) {}

void MyPlayer::registerTank(int tankIndex) {
    tanks.push_back({tankIndex, {0, 0}, Direction::Up, numShells, true});
}

void MyPlayer::updateTankInfo(int tankIndex, const std::pair<int, int>& pos,
                               Direction dir, int shells) {
    for (auto& tank : tanks) {
        if (tank.tankIndex == tankIndex) {
            tank.lastKnownPosition = pos;
            tank.direction = dir;
            tank.remainingShells = shells;
            return;
        }
    }
    throw std::runtime_error("Tank index not found in MyPlayer.");
}

void MyPlayer::updateTankWithBattleInfo(TankAlgorithm& algorithm, SatelliteView& view) {
    // ה-BattleInfo ייאסף מהטנק דרך GameManager, אז כאן אין עדכון
    // זו קריאה חד-פעמית של השחקן לעדכן את האלגוריתם
    // אין לנו דרך חוקית לזהות את הטנק המתאים מתוך הפונקציה הזאת לפי הכללים

    // לכן, בשלב הזה, נשאיר את הפונקציה ריקה (אם לא נשתמש בה בפועל)
    (void)algorithm;
    (void)view;
}
