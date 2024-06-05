// Header file declaring Regal tokens.

#include <vector>  
#include <string>

#ifndef TOKENS_HPP
#define TOKENS_HPP

// Lexing type aliases.
using String = std::string;
using IntArray = std::vector<int>;
using DataArray = std::vector<std::variant<int, String>>;
using TokenList = std::list<DataArray>;

// Enum for all tokens in Regal.
enum Token {
    // Keywords
    Let, Now,

    // Primitive data types
    Int,

    // Primitive Operators
    Plus, Minus, Mult, Div, Exp,

    // Variables
    Var,
    Bind,

    // Groupers
    LeftPar, RightPar,

    // Nothing token
    Nothing
};

#endif