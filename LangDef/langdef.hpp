// Header file containing aliases and values for language definition.

#include <string>
#include <vector>  
#include <variant>
#include <tuple>
#include <list>

#ifndef LANGDEF_HPP
#define LANGDEF_HPP

enum ReturnValue {
    FAILURE = -1,
};

// Lexing and parsing type aliases.
using String = std::string;
using IntArray = std::vector<int>;
using Token = std::vector<std::variant<int, String>>;
using TokenArray = std::vector<Token>;
using TokenList = std::list<Token>;

template<typename Type1, typename Type2>
using Tuple = std::tuple<Type1, Type2>;
template<typename Type>
using extract = std::get<Type>;

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

// Vector alias for token keys.
using TokenKeyArray = std::vector<TokenKey>;

// TokenKey subsets for parsing.
TokenKeyArray assignment_keywords = { Let, Now };
TokenKeyArray primitive_operators = { Plus, Minus, Mult, Div, Exp };
TokenKeyArray additive_operators = { Plus, Minus };
TokenKeyArray multiplicative_operators = { Mult, Div };

// Enum for actions that the parser and evaluator must perform.
enum Action {
//  Variables
    Assign, Reassign,

//  Primitive Operators
    Add, Subtract, Multiply, Divide, Exponential
};

// Alias representing the instructions sent from the parser to the evaluator.
// While functionally the same as TokenList, each vector will contain an Action and its necessary data.
//     Example: { Add, 3, 6 } represents the action of 3 + 6
using ActionList = std::list<std::vector<std::variant<int, String>>>;

#endif