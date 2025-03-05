// Header file containing code used in optimization and execution of AST trees.

#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <list>
#include <map>
#include <vector>
#include <tuple>
#include "inc_debug/error_handling.hpp"
#include "inc_debug/compiler_debug.hpp"


// Namespace for structures involving interpreted data storage.
namespace DataStorage {
    struct variableInfo {
        TypingUtils::dataType type;
        std::shared_ptr<CodeTree::valueData> value;
        bool optimize_value;
    };

//  Stack structure to store variables in distinct scopes.
    class stack {
        public:
            std::map<std::string, variableInfo> locals;
            std::vector<std::shared_ptr<stack>> inner_scopes;
            std::shared_ptr<stack> parent_scope;

            stack();
            stack(std::shared_ptr<stack> parent);
    };
}

// Optimize and typecheck a given code tree down to its necessary runtime components, including variable assignment. 
// Update the given code tree to a presumably smaller tree structure.
// Return true if the code tree was optimized, meaning the given code tree was known at before runtime.
//      data_node: code tree to be optimized (input/output)
//      scope_stack: stack containing initialized variables (input/output)
//      update_stacks: true if variables should be updated in the current scope and parent scopes (input)
bool optimize_typecheck_data_node(std::shared_ptr<CodeTree::dataNode>& data_node, std::shared_ptr<DataStorage::stack>& scope_stack, const bool update_stacks);

// Optimize and typecheck a given value tree down to its necessary runtime components, including variable assignment. 
// Update the given value tree to a presumably smaller tree structure.
// Return a pair that contains a boolean representing whether the given tree was optimized and a dataType representing the type of the given value tree.
//      value_data: code tree to be optimized (input/output)
//      scope_stack: stack containing initialized variables (input/output)
std::pair<bool, TypingUtils::dataType> optimize_typecheck_value_data(std::shared_ptr<CodeTree::valueData>& value_data, std::shared_ptr<DataStorage::stack>& scope_stack);

#endif
