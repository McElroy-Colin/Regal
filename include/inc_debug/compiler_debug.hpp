// Header file containing internal interpreter debugging utilities.

#ifndef COMPILER_DEBUG_HPP
#define COMPILER_DEBUG_HPP

#include "inc_langdef/langdef.hpp"
#include "inc_debug/error_handling.hpp"


// Namespace for utilities in debugging and displaying Regal code.
namespace DebugUtils {
    
//  Display options for pieces of data.
    enum class tokenDispOption {
        Literal,     // display as input by the user (e.g. 'let' is 'let', '4' is '4')
        Key,         // display key name (e.g. 'let' is 'let', '4' is '<integer>')
        Subset       // display the set of tokens the data belongs to (e.g. 'let' is '<keyword>', '4' is '<number>')
    };
    
//  Store the display string of the given token.
//       disp_token: token to display (input)
//       disp_option: controls the display string for the given token (input)
//       display_str: display string of the token (output)
    void display_token(const TokenDef::token& disp_token, const tokenDispOption disp_option, std::string& display_str);

//  Return the display string of the given token.
//       disp_token: token to display (input)
//       disp_option: controls the display string for the given token (input)
    std::string display_token(const TokenDef::token& disp_token, const tokenDispOption disp_option);

//  Store the display string for the given type.
//      type: type to display (input)
//      display_str: string to store the type's display string (output)
    void display_type(const TypingUtils::dataType type, std::string& display_str);

//  Return the display string for the given type.
//      type: type to display (input)
    std::string display_type(const TypingUtils::dataType type);
}

#endif
