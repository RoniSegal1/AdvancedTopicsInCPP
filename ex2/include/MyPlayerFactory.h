#pragma once
#include "PlayerFactory.h"
#include "AggressivePlayer.h"
#include "DefensivePlayer.h"
#include <memory>
#include <iostream> // נדרש בשביל std::cerr

class MyPlayerFactory : public PlayerFactory {
public:
    std::unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                   size_t max_steps, size_t num_shells) const override {
        if (player_index == 1) {
            auto player = std::make_unique<DefensivePlayer>(player_index, x, y, max_steps, num_shells);
            return player;
        } else {
            auto player = std::make_unique<AggressivePlayer>(player_index, x, y, max_steps, num_shells);
            return player;
        }
    }

    ~MyPlayerFactory() override = default;
};
