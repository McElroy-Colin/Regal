// File containing the Regal execution function.

#include "../../../include/inc_langdef/langdef.hpp"
#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


// Execute the given action on the given stack.
//      action: action to execute (input)
//      var_stack: stack of variables to execute on (input)
void execute_action(Action& action, std::map<String, Action>& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Assign>>(action)) { // Assign
        std::shared_ptr<Assign> var_action = std::move(std::get<std::shared_ptr<Assign>>(action));
        execute_action(var_action->expression, var_stack);

        var_stack[var_action->variable] = var_action->expression;

        return;
    } else if (std::holds_alternative<std::shared_ptr<Reassign>>(action)) { // Reassign
        std::shared_ptr<Reassign> var_action = std::move(std::get<std::shared_ptr<Reassign>>(action));

//      Locate the given variable in the stack.
        auto iter = var_stack.find(var_action->variable);
        if (iter == var_stack.end()) {
            const String error_msg = "variable \'" + var_action->variable + "\' not initialized";
            throw std::runtime_error(error_msg);
        }

        execute_action(var_action->expression, var_stack);

        if ((var_action->implicit) && (type_mismatch(var_stack[var_action->variable], var_action->expression))) {
            const String error_msg = "impicit reassignment of variable \'" + var_action->variable + "\' must be of the same type.";
            throw std::runtime_error(error_msg);
        }

        var_stack[var_action->variable] = var_action->expression;

        return;
    }
    
    return;
}