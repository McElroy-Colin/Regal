/*

Standard I/O function declarations.

*/

#ifndef STDIO_HPP
#define STDIO_HPP

#include <iostream>
#include <type_traits>

#include "inc_internal/display_utils.hpp"
#include "inc_internal/error_handling.hpp"

/*
Convert a piece of irredicible data to a string.

Parameters:
    data: data to convert to a string (input)

Return the display string for the piece of data.
*/
inline const std::string to_string(const CodeTree::irreducibleData* data) noexcept {
    return data->disp();
}

// Prints a piece of irreducible data with optional prefix and suffix.
//      data_node: data to print (input)
//      prefix: string to prepend to the data (default empty) (input)
//      suffix: string to append to the data (default "\033[0m") (input)

/*
Print a display string for a piece of irreducible data to standard output.

Parameters:
    data: piece of data to print (input)
    prefix: string to prepend to the display string (default "") (input)
    suffix: string to append to the display string (default "\033[0m") (input)
            the default string turns future printed text white automatically
*/
void print(const CodeTree::irreducibleData* const data, const std::string prefix = "", const std::string suffix = "\033[0m") noexcept;

#endif