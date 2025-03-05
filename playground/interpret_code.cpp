// Primary file that takes code from stdin and interprets it.

#include <sstream>
#include <iostream>
#include <chrono>
#include "inc_langdef/lexer.hpp"
#include "inc_langdef/parser.hpp"
#include "inc_langdef/optimizer.hpp"
#include "inc_stdlib/stdio.hpp"

using std::string, std::list, std::shared_ptr, std::ostringstream, std::exception, 
      std::make_shared, std::static_pointer_cast, std::cin, std::cout, std::cerr, std::flush;
using namespace std::chrono;
using namespace TokenDef;
using namespace CodeTree;
using namespace DataStorage;
using namespace DebugUtils;

// Print a display of the local variable stack to stdout.
//      scope_stack: Reference to the current local stack (input)
void _display_locals(shared_ptr<stack>& scope_stack) {
    string display_str = "Locals:";
//  Iterate over the current scope_stack's variables.
    for (const auto& [var, value] : scope_stack->locals) {
//      Ensure the variable was reduced at interpretation-time (pre-runtime).
        if (value.optimize_value) {
            shared_ptr<irreducibleData> curr_data = static_pointer_cast<irreducibleData>(value.value);

            if (!curr_data) {
                cerr << "\nERROR: dynamic cast failed on \'" << var << "\'";
                exit(EXIT_FAILURE);
            }

//          add the variable's display.
            display_str += "\n   " + display_type(value.type) + " " + var + ": " + to_string(curr_data);
//      If the variable was not reduced pre-runtime, display adefault message.
        } else {
            display_str += "\n   cannot display variable \'" + var + "\'";
        }
    }

//  Print the display to stdout.
    cout << display_str;

    return;
}

// Print a display of the given time duration.
//      duration: double representing time in nanoseconds (input)
void _display_time(const double duration) {
//  Include the "$$$" delimiter so a GUI can separate interpretation time display from other displays.
    cout << "$$$Interpretation time: " << duration << " ns" << flush;
}

// Assign stdin text to the output variable. Text includes all special characters.
//      text: string to store stdin data (output)
void read_text(string& text) {
    ostringstream buffer;
    buffer << cin.rdbuf();
    text = buffer.str();
    return;
}

// Interpret the contents of a file of Regal code, updating the stack and optimizing a code tree.
// Return the time taken in nanoseconds to interpret the given code.
// Print any exceptions caught during interpretation to stderr and exit on exception.
//      file_contents: string representing the code in the file (input)
//      scope_stack: stack variable to be updated as code is optimized at interpretation-time (output)
double interpret_file(string& file_contents, shared_ptr<stack>& scope_stack) {
    list<token> token_list;
    shared_ptr<dataNode> parsed_code;
    _V2::system_clock::time_point start_time, end_time;

    try {

//      Time the interpretation.
        start_time = high_resolution_clock::now();

//      Interpret the code by lexing, parsing, then optimizing.
        lex_string(file_contents, token_list);
        parsed_code = parse_file(token_list);
        optimize_typecheck_data_node(parsed_code, scope_stack, true);

//      Stop time.
        end_time = high_resolution_clock::now();

//  Catch exceptions, print their error messages to stdout and exit.
    } catch (const exception& e) {
        cerr << e.what() << flush;
        exit(EXIT_FAILURE);
    }

    return duration_cast<nanoseconds>(end_time - start_time).count();
}

int main() {
    string code;
    shared_ptr<stack> scope_stack = make_shared<stack>();
    double interpretation_time;

//  Store code in the string variable.
    read_text(code);
//  Interpret the code and update the stack.
    interpretation_time = interpret_file(code, scope_stack);
//  Display the stack.
    _display_locals(scope_stack);
//  Display the time taken to interpret.
    _display_time(interpretation_time);

    return 0;
}
