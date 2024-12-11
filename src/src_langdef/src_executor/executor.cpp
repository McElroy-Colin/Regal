#include "../../../include/inc_langdef/langdef.hpp"
#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


bool execute_action(Action& action, VarMap& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) { // Integer
        return true;
    } else if (std::holds_alternative<std::shared_ptr<Variable>>(action)) { // Variable
        return true;
    } else if (std::holds_alternative<std::shared_ptr<BinaryOperator>>(action)) { // BinaryOperator
        return true;
    } else if (std::holds_alternative<std::shared_ptr<Assign>>(action)) { // Assign
        std::shared_ptr<Assign> var_action = std::move(std::get<std::shared_ptr<Assign>>(action));
        execute_action(var_action->expression, var_stack);

        var_stack[var_action->variable] = var_action->expression;

        return true;
    } else if (std::holds_alternative<std::shared_ptr<Reassign>>(action)) { // Reassign
        std::shared_ptr<Reassign> var_action = std::move(std::get<std::shared_ptr<Reassign>>(action));
        execute_action(var_action->expression, var_stack);

        var_stack[var_action->variable] = var_action->expression;

        return true;
    } else {
        perror("Optimization failed.");
        throw std::exception();
    }
}