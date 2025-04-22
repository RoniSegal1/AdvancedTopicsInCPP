#pragma once
#include <set>
#include "Tank.h"

class Player {
private:
    int playerId;
    std::set<Tank*> tanks; // One tank for now, multiple in future

public:
    Player();
    Player(int id);

    int getId() const;

    void addTank(Tank* tank);
    void removeTank(Tank* tank);
    const std::set<Tank*>& getTanks() const;

    bool hasAliveTanks() const;
};
