// File containing Regal optimization and type-checking functions.

#include "../../../include/inc_langdef/optimizer.hpp"
#include "../../../include/inc_debug/error_handling.hpp"


// Anonymous namespace containing optimization helper functions.
namespace {

//  Calculate the exponent with integer base and exponent.
//      base: integer base (input)
//      exp: integer exponent to apply to base (input)
    int int_exp(const int base, const int exp) {
        if (exp == 0) {
            return 1;
        } else if (exp < 0) {
            throw IncorrectInputError("invalid integer exponent given: \'" + std::to_string(exp) + "\'");
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

// Optimize a given action down to its necessary runtime components. 
// Update the given action to a presumably smaller tree structure. 
// Return true if the action was optimized, meaning the given action was not a low-level value.
//      action: action to be optimized (input/output)
//      var_stack: stack containing initialized variables (input)
bool optimize_action(Action& action, std::map<String, Action>& var_stack) {
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) { // Integer
        return false;
    } else if (std::holds_alternative<std::shared_ptr<Boolean>>(action)) { // Boolean
        return false;

    } else if (std::holds_alternative<std::shared_ptr<Variable>>(action)) { // Variable
        std::shared_ptr<Variable> var_action = std::move(std::get<std::shared_ptr<Variable>>(action));
        const String& variable = var_action->get_variable();

//      Locate the given variable in the stack.
        auto iter = var_stack.find(variable);
        if (iter == var_stack.end()) {
//          Handle when a variable does not exist in the stack.
            throw VariableNotInitializedError(variable);
        }

//      Update action to the variable's stored value.
        action = iter->second;

        return true;

    } else if (std::holds_alternative<std::shared_ptr<UnaryOperator>>(action)) { // UnaryOperator
        std::shared_ptr<UnaryOperator> unary_op = std::move(std::get<std::shared_ptr<UnaryOperator>>(action));
        
//      Optimize the operator's expression.
        optimize_action(unary_op->get_expression(), var_stack);

        switch (unary_op->get_op()) {
            case Not: {
                if (!std::holds_alternative<std::shared_ptr<Boolean>>(unary_op->get_expression())) { // Boolean
                    throw InavlidOperatorError(unary_op->get_expression(), Not);
                }

                std::shared_ptr<Boolean> bool_expr = std::move(std::get<std::shared_ptr<Boolean>>(unary_op->get_expression()));
                action = std::make_shared<Boolean>(!bool_expr->get_boolean());
                return true;
            }
            default:
                throw InavlidOperatorError(unary_op->get_expression(), unary_op->get_op());
        }

    } else if (std::holds_alternative<std::shared_ptr<BinaryOperator>>(action)) { // BinaryOperator
        std::shared_ptr<BinaryOperator> binary_op = std::move(std::get<std::shared_ptr<BinaryOperator>>(action));

//      Optimize each side of the operator.
        optimize_action(binary_op->get_expression1(), var_stack);
        optimize_action(binary_op->get_expression2(), var_stack);

//      For each operator, check for a type mismatch then perform the operation and assign it to the output action variable.
        switch (binary_op->get_op()) {
            case Plus:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));
                    action = std::make_shared<Integer>(int1->get_number() + int2->get_number());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;

            case Minus:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));
                    action = std::make_shared<Integer>(int1->get_number() - int2->get_number());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;

            case Mult:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));
                    action = std::make_shared<Integer>(int1->get_number() * int2->get_number());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;

            case Div:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));

                    if (int2->get_number() == 0) {
                        throw DivisionByZeroError();
                    }
                    action = std::make_shared<Integer>(int1->get_number() * int2->get_number());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;

            case Exp:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));
                    action = std::make_shared<Integer>(int_exp(int1->get_number(), int2->get_number()));
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;
            
            case Greater:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));
                    action = std::make_shared<Boolean>(int1->get_number() > int2->get_number());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;
            
            case Less:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));
                    action = std::make_shared<Boolean>(int1->get_number() < int2->get_number());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;

            case And:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Boolean>>(binary_op->get_expression1())) {
                    std::shared_ptr<Boolean> bool1 = std::move(std::get<std::shared_ptr<Boolean>>(binary_op->get_expression1()));
                    std::shared_ptr<Boolean> bool2 = std::move(std::get<std::shared_ptr<Boolean>>(binary_op->get_expression2()));
                    action = std::make_shared<Boolean>(bool1->get_boolean() && bool2->get_boolean());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;

            case Or:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Boolean>>(binary_op->get_expression1())) {
                    std::shared_ptr<Boolean> bool1 = std::move(std::get<std::shared_ptr<Boolean>>(binary_op->get_expression1()));
                    std::shared_ptr<Boolean> bool2 = std::move(std::get<std::shared_ptr<Boolean>>(binary_op->get_expression2()));
                    action = std::make_shared<Boolean>(bool1->get_boolean() || bool2->get_boolean());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;

            case Is:
                if (type_mismatch(binary_op->get_expression1(), binary_op->get_expression2())) {
                    throw TypeMismatchError(binary_op->get_op());
                }

                if (std::holds_alternative<std::shared_ptr<Integer>>(binary_op->get_expression1())) {
                    std::shared_ptr<Integer> int1 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression1()));
                    std::shared_ptr<Integer> int2 = std::move(std::get<std::shared_ptr<Integer>>(binary_op->get_expression2()));
                    action = std::make_shared<Boolean>(int1->get_number() == int2->get_number());

                } else if (std::holds_alternative<std::shared_ptr<Boolean>>(binary_op->get_expression1())) {
                    std::shared_ptr<Boolean> bool1 = std::move(std::get<std::shared_ptr<Boolean>>(binary_op->get_expression1()));
                    std::shared_ptr<Boolean> bool2 = std::move(std::get<std::shared_ptr<Boolean>>(binary_op->get_expression2()));
                    action = std::make_shared<Boolean>(bool1->get_boolean() == bool2->get_boolean());
                } else {
                    throw InavlidOperatorError(binary_op->get_expression1(), binary_op->get_op());
                }
                return true;
            default:
                throw FatalError("binary operator \'" + binary_op->disp(Literal) + "\' not recognized");
        }
    } else if (std::holds_alternative<std::shared_ptr<TernaryOperator>>(action)) { // TernaryOperator
        std::shared_ptr<TernaryOperator> ternary_op = std::move(std::get<std::shared_ptr<TernaryOperator>>(action));

        optimize_action(ternary_op->get_expression1(), var_stack);
        optimize_action(ternary_op->get_expression2(), var_stack);
        optimize_action(ternary_op->get_expression3(), var_stack);

        switch (ternary_op->get_op()) {
            case If: {
                if (!std::holds_alternative<std::shared_ptr<Boolean>>(ternary_op->get_expression2())) {
                    throw TypeMismatchError(ternary_op->get_expression2());
                }

                std::shared_ptr<Boolean> bool_condition = std::move(std::get<std::shared_ptr<Boolean>>(ternary_op->get_expression2()));

                if (bool_condition->get_boolean()) {
                    action = std::move(ternary_op->get_expression1());
                } else {
                    action = std::move(ternary_op->get_expression3());
                }

                return true;
            }
            default:
                throw FatalError("ternary operator \'" + ternary_op->disp(Literal) + "\' not recognized");
        }

    // Assigning/reasigning a variable only optimizes the expression being assigned, no assignment is made.
    } else if (std::holds_alternative<std::shared_ptr<Assign>>(action)) { // Assign
        optimize_action(std::get<std::shared_ptr<Assign>>(action)->get_expression(), var_stack);
            
        return true;
    } else if (std::holds_alternative<std::shared_ptr<Reassign>>(action)) { // Reassign
        optimize_action(std::get<std::shared_ptr<Reassign>>(action)->get_expression(), var_stack);
            
        return true;
    }
    
    throw FatalError("optimization failed");
}
