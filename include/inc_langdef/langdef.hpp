// Header file containing aliases and values for language definition.

#include <string>
#include <vector>
#include <variant>
#include <list>
#include <memory>
#include <typeindex>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#ifndef LANGDEF_HPP
#define LANGDEF_HPP

using String = std::string;

// Enum for all tokens in Regal.
enum TokenKey {
//  Keywords
    Let, Now,

//  Primitive data types
    Int, Bool,

//  Primitive Operators
    Plus, Minus, Mult, Div, Exp, And, Or, Not, Greater, Less, /*Equals,*/ 

//  Variables
    Var,
    Equals,

//  Groupers
    LeftPar, RightPar,

//  Nothing token
    Nothing
};

// Useful subsets of tokens.
std::vector<TokenKey> keyword_tokens = {Let, Now};
std::vector<TokenKey> number_tokens = {Int};

// Token aliases.
using Token = std::vector<std::variant<TokenKey, int, String>>;

// AST node definitions.
struct Integer;
struct Boolean;
struct Variable;
struct UnaryOperator;
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
    std::shared_ptr<Boolean>,
    std::shared_ptr<Variable>, 
    std::shared_ptr<UnaryOperator>, 
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

// Struct for a Boolean node.
struct Boolean {
    bool boolean;

    Boolean() : boolean(false) {}

    Boolean(bool b) : boolean(b) {}

    Boolean(const Boolean& other) : boolean(other.boolean) {}

    void _disp(String& result) {
        result = boolean ? "true" : "false";
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

    void _disp(String& result) {
        result = variable;
        return;
    }
};

// Struct for a unary operator node.
struct UnaryOperator {
    TokenKey op;
    Action expression;

    UnaryOperator() : op(Nothing), expression() {}

    UnaryOperator(TokenKey op, Action e) : op(op), expression(e) {}

    UnaryOperator(UnaryOperator&& other) noexcept : op(other.op),
        expression(std::move(other.expression)) {}

    UnaryOperator(const UnaryOperator& other) : 
        op(other.op), expression(other.expression) {}
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
    bool implicit;

    Reassign() : variable(""), expression(), implicit(false) {}

    Reassign(String v, Action e, bool i) : variable(v), expression(e), implicit(i) {}

    Reassign(Reassign&& other) : variable(std::move(other.variable)), 
        expression(std::move(other.expression)), implicit(other.implicit) {}

    Reassign(const Reassign& other) : variable(other.variable), expression(other.expression), implicit(other.implicit) {}
};

#endif