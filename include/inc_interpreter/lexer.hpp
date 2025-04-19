/*

The lex_string function declaration and lexing-related constants.

*/

#ifndef LEXER_HPP
#define LEXER_HPP

#include <list>

#include "inc_interpreter/interp_utils.hpp"
#include "inc_internal/error_handling.hpp"


// Width of current environment tab character in spaces, for checking indent amounts.
constexpr std::uint8_t TAB_WIDTH = 4;

/*
Construct a list of tokens from a given string. A token is a 3-tuple of 
that looks like (tokenKey, optional data, line number).
    e.g.  "let"  ->  (tokenKey::Assign, false (default), line number)
           "12"  ->  (tokenKey::Int32, 12, line number)
         "var1"  ->  (tokenKey::Var, "var1", line number)
The list stores tokens in order of appearance in the given string.
Tokens are defined in interp_utils in the include folder and TokenList in the docs folder.

This function assumes that the given string is less than 2^32 characters long.

Throw an exception if
    an unrecognized sequence of characters is in the given string,
    a substring of digit characters contains more than one '.' character, 
    a substring of digit characters represents a number that is too large to represent with 64-bits (integer or floating-point), or
    there is an unclosed comment block (i.e. the number of "##" sequences in the input string is odd).

Parameters:
    input: string to construct a list of tokens from (input)

Return a list of tokens representing the given input string.
*/
const std::list<TokenDef::token> lex_string(std::string& input);

#endif
