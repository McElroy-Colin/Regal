// Header file containing code used in optimization and execution of AST trees.

#include <map>
#include "langdef.hpp"

#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

// TODO: this is bad, dont want to manually update this when I add more actions
std::vector<size_t> number_types = {
    0 /* intContainer */
};


//  Return true if the given actions are of different types.
//      action1: first code_tree to compare (input)
//      action2: second code_tree to compare (input)
bool type_mismatch(const syntaxNode& action1, const syntaxNode& action2) {
    return action1.index() != action2.index();
}


//  Return true if the given code_tree is non of the given types.
//      code_tree: code_tree to compare (input)
//      types: types to check the code_tree for (input)
bool incompatible_type(const syntaxNode& code_tree, const std::vector<size_t>& types) {
    for (int i = 0; i < types.size(); i++) {
        if (types[i] == code_tree.index()) {
            return false;
        }
    }
    return true;
}


#endif