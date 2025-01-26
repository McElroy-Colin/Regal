// File containing Regal optimization and type-checking functions.

#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"


// Anonymous namespace containing optimization helper functions.
namespace {

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

// TODO: convert it to a switch statement for optimization

// Optimize a given action down to its necessary components. Update the given action to a presumably smaller tree structure. 
// Return true if the action was optimized, meaning the given action was not a primitive.
//      action: action to be updated and optimized (input)
//      var_stack: stack containing initialized variables (input)
bool optimize_action(Action& action, std::map<String, Action>& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) { // Integer
        return false;
    } else if (std::holds_alternative<std::shared_ptr<Boolean>>(action)) { // Boolean
        return false;

    } else if (std::holds_alternative<std::shared_ptr<Variable>>(action)) { // Variable
        std::shared_ptr<Variable> var_action = std::move(std::get<std::shared_ptr<Variable>>(action));
        const String variable = std::move(var_action->variable);

//      Locate the given variable in the stack.
        auto iter = var_stack.find(variable);
        if (iter == var_stack.end()) {
            const String error_msg = "variable \'" + variable + "\' not initialized";
            throw std::runtime_error(error_msg);
        }

//      Update action to the variable's stored value.
        action = iter->second;

        return true;

    } else if (std::holds_alternative<std::shared_ptr<UnaryOperator>>(action)) { // UnaryOperator
        std::shared_ptr<UnaryOperator> unary_op = std::move(std::get<std::shared_ptr<UnaryOperator>>(action));

        TokenKey current_op = unary_op->op;

        Action current_expr = std::move(unary_op->expression);
        optimize_action(current_expr, var_stack);

        if (std::holds_alternative<std::shared_ptr<Boolean>>(current_expr)) { // Boolean
            std::shared_ptr<Boolean> bool_expr = std::move(std::get<std::shared_ptr<Boolean>>(current_expr));

            switch (current_op) {
                case Not:
                    action = std::make_shared<Boolean>(!(bool_expr->boolean));
                    return true;
                default:
                    String op_string;
                    const String error_msg = "boolean using \'" + (display_token(current_op, op_string, false), op_string) + "\' as an operator";
                    throw std::runtime_error(error_msg);
            }
        } else {
            throw std::runtime_error("unary operator undefined type");
        }

    } else if (std::holds_alternative<std::shared_ptr<BinaryOperator>>(action)) { // BinaryOperator
        std::shared_ptr<BinaryOperator> binary_op = std::move(std::get<std::shared_ptr<BinaryOperator>>(action));

        TokenKey current_op = binary_op->op;

//      Optimize each side of the operator.
        Action current_expr1 = std::move(binary_op->expression1);
        Action current_expr2 = std::move(binary_op->expression2);
        optimize_action(current_expr1, var_stack);
        optimize_action(current_expr2, var_stack);

//      Ensure that the expressions match in type.
        if (type_mismatch(current_expr1, current_expr2)) {
            throw std::runtime_error("binary operator mismatched types");
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
                        throw std::runtime_error("division by zero");
                    }

                    action = std::make_shared<Integer>(int1->number / int2->number); // Returns truncated division
                    return true;
                case Exp:
                    action = std::make_shared<Integer>(int_exp(int1->number, int2->number));
                    return true;
                case Greater:
                    action = std::make_shared<Boolean>(int1->number > int2->number);
                    return true;
                case Less:
                    action = std::make_shared<Boolean>(int1->number < int2->number);
                    return true;
                case Equals:
                    action = std::make_shared<Boolean>(int1->number == int2->number);
                    return true;
                default:
                    String op_string;
                    const String error_msg = "integers using \'" + (display_token(current_op, op_string, false), op_string) + "\' as an operator";
                    throw std::runtime_error(error_msg);
            }
        } else if (std::holds_alternative<std::shared_ptr<Boolean>>(current_expr1)) { // Boolean
            std::shared_ptr<Boolean> bool1 = std::move(std::get<std::shared_ptr<Boolean>>(current_expr1));
            std::shared_ptr<Boolean> bool2 = std::move(std::get<std::shared_ptr<Boolean>>(current_expr2));

            switch(current_op){
                case Or:
                    action = std::make_shared<Boolean>(bool1 ->boolean || bool2->boolean);
                    return true;
                case And:
                    action = std::make_shared<Boolean>(bool1 ->boolean && bool2->boolean);
                    return true;
                default:
                    String op_string;
                    const String error_msg = "booleans using \'" + (display_token(current_op, op_string, false), op_string) + "\' as an operator";
                    throw std::runtime_error(error_msg);
            }
        } else {
            throw std::runtime_error("binary operator undefined type");
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
