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

enum ReturnValue {
    FAILURE = -1,
};

using String = std::string;

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

// Store the display string of the given token.
//      token: token to display (input)
//      display: display string of the token (output)
//      subset: true if the token represents a subset (input)
void display_token(const TokenKey token, String& display, const bool subset = false) {
    switch (token) {
        case TokenKey::Let:
            if (subset) {
                display = "<keywords>";
            } else {
                display = "let";
            }
            break;

        case TokenKey::Now:
            if (subset) {
                display = "<keywords>";
            } else {
                display = "now";
            }
            break;

        case TokenKey::Int:
            if (subset) {
                display = "<number>";
            } else {
                display = "<integer>";
            }
            break;

        case TokenKey::Plus:
            if (subset) {
                display = "<additive operator>";
            } else {
                display = "+";
            }
            break;

        case TokenKey::Minus:
            if (subset) {
                display = "<additive operator>";
            } else {
                display = "-";
            }
            break;

        case TokenKey::Mult:
            if (subset) {
                display = "<multiplicative operator>";
            } else {
                display = "*";
            }
            break;

        case TokenKey::Div:
            if (subset) {
                display = "<multiplicative operator>";
            } else {
                display = "/";
            }
            break;

        case TokenKey::Exp:
            display = "**";
            break;

        case TokenKey::Var:
            display = "<variable>";
            break;

        case TokenKey::Bind:
            display = "=";
            break;

        case TokenKey::LeftPar:
            display = "(";
            break;

        case TokenKey::RightPar:
            display = ")";
            break;

        case TokenKey::Nothing:
            display = "nothing";
            break;

        default:
            display = "<UNKNWON TOKEN>";
    }

    return;
}


// Useful subsets of tokens.
std::vector<TokenKey> keyword_tokens = {Let, Now};
std::vector<TokenKey> number_tokens = {Int};
std::vector<TokenKey> additive_tokens = {Plus, Minus};
std::vector<TokenKey> multiplicative_tokens = {Mult, Div};

// Token aliases.
using Token = std::vector<std::variant<TokenKey, int, String>>;

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