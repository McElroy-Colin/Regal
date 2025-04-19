/*

Standard I/O function implementations.

*/

#include "inc_stdlib/stdio.hpp"

// Standard library aliases
using std::string, std::shared_ptr, std::is_base_of_v, std::visit, std::cout, std::endl;

// interp_utils namespace
using namespace CodeTree;


inline void print(const irreducibleData* const data, const string prefix, const string suffix) noexcept {
    cout << prefix << to_string(data) << suffix << endl;

    return;
}