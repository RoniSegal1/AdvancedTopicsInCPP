#pragma once

#include "common/PlayerFactory.h"
#include "MyPlayer.h"
#include <memory>

class MyPlayerFactory : public PlayerFactory {
public:
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                   size_t max_steps, size_t num_shells) const override {
        return std::make_unique<MyPlayer>(player_index, x, y, max_steps, num_shells);
    }
};
