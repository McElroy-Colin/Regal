#include <ranges>
#include "../../../include/inc_langdef/inc_parser/parser.hpp"
#include "../../../include/inc_langdef/inc_optimizer/static_optimizer.hpp"

// Temp
#include <iostream>

bool type_mismatch(Action& action1, Action& action2) {
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

int power(int base, int exp) {
    if (exp == 0) {
        return 1;
    } else if (exp < 0) {
        perror("Exponential must be a nonnegative exponent.");
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

bool evaluate_action(Action& action, VarMap& var_stack) {
    switch (action.index()) {
        case 0: // Integer
            return true;
        case 1: // Variable
            return true;
        case 2: // BinaryOperator
            std::shared_ptr<BinaryOperator> binary_op = std::move(std::get<std::shared_ptr<BinaryOperator>>(action));

            Operator current_op = binary_op->op;

            std::cout << std::get<std::shared_ptr<Integer>>(binary_op->expression1).use_count() << std::endl;

            Action current_expr1 = std::move(binary_op->expression1);

            std::cout << std::get<std::shared_ptr<Integer>>(binary_op->expression1).use_count() << std::endl;
            std::cout << std::get<std::shared_ptr<Integer>>(current_expr1).use_count() << std::endl;

            Action current_expr2 = binary_op->expression2;
            evaluate_action(current_expr1, var_stack);
            evaluate_action(current_expr2, var_stack);

            if (type_mismatch(current_expr1, current_expr2)) {
                perror("");
                exit(EXIT_FAILURE);
            } else if (incompatible_type(current_expr1, number_types)) {
                perror("");
                exit(EXIT_FAILURE);
            }
            
            switch (current_expr1.index()) {
                case 0: // Integer
                    std::shared_ptr<Integer> int1 = std::get<std::shared_ptr<Integer>>(current_expr1);
                    std::shared_ptr<Integer> int2 = std::get<std::shared_ptr<Integer>>(current_expr2);
                    
                    switch (current_op) {
                        case Add:
                            action = std::make_shared<Integer>(int1->number + int2->number);
                            return true;
                        case Subtract:
                            action = std::make_shared<Integer>(int1->number - int2->number);
                            return true;
                        case Multiply:
                            action = std::make_shared<Integer>(int1->number * int2->number);
                            return true;
                        case Divide: 
                            if (int2->number == 0) {
                                perror("");
                                exit(EXIT_FAILURE);
                            }

                            action = std::make_shared<Integer>(int1->number / int2->number);
                            return true;
                        case Exponential:
                            action = std::make_shared<Integer>(power(int1->number, int2->number));
                            return true;
                    }
            }
    }
}

int main() {
    Action num1 = std::make_shared<Integer>(2);
    Action num2 = std::make_shared<Integer>(4);

    Action add = std::make_shared<BinaryOperator>(Add, num1, num2);

    VarMap empty;

    evaluate_action(add, empty);

    return 0;
}