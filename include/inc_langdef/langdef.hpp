// Header file containing aliases and values for language definition.

#include <string>
#include <vector>
#include <variant>
#include <list>

#ifndef LANGDEF_HPP
#define LANGDEF_HPP

enum ReturnValue {
    FAILURE = -1,
};

using String = std::string;
using IntArray = std::vector<int>;

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

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

#endif