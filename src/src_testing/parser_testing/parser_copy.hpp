// Header file containing aliases, values and structs for parsing and AST generation.

#include "../../../include/inc_langdef/langdef.hpp"
#include <memory>

#ifndef PARSER_COPY_HPP
#define PARSER_COPY_HPP

// Enum for other types of AST nodes that do not need a struct.
enum OtherNodes_c {
    ON_nothing_c
};

// AST node definitions.
struct Integer_c;
struct Variable_c;
struct BinaryOperator_c;
struct Assign_c;
struct Reassign_c;
struct Nothing_c;

// Variant using shared pointers to allow the AST to have recursive, type-safe nodes.
using Action_c = std::variant<
    std::shared_ptr<Integer_c>, // 0
    std::shared_ptr<Variable_c>, // 1
    std::shared_ptr<BinaryOperator_c>, // 2 
    std::shared_ptr<Assign_c>, // 3
    std::shared_ptr<Reassign_c>, // 4
    OtherNodes_c, // 5
    int
>;

// Struct for an Integer node.
struct Integer_c {
    int number;

    Integer_c() : number(0) {}

    Integer_c(int n) : number(n) {}

    Integer_c(const Integer_c& other) : number(other.number) {}

    void update(int replacement) {
        number = replacement;
    }
};

// Struct for a Variable node.
struct Variable_c {
    String variable;

    Variable_c() : variable("") {}

    Variable_c(String var) : variable(var) {}

    Variable_c(Variable_c&& other) noexcept : variable(std::move(other.variable)) {}

    Variable_c(const Variable_c& other) : variable(other.variable) {}
};

// Struct for a binary operator node.
struct BinaryOperator_c {
    TokenKey op;
    Action_c expression1, expression2;

    BinaryOperator_c() : op(Nothing), expression1(0), expression2(0) {}

    BinaryOperator_c(TokenKey op, Action_c e1, Action_c e2) : op(op), expression1(e1), expression2(e2) {}

    BinaryOperator_c(BinaryOperator_c&& other) noexcept : op(other.op),
        expression1(std::move(other.expression1)),
        expression2(std::move(other.expression2)) {}

    BinaryOperator_c(const BinaryOperator_c& other) : 
        op(other.op), expression1(other.expression1), 
        expression2(other.expression2) {}
};

// Struct for an assignment node.
struct Assign_c {
    String variable;
    Action_c expression;

    Assign_c() : variable(""), expression(0) {}

    Assign_c(const String& v, const Action_c& e) : variable(v), expression(e) {}

    Assign_c(Assign_c&& other) : variable(std::move(other.variable)), 
        expression(std::move(other.expression)) {}

    Assign_c(const Assign_c& other) : variable(other.variable), expression(other.expression) {}
};

// Struct for a reassignment node.
struct Reassign_c {
    String variable;
    Action_c expression;

    Reassign_c() : variable(""), expression(0) {}

    Reassign_c(String v, Action_c e) : variable(v), expression(e) {}

    Reassign_c(Reassign_c&& other) : variable(std::move(other.variable)), 
        expression(std::move(other.expression)) {}

    Reassign_c(const Reassign_c& other) : variable(other.variable), expression(other.expression) {}

};

#endif