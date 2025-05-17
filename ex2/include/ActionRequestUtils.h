#ifndef ACTION_REQUEST_UTILS_H
#define ACTION_REQUEST_UTILS_H

#include <string>
#include "ActionRequest.h" 

inline std::string toString(ActionRequest action) {
    switch (action) {
        case ActionRequest::MoveForward:     return "MoveForward";
        case ActionRequest::MoveBackward:    return "MoveBackward";
        case ActionRequest::RotateLeft90:    return "RotateLeft90";
        case ActionRequest::RotateRight90:   return "RotateRight90";
        case ActionRequest::RotateLeft45:    return "RotateLeft45";
        case ActionRequest::RotateRight45:   return "RotateRight45";
        case ActionRequest::Shoot:           return "Shoot";
        case ActionRequest::GetBattleInfo:   return "GetBattleInfo";
        case ActionRequest::DoNothing:       return "DoNothing";
        default:                             return "Unknown";
    }
}

#endif 
