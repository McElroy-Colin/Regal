/*

Structures and function declarations for pre-runtime code optimization and typechecking.

*/

#ifndef SEMANTIC_ANALYSIS_HPP
#define SEMANTIC_ANALYSIS_HPP

#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <functional>

// interp_utils.hpp is included in both of the following includes.
#include "inc_internal/error_handling.hpp"
#include "inc_internal/display_utils.hpp"


// Structures involving interpreted data storage.
namespace DataStorage {

//  Data associated with a variable.
    struct variableInfo {
//      the variable's type
        TypingUtils::dataType type;
//      the variable's value
        std::shared_ptr<CodeTree::valueData> value;
//      true if the value has been optimized pre-runtime
        bool optimize_value;
    };

//  Structure to store variables in distinct scopes.
    class environment {
        public:
//          collection of variables mapped to their values in the current scope
            std::map<std::string, variableInfo> locals;
//          collection of sibling scopes below the current scope
            std::vector<std::shared_ptr<environment>> inner_scopes;
//          reference to the parent scope
            std::shared_ptr<environment> parent_scope;

//          Default constructor, initialize the locals to an empty map, the inner scopes to an empty vector, and the parent scope to nullptr.
            inline environment() noexcept
                : locals({}), 
                  inner_scopes(), 
                  parent_scope(nullptr) {}

//          Initialize the locals to an empty map, the inner scopes to an empty vector, and the parent scope to its given value.
            inline explicit environment(std::shared_ptr<environment> parent)
                : locals({}), 
                  inner_scopes(), 
                  parent_scope(parent) {}
    };

}

/*
Optimize and typecheck on a given AST. Typecheck all of its operations and optimize any constant operations including simple variable assignments.
Update the given data node with the optimized tree structure and update the given environment object with variable updates/assignments if the given boolean is true.

This function assumes that the given data node is a child class member and not an actual instance of the data node class.
This function also assumes that any data node object's type variable accurately represents the child class member that object is.
    e.g. an instance of an ifBlock must have its type variable set to nodeType::IfBlock.

Throw an exception if 
    any code is not correct in type (e.g. an 'if' condition is not of type boolean),
    a variable is referenced without explicit assignment or is assigned explicitly twice, or
    an operator takes invalid operands (e.g. -1 ** 0.5).
Throw a fatal error if an operator, data type, or data node class is not recognized (not implemented).

Parameters:
    data_node: shared pointer to the root of the given AST (input/output)
    scope_env: environment object referencing the current scope in recursive execution (input/output)
    update_env: true if the current recursive execution should update the given environment
                false if the current recursive execution is just to typecheck (input)

Return true if the given data node was completely optimized down to a single node, 
i.e. the entire program was executable before runtime.
*/
const bool analyze_data_node(std::shared_ptr<CodeTree::dataNode>& data_node, std::shared_ptr<DataStorage::environment>& scope_env, const bool update_env);

/*
Optimize and typecheck on a given expressional AST. Typecheck all of its operations and optimize any constant aoperations.
Update the given value data with the optimized tree structure.

This function assumes that the given data node is a child class member and not an actual instance of the value data class.
This function also assumes that any value data object's type variable accurately represents the child class member that object is.
    e.g. an instance of a varContainer must have its type variable set to nodeType::VarContainer.

Throw an exception if 
    any code is not correct in type (e.g. adding '4' to 'false'),
    a variable is referenced without explicit assignment, or
    an operator takes invalid operands (e.g. -1 ** 0.5).
Throw a fatal error if an operator, data type, or value data class is not recognized (not implemented).

Parameters:
    value_data: shared pointer to the root of the given expressional AST (input/output)
    scope_env: environment object referencing the current scope in recursive execution (input)

Return a pair containing
    first: true if the given expressional AST could be completely optimized down to a single node
    second: the type of the given expressional AST
*/
std::pair<bool, TypingUtils::dataType> analyze_value_data(std::shared_ptr<CodeTree::valueData>& value_data, std::shared_ptr<DataStorage::environment>& scope_env);

#endif
