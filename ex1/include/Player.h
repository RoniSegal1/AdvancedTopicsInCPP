#pragma once
#include <set>
#include "Tank.h"

/**
 * @class Player
 * @brief Represents a player in the game, managing one or more tanks.
 *
 * Currently each player holds one tank, but the design supports future extension to multiple tanks.
 */
class Player {
private:
    int playerId; ///< Player's unique ID (e.g., 1 or 2)
    std::set<Tank*> tanks; ///< Set of tanks belonging to this player

public:
    /**
     * @brief Default constructor.
     */
    Player();

    /**
     * @brief Constructor with player ID.
     * @param id The player's ID
     */
    Player(int id);

    // Prevent copy operations (each player instance should be unique)
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    /**
     * @brief Returns the player's ID.
     */
    int getId() const;

    /**
     * @brief Adds a tank to the player's set.
     * @param tank Pointer to the tank to add
     */
    void addTank(Tank* tank);

    /**
     * @brief Removes a tank from the player's set.
     * @param tank Pointer to the tank to remove
     */
    void removeTank(Tank* tank);

    /**
     * @brief Returns a const reference to the player's tanks.
     */
    const std::set<Tank*>& getTanks() const;

    /**
     * @brief Checks if the player still has active tanks.
     * @return True if at least one tank remains
     */
    bool hasAliveTanks() const;

    /**
     * @brief Clears all tanks associated with the player.
     */
    void clearTanks();
};
