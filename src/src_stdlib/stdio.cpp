// Standard I/O functions.

#include "inc_stdlib/stdio.hpp"

using std::string, std::shared_ptr, std::is_base_of_v, std::visit, std::cout, std::endl;
using namespace CodeTree;
using namespace DebugUtils;

// Converts a piece of irreducible data to a string and stores it.
//      data: data to conver to string (input)
//      converted: string representation of data (output)
void to_string(const shared_ptr<irreducibleData>& data, string& converted) {
    converted = data->disp(tokenDispOption::Literal);
    return;
}

// Return the display string for a piece of irreducible data.
//      data: data to conver to string (input)
string to_string(const shared_ptr<irreducibleData>& data) {
    return data->disp(tokenDispOption::Literal);
}

// Prints a piece of irreducible data with optional prefix and suffix.
//      data_node: data to print (input)
//      prefix: string to prepend to the data (default empty) (input)
//      suffix: string to append to the data (default "\033[0m") (input)
void print(const shared_ptr<irreducibleData>& data_node, string prefix, string suffix) {
    string action_disp;

    to_string(data_node, action_disp);
    cout << prefix << action_disp << suffix << endl;

    return;
}