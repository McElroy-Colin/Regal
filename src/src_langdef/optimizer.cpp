// File containing Regal optimization and type-checking functions.

#include "inc_langdef/optimizer.hpp"

using std::list, std::map, std::shared_ptr, std::string, std::pair, std::tuple, 
      std::to_string, std::make_pair, std::make_tuple, std::move, std::get, std::dynamic_pointer_cast, std::make_shared;
using namespace TokenDef;
using namespace CodeTree;
using namespace TypingUtils;
using namespace DataStorage;
using namespace DebugUtils;


// Namespace for structures involving interpreted data storage.
namespace DataStorage {
    stack::stack() : locals({}), inner_scopes(), parent_scope(nullptr) {}
    stack::stack(shared_ptr<stack> parent) : locals({}), inner_scopes(), parent_scope(parent) {}
}

// Anonymous namespace containing optimization helper functions.
namespace {
//  Calculate the exponential with integer base and exponent.
//      base: integer base (input)
//      exp: integer exponent to apply to base (input)
    int _int_exp(const int base, const int exp) {
        if (exp == 0) {
            return 1;
        } else if (exp < 0) {
            throw IncorrectInputError("invalid integer exponent given: \'" + to_string(exp) + "\'");
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

//  Initialize a new scope and optimize a code block within it.
//  Return true if the block of code was completely optimized.
//      parent_scope: initial scope environment (input)
//      code_block: block of code to be optimized (input)
//      update_stacks: true if current and parent stacks should reassign variables (input)
//      pop_scope: true if the optimized scope should be removed after optimization (input)
    bool _create_optimize_scope(shared_ptr<stack> parent_scope, shared_ptr<dataNode>& code_block, const bool update_stacks, const bool pop_scope) {
        bool optimized_block;

//      Initialize a new scope and optimize the code block.
        parent_scope->inner_scopes.push_back(make_shared<stack>(parent_scope));
        optimized_block = optimize_typecheck_data_node(code_block, parent_scope->inner_scopes.back(), update_stacks);

        if (pop_scope) {
            parent_scope->inner_scopes.pop_back();
        }

        return optimized_block;
    }

//  Optimize and typecheck the expressions in a binary operator. This function assumes operator expressions should have matching types.
//  Return both whether the expressions were optimized and the type of the expressions in a pair.
//      binary_op: pointer to binary operator to be optimized (input)
//      scope_stack: current scope to optimize the expressions over (input)
    pair<bool, dataType> _binaryop_optimize_typecheck(binaryOp* binary_op, shared_ptr<stack>& scope_stack) {
//      Optimize both expressions.
        pair<bool, dataType> optimized1_type1 = optimize_typecheck_value_data(binary_op->expression1, scope_stack);
        pair<bool, dataType> optimized2_type2  = optimize_typecheck_value_data(binary_op->expression2, scope_stack);

//      Ensure expressions have the same type.
        if ((optimized1_type1.second != optimized2_type2.second)) {
            throw TypeMismatchError(binary_op->op, optimized1_type1.second, optimized2_type2.second);
        } else if (!(optimized1_type1.first && optimized2_type2.first)) {
            return make_pair(false, optimized1_type1.second);
        }

        return make_pair(true, optimized1_type1.second);
    }
}

// Optimize and typecheck a given code tree down to its necessary runtime components, including variable assignment. 
// Update the given code tree to a presumably smaller tree structure.
// Return true if the code tree was optimized, meaning the given code tree was known at before runtime.
//      data_node: code tree to be optimized (input/output)
//      scope_stack: stack containing initialized variables (input/output)
//      update_stacks: true if variables should be updated in the current scope and parent scopes (input)
bool optimize_typecheck_data_node(shared_ptr<dataNode>& data_node, shared_ptr<stack>& scope_stack, const bool update_stacks) {
//  Check for each type of non-value data.
    if (codeScope* code_scope = dynamic_cast<codeScope*>(data_node.get())) { // codeScope
        bool optimized_operation, optimized_remainder;

//      Optimize recursively.
        optimized_operation = optimize_typecheck_data_node(code_scope->curr_operation, scope_stack, update_stacks);
        optimized_remainder = optimize_typecheck_data_node(code_scope->remainder, scope_stack, update_stacks);

        return optimized_operation && optimized_remainder;

    } else if (ifBlock* if_block = dynamic_cast<ifBlock*>(data_node.get())) { // ifBlock
        pair<bool, dataType> optimized_type_condition;
        bool optimized_if, optimized_else;

//      Optimize recursively.
        optimized_type_condition = optimize_typecheck_value_data(if_block->bool_condition, scope_stack);
        
        if (optimized_type_condition.second != dataType::BoolT) {
            throw ConditionMismatchError(optimized_type_condition.second, false);
        }

        if (optimized_type_condition.first) {
            shared_ptr<boolContainer> condition = dynamic_pointer_cast<boolContainer>(if_block->bool_condition);
            if (!condition) {
                throw FatalError("dynamic pointer cast in if-block condition failed");

            } else if (condition->boolean) {
                optimized_if = _create_optimize_scope(scope_stack, if_block->code_block, update_stacks, true);
                if (if_block->contains_else) {
                    _create_optimize_scope(scope_stack, if_block->else_block, false, true);
                }
                
                data_node = move(if_block->code_block);
                return optimized_if;

            } else if (if_block->contains_else) {
                _create_optimize_scope(scope_stack, if_block->code_block, false, true);
                optimized_else = _create_optimize_scope(scope_stack, if_block->else_block, update_stacks, true);

                data_node = move(if_block->else_block);
                return optimized_else;

            } else {
                _create_optimize_scope(scope_stack, if_block->code_block, false, true);

                return true;
            }
        } else {
            _create_optimize_scope(scope_stack, if_block->code_block, false, false);
            _create_optimize_scope(scope_stack, if_block->else_block, false, false);

            return false;
        }

    } else if (assignOp* assign = dynamic_cast<assignOp*>(data_node.get())) { // assign
        map<string, variableInfo>::iterator iter;
        pair<bool, dataType> optimized_type;

        shared_ptr<stack> current_scope = scope_stack;
        while (current_scope != nullptr) {
            iter = current_scope->locals.find(assign->variable);
            if (iter != current_scope->locals.end()) {
                throw VariablePreInitializedError(assign->variable);
            }

            current_scope = current_scope->parent_scope;
        }

        optimized_type = optimize_typecheck_value_data(assign->expression, scope_stack);

        scope_stack->locals[assign->variable] = {optimized_type.second, assign->expression, optimized_type.first};
        return optimized_type.first;

    } else if (reassignOp* reassign = dynamic_cast<reassignOp*>(data_node.get())) { // reassign
        map<string, variableInfo>::iterator iter;

        pair<bool, dataType> optimized_type = optimize_typecheck_value_data(reassign->expression, scope_stack);

        shared_ptr<stack> current_scope = scope_stack;
        iter = current_scope->locals.find(reassign->variable);
        while (iter == current_scope->locals.end()) {
            if (current_scope->parent_scope != nullptr) {
                current_scope = current_scope->parent_scope;
                iter = current_scope->locals.find(reassign->variable);
            } else {
                throw VariableNotInitializedError(reassign->variable, false);
            }
        }

        if (reassign->implicit && (optimized_type.second != iter->second.type)) {
            throw ImplicitMismatchError(reassign->variable, iter->second.type, optimized_type.second);
        } else if (!reassign->implicit && (optimized_type.second == iter->second.type)) {
            throw ExplicitMismatchError(reassign->variable, optimized_type.second);
        } else if (update_stacks || (scope_stack == current_scope)) {
            iter->second = {optimized_type.second, reassign->expression, optimized_type.first};
        }

        return optimized_type.first;

    } else {
        throw FatalError("optimization failed");
    }
}

// Optimize and typecheck a given value tree down to its necessary runtime components, including variable assignment. 
// Update the given value tree to a presumably smaller tree structure.
// Return a pair that contains a boolean representing whether the given tree was optimized and a dataType representing the type of the given value tree.
//      value_data: code tree to be optimized (input/output)
//      scope_stack: stack containing initialized variables (input/output)
pair<bool, dataType> optimize_typecheck_value_data(shared_ptr<valueData>& value_data, shared_ptr<stack>& scope_stack) {
//  Check for each type of value data.
    if (unaryOp* unary_op = dynamic_cast<unaryOp*>(value_data.get())) { // unaryOp
        pair<bool, dataType> optimized_type = optimize_typecheck_value_data(unary_op->expression, scope_stack);

//      For each operator, check the expression type and perform the operation if optimizable.
        switch (unary_op->op) {
            case tokenKey::Not:
            case tokenKey::NotW:
//              Ensure this operator takes a boolean.
                if (optimized_type.second != dataType::BoolT) {
                    throw InavlidOperatorError(unary_op->op, dataType::BoolT, optimized_type.second);
//              If the expression could be evaluated, negate it.
                } else if (optimized_type.first) {
                    shared_ptr<boolContainer> bool_expression = dynamic_pointer_cast<boolContainer>(unary_op->expression);
                    value_data = make_shared<boolContainer>(!bool_expression->boolean);

                    return make_pair(true, dataType::BoolT);
                }

                return make_pair(false, dataType::BoolT);
                
            default:
                throw FatalError("unexpected unary operator");
        }

    } else if (binaryOp* binary_op = dynamic_cast<binaryOp*>(value_data.get())) { // binaryOp
        pair<bool, dataType> optimized_type;

//      For each operator, check the expression types and perform the operation if optimizable.
        switch (binary_op->op) {
            case tokenKey::Plus:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<intContainer>(int1->number + int2->number);

                        return make_pair(true, dataType::IntT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            case tokenKey::Minus:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<intContainer>(int1->number - int2->number);

                        return make_pair(true, dataType::IntT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            case tokenKey::Mult:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<intContainer>(int1->number * int2->number);

                        return make_pair(true, dataType::IntT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            case tokenKey::Div:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        
                        if (int2->number == 0) {
                            throw DivisionByZeroError("division by zero");
                        }

                        value_data = make_shared<intContainer>(int1->number / int2->number);

                        return make_pair(true, dataType::IntT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            case tokenKey::Exp:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<intContainer>(_int_exp(int1->number, int2->number));

                        return make_pair(true, dataType::IntT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            case tokenKey::And:
            case tokenKey::AndW:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::BoolT: {
                        shared_ptr<boolContainer> bool1 = dynamic_pointer_cast<boolContainer>(binary_op->expression1);
                        shared_ptr<boolContainer> bool2 = dynamic_pointer_cast<boolContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(bool1->boolean && bool2->boolean);

                        return make_pair(true, dataType::BoolT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            case tokenKey::Or:
            case tokenKey::OrW:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::BoolT: {
                        shared_ptr<boolContainer> bool1 = dynamic_pointer_cast<boolContainer>(binary_op->expression1);
                        shared_ptr<boolContainer> bool2 = dynamic_pointer_cast<boolContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(bool1->boolean || bool2->boolean);

                        return make_pair(true, dataType::BoolT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }
            case tokenKey::Xor:
            case tokenKey::XorW:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::BoolT: {
                        shared_ptr<boolContainer> bool1 = dynamic_pointer_cast<boolContainer>(binary_op->expression1);
                        shared_ptr<boolContainer> bool2 = dynamic_pointer_cast<boolContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(bool1->boolean ^ bool2->boolean);

                        return make_pair(true, dataType::BoolT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }
            case tokenKey::Greater:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(int1->number > int2->number);

                        return make_pair(true, dataType::BoolT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }
            case tokenKey::Less:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(int1->number < int2->number);

                        return make_pair(true, dataType::BoolT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }
            case tokenKey::Equals:
            case tokenKey::Is:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(int1->number == int2->number);

                        return make_pair(true, dataType::BoolT);
                    }

                    case dataType::BoolT: {
                        shared_ptr<boolContainer> bool1 = dynamic_pointer_cast<boolContainer>(binary_op->expression1);
                        shared_ptr<boolContainer> bool2 = dynamic_pointer_cast<boolContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(bool1->boolean == bool2->boolean);

                        return make_pair(true, dataType::BoolT);
                    }

                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }
            case tokenKey::GrEqual:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(int1->number >= int2->number);

                        return make_pair(true, dataType::BoolT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            case tokenKey::LessEqual:
//              Optimize and typecheck the binary expressions.
                optimized_type = _binaryop_optimize_typecheck(binary_op, scope_stack);

                if (!optimized_type.first) {
                    return make_pair(false, optimized_type.second);
                }

//              Extract the expression types in order to retrieve class member variables.
                switch (optimized_type.second) {
                    case dataType::IntT: {
                        shared_ptr<intContainer> int1 = dynamic_pointer_cast<intContainer>(binary_op->expression1);
                        shared_ptr<intContainer> int2 = dynamic_pointer_cast<intContainer>(binary_op->expression2);
                        value_data = make_shared<boolContainer>(int1->number <= int2->number);

                        return make_pair(true, dataType::BoolT);
                    }
                    default:
                        throw InavlidOperatorError(binary_op->op, optimized_type.second);
                }

            default:
                throw FatalError("binary operator not recognized");
        }

    } else if (ternaryOp* ternary_op = dynamic_cast<ternaryOp*>(value_data.get())) { // ternaryOp
        pair<bool, dataType> optimized_type_condition, optimized1_type1, optimized2_type2;

//      Optimize each ternary operator expression.
        optimized_type_condition = optimize_typecheck_value_data(ternary_op->expression2, scope_stack);
        optimized1_type1 = optimize_typecheck_value_data(ternary_op->expression1, scope_stack);
        optimized2_type2 = optimize_typecheck_value_data(ternary_op->expression3, scope_stack);

//      For each operator, check the expression types and perform the operation if optimizable.
        switch (ternary_op->op) {
            case tokenKey::If:
//              Ensure the condition is a boolean and the expressions have matching types.
                if (optimized_type_condition.second != dataType::BoolT) {
                    throw ConditionMismatchError(optimized_type_condition.second, true);
                } else if (optimized1_type1.second != optimized2_type2.second) {
                    throw TypeMismatchError(ternary_op->op, optimized1_type1.second, optimized2_type2.second);
                }

//              If the condition could be optimized, replace the ternary if with whichever expression should be executed.
                if (optimized_type_condition.first) {
                    shared_ptr<boolContainer> condition = dynamic_pointer_cast<boolContainer>(ternary_op->expression2);

                    if (condition->boolean) {
                        value_data = move(ternary_op->expression1);
                        return optimized1_type1;
                    } else {
                        value_data = move(ternary_op->expression3);
                        return optimized2_type2;
                    }

                }

                return make_pair(false, optimized1_type1.second);

            default:
                throw FatalError("ternary operator not recognized");
        }
    } else if (varContainer* var_container = dynamic_cast<varContainer*>(value_data.get())) { // varContainer
        map<string, variableInfo>::iterator iter;

//      Find the variable in the current scope or any parent scope.
        shared_ptr<stack> current_scope = scope_stack;
        iter = current_scope->locals.find(var_container->variable);
        while (iter == current_scope->locals.end()) {
            if (current_scope->parent_scope != nullptr) {
                current_scope = current_scope->parent_scope;
                iter = current_scope->locals.find(var_container->variable);
            } else {
                throw VariableNotInitializedError(var_container->variable);
            }
        }

        value_data = iter->second.value;
        return make_pair(iter->second.optimize_value, iter->second.type);

    } else if (dynamic_cast<intContainer*>(value_data.get())) { // intContainer
        return make_pair(true, dataType::IntT);
    } else if (dynamic_cast<boolContainer*>(value_data.get())) { // boolContainer
        return make_pair(true, dataType::BoolT);
    } else {
        throw FatalError("value data not recognized during optimization");
    }
}
