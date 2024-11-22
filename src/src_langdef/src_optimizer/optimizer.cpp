#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


// Anonymous namespace containing optimization helper functions.
namespace {

//  Check that given actions are of different types.
    bool type_mismatch(Action& action1, Action& action2) {
        return action1.index() != action2.index();
    }


//  Check that the given action is none of the given types.
    bool incompatible_type(Action& action, std::vector<size_t> types) {
        for (int i = 0; i < types.size(); i++) {
            if (types[i] == action.index()) {
                return false;
            }
        }
        return true;
    }


//  Calculate the exponent with integer base and exponent.
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


// Optimize a given action down to its necessary components. Update the given action to a presumably smaller tree structure. 
// Return true if the action was optimized, meaning the given action was not a primitive.
bool optimize_action(Action& action, VarMap& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) { // Integer
        return false;

    } else if (std::holds_alternative<std::shared_ptr<Variable>>(action)) { // Variable
        std::shared_ptr<Variable> var_action = std::move(std::get<std::shared_ptr<Variable>>(action));
        String variable = std::move(var_action->variable);

//      Locate the given variable in the stack.
        auto iter = var_stack.find(variable);
        if (iter == var_stack.end()) {
            perror("Variable not initialized");
            exit(EXIT_FAILURE);
        }

//      Update action to the variable's stored value.
        action = iter->second;

        return true;

    } else if (std::holds_alternative<std::shared_ptr<BinaryOperator>>(action)) { // BinaryOperator
        std::shared_ptr<BinaryOperator> binary_op = std::move(std::get<std::shared_ptr<BinaryOperator>>(action));

        TokenKey current_op = binary_op->op;

//      Optimize each side of the operator.
        Action current_expr1 = std::move(binary_op->expression1);
        Action current_expr2 = std::move(binary_op->expression2);
        optimize_action(current_expr1, var_stack);
        optimize_action(current_expr2, var_stack);

//      Ensure that the expression match in type and are compatible with their operator.
        if ((type_mismatch(current_expr1, current_expr2)) || (incompatible_type(current_expr1, number_types))) {
            perror("Incompatible types in binary operator");
            exit(EXIT_FAILURE);
        }
        
//      Evaluate based on what type the expressions are.
        if (std::holds_alternative<std::shared_ptr<Integer>>(current_expr1)) { // Integer
            std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(current_expr1));
            std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(current_expr2));
            
//          Update action with the evaluated operaton.
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
                        perror("Division by zero");
                        exit(EXIT_FAILURE);
                    }

                    action = std::make_shared<Integer>(int1->number / int2->number);
                    return true;
                case Exp:
                    action = std::make_shared<Integer>(int_exp(int1->number, int2->number));
                    return true;
                default:
                    perror("Binary operator not using a valid operator");
                    exit(EXIT_FAILURE);
            }
        } else {
            perror("Optimization failed");
            exit(EXIT_FAILURE);
        }
    // Assigning/reasigning a variable only optimizes the expression being assigned, no assignment is made.
    } else if (std::holds_alternative<std::shared_ptr<Assign>>(action)) { // Assign
        optimize_action(std::get<std::shared_ptr<Assign>>(action)->expression, var_stack);
            
        return true;
    } else if (std::holds_alternative<std::shared_ptr<Reassign>>(action)) { // Reassign
        optimize_action(std::get<std::shared_ptr<Reassign>>(action)->expression, var_stack);
            
        return true;
        
    } else {
        perror("Optimization failed.");
        exit(EXIT_FAILURE);
    }
}
