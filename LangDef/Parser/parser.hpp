// Header file containing aliases, values and structs for parsing and AST generation.

#include "../langdef.hpp"
#include <memory>

#ifndef PARSER_HPP
#define PARSER_HPP

// Enum for potential operations.
enum Operator {
    None, Add, Subtract, Multiply, Divide, Exponential
};

// Enum for other types of AST nodes that do not need a struct.
enum OtherNodes {
    ON_nothing
};

// AST node definitions.
struct Integer;
struct Variable;
struct BinaryOperator;
struct Assign;
struct Reassign;
struct Nothing;

// Variant using shared pointers to allow the AST to have recursive, type-safe nodes.
using Action = std::variant<
    std::shared_ptr<Integer>, 
    std::shared_ptr<Variable>, 
    std::shared_ptr<BinaryOperator>, 
    std::shared_ptr<Assign>, 
    std::shared_ptr<Reassign>, 
    OtherNodes,
    int
>;

// Struct for an Integer node.
struct Integer {
    int number;

    Integer() : number(0) {}

    Integer(int n) : number(n) {}

    Integer(const Integer& other) : number(other.number) {}
};

// Struct for a Variable node.
struct Variable {
    String variable;

    Variable() : variable("") {}

    Variable(String var) : variable(var) {}

    Variable(const Variable& other) : variable(other.variable) {}
};

// Struct for a binary operator node.
struct BinaryOperator {
    Operator op;
    Action expression1, expression2;

    BinaryOperator() : op(None), expression1(0), expression2(0) {}

    BinaryOperator(Operator op, Action e1, Action e2) : op(op), expression1(e1), expression2(e2) {}

    BinaryOperator(const BinaryOperator& other) : 
        op(other.op), expression1(other.expression1), 
        expression2(other.expression2) {}
};

// Struct for an assignment node.
struct Assign {
    String variable;
    Action expression;

    Assign() : variable(""), expression(0) {}

    Assign(const String& v, const Action& e) : variable(v), expression(e) {}

    Assign(const Assign& other) : variable(other.variable), expression(other.expression) {}
};

// Struct for a reassignment node.
struct Reassign {
    String variable;
    Action expression;

    Reassign() : variable(""), expression(0) {}

    Reassign(String v, Action e) : variable(v), expression(e) {}

    Reassign(const Reassign& other) : variable(other.variable), expression(other.expression) {}
};

#endif