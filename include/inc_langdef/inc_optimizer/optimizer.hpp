#include <variant>
#include <vector>
#include <string>
#include <map>
#include "../langdef.hpp"

#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

// TODO: this is bad, dont want to manually update this when i add more actions
std::vector<size_t> number_types = {
    0 /* Integer */
};


//  Return true if the given actions are of different types.
//      action1: first action to compare (input)
//      action2: second action to compare (input)
bool type_mismatch(const Action& action1, const Action& action2) {
    return action1.index() != action2.index();
}


//  Return true if the given action is non of the given types.
//      action: action to compare (input)
//      types: types to check the action for (input)
bool incompatible_type(const Action& action, const std::vector<size_t>& types) {
    for (int i = 0; i < types.size(); i++) {
        if (types[i] == action.index()) {
            return false;
        }
    }
    return true;
}


#endif