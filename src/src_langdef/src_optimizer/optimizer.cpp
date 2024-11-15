#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


// Anonymous namespace containing optimization helper functions.
namespace {


    bool type_mismatch(Action& action1, Action& action2) { // work on how to type check variables
        return action1.index() != action2.index();
    }


    bool incompatible_type(Action& action, std::vector<size_t> types) {
        for (int i = 0; i < types.size(); i++) {
            if (types[i] == action.index()) {
                return false;
            }
        }
        return true;
    }


    int int_exp(int base, int exp) {
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

}


bool evaluate_primitive(Action& action, VarMap& var_stack, VarData& action_value) {
    if (action.index() == 0) { // Integer
        std::shared_ptr<Integer> int_action = std::move(std::get<std::shared_ptr<Integer>>(action));
        int num = int_action->number;

        action_value = num;

        return true;
    } else if (action.index() == 1) { // Variable
        std::shared_ptr<Variable> var_action = std::move(std::get<std::shared_ptr<Variable>>(action));
        String variable = std::move(var_action->variable);

        action_value = var_stack.at(variable);

        return true;
    }
    return false;
}


bool optimize_action(Action& action, VarMap& var_stack) {
    if (action.index() == 0) { // Integer
        return true;

    } else if (action.index() == 1) { // Variable
        return true;

    } else if (action.index() == 2) { // BinaryOperator
        std::shared_ptr<BinaryOperator> binary_op = std::move(std::get<std::shared_ptr<BinaryOperator>>(action));

        TokenKey current_op = binary_op->op;

        Action current_expr1 = std::move(binary_op->expression1);
        Action current_expr2 = std::move(binary_op->expression2);
        
        optimize_action(current_expr1, var_stack);
        optimize_action(current_expr2, var_stack);

        if ((type_mismatch(current_expr1, current_expr2)) || (incompatible_type(current_expr1, number_types))) { // doesn't work with variables
            perror("");
            exit(EXIT_FAILURE);
        }
        
        if (current_expr1.index() == 0) { // Integer
            std::shared_ptr<Integer> int1 = std::get<std::shared_ptr<Integer>>(current_expr1);
            std::shared_ptr<Integer> int2 = std::get<std::shared_ptr<Integer>>(current_expr2);
            
            switch (current_op) {
                case Plus:
                    action = std::make_shared<Integer>(int1->number + int2->number);
                    return true;
                case Minus:
                    action = std::make_shared<Integer>(int1->number - int2->number);
                    return true;
                case Mult:
                    action = std::make_shared<Integer>(int1->number * int2->number);
                    return true;
                case Div: 
                    if (int2->number == 0) {
                        perror("");
                        exit(EXIT_FAILURE);
                    }

                    action = std::make_shared<Integer>(int1->number / int2->number);
                    return true;
                case Exp:
                    action = std::make_shared<Integer>(int_exp(int1->number, int2->number));
                    return true;
            }
        } else {
            perror("Optimization failed.");
            exit(EXIT_FAILURE);
        }
    } else if (action.index() == 3) { // Assign
            VarData var_value;
            
            std::shared_ptr<Assign> assignment = std::move(std::get<std::shared_ptr<Assign>>(action));
            Action expr = std::move(assignment->expression);
            optimize_action(expr, var_stack);
            evaluate_primitive(expr, var_stack, var_value);

            var_stack[assignment->variable] = var_value;
            
            return true;
    } else {
        perror("Optimization failed.");
        exit(EXIT_FAILURE);
    }
}
