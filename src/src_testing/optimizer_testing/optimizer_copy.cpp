/*
#include "../parser_testing/parser_copy.hpp"
#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


bool type_mismatch_c(Action_c& action1, Action_c& action2) {
    return action1.index() != action2.index();
}

bool incompatible_type_c(Action_c& action, std::vector<size_t> types) {
    for (int i = 0; i < types.size(); i++) {
        if (types[i] == action.index()) {
            return false;
        }
    }
    return true;
}

int int_exp_c(int base, int exp) {
    if (exp == 0) {
        return 1;
    } else if (exp < 0) {
        perror("Integer exponential must use a nonnegative exponent.");
        exit(EXIT_FAILURE);
    }

    auto rec_power = [](auto self, int b, int e) -> int {
        if (e == 1) {
            return b;
        }
        return b * self(self, b, e - 1);
    };

    return rec_power(rec_power, base, exp);
}


bool evaluate_primitive_c(Action_c& action, VarMap& var_stack, VarData& action_value) {
    if (action.index() == 0) { // Integer
        std::shared_ptr<Integer_c> int_action = std::move(std::get<std::shared_ptr<Integer_c>>(action));
        int num = int_action->number;

        action_value = num;

        return true;
    } else if (action.index() == 1) { // Variable
        std::shared_ptr<Variable_c> var_action = std::move(std::get<std::shared_ptr<Variable_c>>(action));
        String variable = var_action->variable;

        action_value = var_stack.at(variable);

        return true;
    }
    return false;
}


bool optimize_action_c(Action_c& action, VarMap& var_stack) {
    if (action.index() == 0) { // Integer
        return true;

    } else if (action.index() == 1) { // Variable
        return true;

    } else if (action.index() == 2) { // BinaryOperator
        std::shared_ptr<BinaryOperator_c> binary_op = std::move(std::get<std::shared_ptr<BinaryOperator_c>>(action));

        TokenKey current_op = binary_op->op;

        Action_c current_expr1 = std::move(binary_op->expression1);
        Action_c current_expr2 = std::move(binary_op->expression2);
        
        optimize_action_c(current_expr1, var_stack);
        optimize_action_c(current_expr2, var_stack);

        if ((type_mismatch_c(current_expr1, current_expr2)) || (incompatible_type_c(current_expr1, number_types))) {
            perror("");
            exit(EXIT_FAILURE);
        }
        
        if (current_expr1.index() == 0) { // Integer
            std::shared_ptr<Integer_c> int1 = std::get<std::shared_ptr<Integer_c>>(current_expr1);
            std::shared_ptr<Integer_c> int2 = std::get<std::shared_ptr<Integer_c>>(current_expr2);
            
            switch (current_op) {
                case Plus:
                    action = std::make_shared<Integer_c>(int1->number + int2->number);
                    return true;
                case Minus:
                    action = std::make_shared<Integer_c>(int1->number - int2->number);
                    return true;
                case Mult:
                    action = std::make_shared<Integer_c>(int1->number * int2->number);
                    return true;
                case Div: 
                    if (int2->number == 0) {
                        perror("");
                        exit(EXIT_FAILURE);
                    }

                    action = std::make_shared<Integer_c>(int1->number / int2->number);
                    return true;
                case Exp:
                    action = std::make_shared<Integer_c>(int_exp_c(int1->number, int2->number));
                    return true;
            }
        }
    } else if (action.index() == 3) { // Assign
            VarData var_value;
            
            std::shared_ptr<Assign_c> assignment = std::move(std::get<std::shared_ptr<Assign_c>>(action));
            Action_c expr = assignment->expression;
            optimize_action_c(expr, var_stack);
            evaluate_primitive_c(expr, var_stack, var_value);

            var_stack[assignment->variable] = var_value;
            
            return true;
    }
}
*/