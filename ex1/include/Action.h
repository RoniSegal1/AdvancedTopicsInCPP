#pragma once
#include <string>

/**
 * @enum Action
 * @brief Represents all possible actions a tank can perform during the game.
 * 
 * Notes on behavior and restrictions:
 * - MoveForward: Executed immediately in one game step.
 * - MoveBackward: Requires two idle steps, then moves on the third step.
 *     - Additional back requests during the delay are ignored.
 *     - Can be canceled by a forward request (tank stays in place).
 * - Rotations (1/8 or 1/4 turn): Executed in one game step.
 * - Shoot: Executed immediately, but triggers a cooldown of 4 turns before shooting again.
 * - Wait: No action is performed this turn.
 */
enum class Action {
    MoveForward,
    MoveBackward,
    RotateLeft1_8,
    RotateRight1_8,
    RotateLeft1_4,
    RotateRight1_4,
    Shoot,
    Wait
};

/**
 * @brief Converts an Action enum to its string representation.
 * @param action The action to convert.
 * @return The name of the action as a string.
 */
inline std::string actionToString(Action action) {
    switch (action) {
        case Action::MoveForward: return "MoveForward";
        case Action::MoveBackward: return "MoveBackward";
        case Action::RotateLeft1_8: return "RotateLeft1_8";
        case Action::RotateRight1_8: return "RotateRight1_8";
        case Action::RotateLeft1_4: return "RotateLeft1_4";
        case Action::RotateRight1_4: return "RotateRight1_4";
        case Action::Shoot: return "Shoot";
        case Action::Wait: return "Wait";
        default: return "UnknownAction";
    }
}
