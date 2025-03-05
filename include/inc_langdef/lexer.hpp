// Header file containing the Regal lexer dependencies and structure.

#ifndef LEXER_HPP
#define LEXER_HPP

#include <list>
#include <tuple>
#include "inc_langdef/langdef.hpp"
#include "inc_debug/error_handling.hpp"


// Width of current environment tab character in spaces, for checking indent amounts.
constexpr int TAB_WIDTH = 4;

// Lex a string into a list of Regal syntax tokens.
// Stores a list of arrays, with each array containing the token enum value and any additional necessary data.
//      input: the code to lex (input)
//      token_list: list of tokens from the given input (output)
void lex_string(std::string& input, std::list<TokenDef::token>& token_list);

#endif