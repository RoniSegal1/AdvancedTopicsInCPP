#pragma once

class MyTankAlgorithm : public TankAlgorithm {
private:
    

public:
    MyTankAlgorithm(int playerIndex, int tankIndex);

    virtual ActionRequest getAction() = 0;
	virtual void updateBattleInfo(BattleInfo& info) = 0;
};