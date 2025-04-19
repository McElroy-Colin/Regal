/*

Declarations for interpreter data display functions.

*/

#ifndef DISPLAY_UTILS_HPP
#define DISPLAY_UTILS_HPP

#include "inc_interpreter/interp_utils.hpp"
#include "inc_internal/error_handling.hpp"


/*
Create a display string for a given token. 
Use a boolean display setting to determine how the display string is created.

Throw a fatal error if the given token has an unrecognized (unimplemented) tokenKey.

Parameters:
    disp_token: token to create a display string for (input)
    literal: true if the display string should reflect the exact input of the user (input)
        e.g. with  literal = true,  (tokenKey::Int32, 12, line_num)  ->  "12"
                   literal = false, (tokenKey::Int32, 12, line_num)  ->  "<int>"

               or  literal = true,  (tokenKey::Plus, false (default), line_num)  ->  "+"
                   literal = false, (tokenKey::Plus, false (default), line_num)  ->  "add"

Return the created display string.
*/
const std::string display_token(const TokenDef::token& disp_token, const bool literal);

/*
Create a display string for a given data type.

Throw a fatal error if the given data type is unrecognized (unimplemented).

Parameters:
    disp_token: data type to create a display string for (input)
    line_number: the line number that the type is referenced on (input)

Return the created display string.
*/
const std::string display_type(const TypingUtils::dataType type, const std::uint32_t line_number);


#endif
