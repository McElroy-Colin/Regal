// File containing the Regal execution function.

#include "../../../include/inc_langdef/langdef.hpp"
#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


// Execute the given action on the given stack.
//      action: action to execute (input)
//      var_stack: stack of variables to execute on (input)
void execute_action(Action& action, std::map<String, Action>& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) { // Integer
        return;
    } else if (std::holds_alternative<std::shared_ptr<Variable>>(action)) { // Variable
        return;
    } else if (std::holds_alternative<std::shared_ptr<BinaryOperator>>(action)) { // BinaryOperator
        return;
    } else if (std::holds_alternative<std::shared_ptr<Assign>>(action)) { // Assign
        std::shared_ptr<Assign> var_action = std::move(std::get<std::shared_ptr<Assign>>(action));
        execute_action(var_action->expression, var_stack);

        var_stack[var_action->variable] = var_action->expression;

        return;
    } else if (std::holds_alternative<std::shared_ptr<Reassign>>(action)) { // Reassign
        std::shared_ptr<Reassign> var_action = std::move(std::get<std::shared_ptr<Reassign>>(action));
        execute_action(var_action->expression, var_stack);

        var_stack[var_action->variable] = var_action->expression;

        return;
    } else {
        throw std::runtime_error("execution failed");
    }
}