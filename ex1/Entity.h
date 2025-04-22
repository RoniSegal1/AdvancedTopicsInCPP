#pragma once

#include <utility>

class Entity {
public:
    // Pure virtual function to get current position
    virtual std::pair<int, int> getPosition() const = 0;

    // Virtual destructor to ensure proper cleanup
    virtual ~Entity() = default;
};
