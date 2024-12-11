#include "../include/inc_langdef/langdef.hpp"
#include <iostream>
#include <type_traits>


// Converts a piece of primitive data to a string.
String _to_string(const Action& action) {
    String result;
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) {
        auto integer = std::get<std::shared_ptr<Integer>>(action);
        integer->_disp(result);
        return result;
    } else {
        perror("Cannot print undisplayable data.");
        throw std::exception();
    }
    
}

// Prints a piece of primitive data.
void print(const Action& action) {
    std::cout << _to_string(action) << std::endl;
    return;
}