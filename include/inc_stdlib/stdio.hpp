// File containing standard I/O functions for Regal.

#ifndef STDIO_HPP
#define STDIO_HPP

#include <iostream>
#include <type_traits>
#include "inc_debug/compiler_debug.hpp"
#include "inc_debug/error_handling.hpp"


// Converts a piece of irreducible data to a string and stores it.
//      data: data to conver to string (input)
//      converted: string representation of data (output)
void to_string(const std::shared_ptr<CodeTree::irreducibleData>& data, std::string& converted);

// Return the display string for a piece of irreducible data.
//      data: data to conver to string (input)
std::string to_string(const std::shared_ptr<CodeTree::irreducibleData>& data);

// Prints a piece of irreducible data with optional prefix and suffix.
//      data_node: data to print (input)
//      prefix: string to prepend to the data (default empty) (input)
//      suffix: string to append to the data (default "\033[0m") (input)
void print(const std::shared_ptr<CodeTree::irreducibleData>& data_node, std::string prefix = "", std::string suffix = "\033[0m");

#endif