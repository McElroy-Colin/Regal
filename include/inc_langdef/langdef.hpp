// Header file containing aliases and values for language definition.

#include <string>
#include <vector>
#include <variant>
#include <list>
#include <memory>
#include <typeindex>
#include <iostream>

#ifndef LANGDEF_HPP
#define LANGDEF_HPP

enum ReturnValue {
    FAILURE = -1,
};

using String = std::string;
using IntArray = std::vector<int>;

// Enum for all tokens in Regal.
enum TokenKey {
//  Keywords
    Let, Now,

//  Primitive data types
    Int,

//  Primitive Operators
    Plus, Minus, Mult, Div, Exp,

//  Variables
    Var,
    Bind,

//  Groupers
    LeftPar, RightPar,

//  Nothing token
    Nothing
};

// Useful subsets of tokens.
using TokenKeyArray = std::vector<TokenKey>;
TokenKeyArray keyword_tokens = {Let, Now};
TokenKeyArray number_tokens = {Int};
TokenKeyArray additive_tokens = {Plus, Minus};
TokenKeyArray multiplicative_tokens = {Mult, Div};

// Token aliases.
using Token = std::vector<std::variant<TokenKey, int, String>>;
using TokenArray = std::vector<Token>;
using TokenList = std::list<Token>;

// AST node definitions.
struct Integer;
struct Variable;
struct BinaryOperator;
struct Assign;
struct Reassign;

// Nodes that do not require a class/struct.
enum OtherNodes {
    ON_Nothing
};

// Variant using shared pointers to allow the AST to have recursive, type-safe nodes.
using Action = std::variant<
    std::shared_ptr<Integer>, 
    std::shared_ptr<Variable>, 
    std::shared_ptr<BinaryOperator>, 
    std::shared_ptr<Assign>, 
    std::shared_ptr<Reassign>,
    OtherNodes
>;

// Struct for an Integer node.
struct Integer {
    int number;

    Integer() : number(0) {}

    Integer(int n) : number(n) {}

    Integer(const Integer& other) : number(other.number) {}
    
    void _disp(String& result) {
        result = std::to_string(number);
        return;
    }
};

// Struct for a Variable node.
struct Variable {
    String variable;

    Variable() : variable("") {}

    Variable(String var) : variable(var) {}
    
    Variable(Variable&& other) noexcept : variable(std::move(other.variable)) {}

    Variable(const Variable& other) : variable(other.variable) {}
};

// Struct for a binary operator node.
struct BinaryOperator {
    TokenKey op;
    Action expression1, expression2;

    BinaryOperator() : op(Nothing), expression1(), expression2() {}

    BinaryOperator(TokenKey op, Action e1, Action e2) : op(op), expression1(e1), expression2(e2) {}

    BinaryOperator(BinaryOperator&& other) noexcept : op(other.op),
        expression1(std::move(other.expression1)),
        expression2(std::move(other.expression2)) {}

    BinaryOperator(const BinaryOperator& other) : 
        op(other.op), expression1(other.expression1), 
        expression2(other.expression2) {}
};

// Struct for an assignment node.
struct Assign {
    String variable;
    Action expression;

    Assign() : variable(""), expression() {}

    Assign(const String& v, const Action& e) : variable(v), expression(e) {}

    Assign(Assign&& other) : variable(std::move(other.variable)), 
        expression(std::move(other.expression)) {}

    Assign(const Assign& other) : variable(other.variable), expression(other.expression) {}
};

// Struct for a reassignment node.
struct Reassign {
    String variable;
    Action expression;

    Reassign() : variable(""), expression() {}

    Reassign(String v, Action e) : variable(v), expression(e) {}

    Reassign(Reassign&& other) : variable(std::move(other.variable)), 
        expression(std::move(other.expression)) {}

    Reassign(const Reassign& other) : variable(other.variable), expression(other.expression) {}
};

#endif