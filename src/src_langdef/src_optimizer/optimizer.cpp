// File containing Regal optimization and type-checking functions.

#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


// Anonymous namespace containing optimization helper functions.
namespace {

//  Return true if the given actions are of different types.
//      action1: first action to compare (input)
//      action2: second action to compare (input)
    bool type_mismatch(const Action& action1, const Action& action2) {
        return action1.index() != action2.index();
    }


//  Return true if the given action is non of the given types.
//      action: action to compare (input)
//      types: types to check the action for (input)
    bool incompatible_type(const Action& action, const std::vector<size_t>& types) {
        for (int i = 0; i < types.size(); i++) {
            if (types[i] == action.index()) {
                return false;
            }
        }
        return true;
    }


//  Calculate the exponent with integer base and exponent.
//      base: integer base (input)
//      exp: integer exponent to apply to base (input)
    int int_exp(int base, int exp) {
        if (exp == 0) {
            return 1;
        } else if (exp < 0) {
            throw std::runtime_error("integer exponential must use a nonnegative exponent");
        }

//      Lambda expression to recursively calculate the exponential.
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
//      action: action to be updated and optimized (input)
//      var_stack: stack containing initialized variables (input)
bool optimize_action(Action& action, std::map<String, Action>& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) { // Integer
        return false;

    } else if (std::holds_alternative<std::shared_ptr<Variable>>(action)) { // Variable
        std::shared_ptr<Variable> var_action = std::move(std::get<std::shared_ptr<Variable>>(action));
        String variable = std::move(var_action->variable);

//      Locate the given variable in the stack.
        auto iter = var_stack.find(variable);
        if (iter == var_stack.end()) {
            String error_msg = "variable \'" + variable + "\' not initialized";
            throw std::runtime_error(error_msg);
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

//      Ensure that the expressions match in type and are compatible with their operator.
        if ((type_mismatch(current_expr1, current_expr2)) || (incompatible_type(current_expr1, number_types))) {
            throw std::runtime_error("incompatible types in binary operator");
        }
        
//      Evaluate based on what type the expressions are.
        if (std::holds_alternative<std::shared_ptr<Integer>>(current_expr1)) { // Integer
            std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(current_expr1));
            std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(current_expr2));
            
//          Update action with the evaluated operation.
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
                        throw std::runtime_error("optimizing failed");
                    }

                    action = std::make_shared<Integer>(int1->number / int2->number); // Returns truncated division
                    return true;
                case Exp:
                    action = std::make_shared<Integer>(int_exp(int1->number, int2->number));
                    return true;
                default:
                    throw std::runtime_error("binary operator not using a valid operator");
            }
        } else {
            throw std::runtime_error("optimizing failed");
        }
    // Assigning/reasigning a variable only optimizes the expression being assigned, no assignment is made.
    } else if (std::holds_alternative<std::shared_ptr<Assign>>(action)) { // Assign
        optimize_action(std::get<std::shared_ptr<Assign>>(action)->expression, var_stack);
            
        return true;
    } else if (std::holds_alternative<std::shared_ptr<Reassign>>(action)) { // Reassign
        optimize_action(std::get<std::shared_ptr<Reassign>>(action)->expression, var_stack);
            
        return true;
        
    } else {
        throw std::runtime_error("optimizing failed");
    }
}
