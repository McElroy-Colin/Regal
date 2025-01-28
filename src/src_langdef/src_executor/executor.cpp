// File containing the Regal execution function.

#include "../../../include/inc_langdef/langdef.hpp"
#include "../../../include/inc_langdef/optimizer.hpp"
#include "../../../include/inc_debug/error_handling.hpp"


// Execute the given action on the given stack.
//      action: action to execute (input)
//      var_stack: stack of variables to execute on (input)
void execute_action(Action& action, std::map<String, Action>& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Assign>>(action)) { // Assign
        std::shared_ptr<Assign> var_action = std::move(std::get<std::shared_ptr<Assign>>(action));
        execute_action(var_action->expression, var_stack);

//      Check if the given variable has already been initialized.
        auto iter = var_stack.find(var_action->variable);
        if (iter != var_stack.end()) {
            throw VariablePreInitializedError(var_action->variable);
        }

        var_stack[var_action->variable] = var_action->expression;

        return;
    } else if (std::holds_alternative<std::shared_ptr<Reassign>>(action)) { // Reassign
        std::shared_ptr<Reassign> var_action = std::move(std::get<std::shared_ptr<Reassign>>(action));

//      Locate the given variable in the stack.
        auto iter = var_stack.find(var_action->variable);
        if (iter == var_stack.end()) {
            throw VariableNotInitializedError(var_action->variable);
        }

        execute_action(var_action->expression, var_stack);

//      Type check reassignment whether it is implicit or not.
        if ((var_action->implicit) && (type_mismatch(var_stack[var_action->variable], var_action->expression))) {
            throw ImplicitMismatchError(var_action->variable);
        } else if ((!var_action->implicit) && (!type_mismatch(var_stack[var_action->variable], var_action->expression))) {
            throw ExplicitMismatchError(var_action->variable);
        }

        var_stack[var_action->variable] = var_action->expression;

        return;
    }
    
    return;
}