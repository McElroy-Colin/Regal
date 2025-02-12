// Header file containing code used in optimization and execution of AST trees.

#include <map>
#include "langdef.hpp"

#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

// TODO: this is bad, dont want to manually update this when I add more actions
std::vector<size_t> number_types = {
    0 /* intContainer */
};

// Namespace for type checking data and utilities.
namespace TypingUtils {
//  Return true if the given nodes are of different types.
//      node1: first code tree to compare (input)
//      node2: second code tree to compare (input)
    bool type_mismatch(const syntaxNode& node1, const syntaxNode& node2) {
        return node1.index() != node2.index();
    }


//  Return true if the given code tree is non of the given types.
//      code_tree: code tree to compare (input)
//      types: types to check the code tree for (input)
    bool incompatible_type(const syntaxNode& code_tree, const std::vector<size_t>& types) {
        for (int i = 0; i < types.size(); i++) {
            if (types[i] == code_tree.index()) {
                return false;
            }
        }
        return true;
    }
}


#endif