#include "../include/inc_langdef/langdef.hpp"


// Converts a piece of primitive data to a string.
String _to_string(const Action& action) {
    String result;
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) {
        auto integer = std::get<std::shared_ptr<Integer>>(action);
        integer->_disp(result);
        return result;
    } else {
        throw std::runtime_error("Cannot print undisplayable data");
    }
    
}

// Prints a piece of primitive data.
void print(const Action& action) {
    std::cout << _to_string(action) << std::endl;
    return;
}