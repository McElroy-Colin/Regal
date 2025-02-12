// File containing standard I/O functions for Regal.

#include "../include/inc_langdef/langdef.hpp"

#ifndef STDIO_HPP
#define STDIO_HPP


// Anonymous namespace containing stdio helper functions.
namespace {

// TODO: use OO paradigm to not have an if statement for all data types

//  Converts a piece of irreducible data to a string and stores it.
//      code_tree: data to conver to string (input)
//      converted: string representation of data (output)
    void _to_string(const syntaxNode& code_tree, string& converted) {
        converted = std::visit([](const auto& a) { return a->disp(Literal); }, code_tree);
        return;
    }

}

// Prints a piece of primitive data with optional prefix and suffix.
//      code_tree: data to print (input)
//      prefix: string to prepend to the data (input)
//      suffix: string to append to the data (input)
void print(const syntaxNode& code_tree, string prefix = "", string suffix = "\033[0m") {
    string action_disp;

    _to_string(code_tree, action_disp);
    std::cout << prefix << action_disp << suffix << std::endl;

    return;
}

#endif