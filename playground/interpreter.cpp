/*

Text interpreter program. Take text from stdin and interpret it as Regal code.
Output error messages and interpretation times to stdout.

*/

#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>

#include "inc_interpreter/lexer.hpp"
#include "inc_interpreter/parser.hpp"
#include "inc_interpreter/semantic_analysis.hpp"
#include "inc_stdlib/stdio.hpp"

// Standard library aliases
using std::string, std::list, std::shared_ptr, std::ostringstream, std::exception, std::pair, std::cin, std::cout, std::cerr,
      std::make_shared, std::static_pointer_cast, std::fixed, std::make_pair, std::flush, std::tie;

// Standard library namespace
using namespace std::chrono;

// interp_utils namespaces
using namespace TokenDef;
using namespace CodeTree;

// semantic_analysis namespace
using namespace DataStorage;


/*
Output a display string for the local variables in the given environment.

Parameters: 
    env: environment to display (input)
*/
void _display_locals(const environment* const env) noexcept {
    string display_str = "Constants:";
//  Iterate over the current scope_stack's variables.
    for (const auto& [var, expr] : env->locals) {
//      Ensure the variable was reduced at interpretation-time (pre-runtime).
        if (expr.optimize_value) {
            const irreducibleData* const curr_data = static_cast<irreducibleData*>(expr.value.get());

//          Add the variable's display.
            display_str += "\n   " + display_type(expr.type, 0) + " " + var + ": " + to_string(curr_data);
//      If the variable was not reduced pre-runtime, display a default message.
        } else {
            display_str += "\n   cannot display variable \'" + var + "\'";
        }
    }

//  Print the display to stdout.
    cout << display_str;

    return;
}

/*
Output a dispolay string for the given parsing and analysis times.

Parameters:
    parsing_time: time in nanoseconds taken for parsing (input)
    analysis_time: time in nanoseconds taken for semantic analysis (input)
*/
inline void _display_time(const double parsing_time, const double analysis_time) noexcept {
    cout << "Parsing: " << fixed << parsing_time / 1e9 << " s\n" 
         << "Semantic Analysis: " << fixed << analysis_time / 1e9 << " s"
         << flush;
}

/*
Store text from stdin in the given text parameter.

Parameters: 
    text: string object to store input text (output)
*/
inline void read_text(string& text) noexcept {
    ostringstream buffer;
    buffer << cin.rdbuf();
    text = buffer.str();
    return;
}

/*
Interpret the given text as Regal code and update the given environment.
Output an error message if the code was not valid.

Exit the program if the given text throws an error from parsing or analysis.

Parameters:
    text: text to interpret (input)
    env: environment pointer to fillduring analysis (output)
    
Return a pair containing
    first: time in nanoseconds taken to parse the code
    second: time in nanoseconds taken to analyze the code
*/
const pair<double, double> interpret_text(string& text, shared_ptr<environment>& env) {
    _V2::system_clock::time_point start_time, parsing_time, end_time;

    try {
//      Start time.
        start_time = high_resolution_clock::now();

//      Parse the code.
        list<token> token_list = lex_string(text);
        shared_ptr<dataNode> parsed_code = parse_file(token_list);

//      End time for parsing, start time for analysis.
        parsing_time = high_resolution_clock::now();

//      Perform semantic analysis.
        analyze_data_node(parsed_code, env, true);

//      Stop time.
        end_time = high_resolution_clock::now();

//  Catch exceptions, print their error messages to stdout and exit.
    } catch (const exception& e) {
        cerr << e.what() << flush;
        exit(EXIT_FAILURE);
    }

//  Return a pair containing the nanosecond times taken.
    return make_pair(duration_cast<nanoseconds>(parsing_time - start_time).count(),
                     duration_cast<nanoseconds>(end_time - parsing_time).count());
}

/*
Interpret text from stdin as Regal code. Output an error message if the code was invalid
or output the environment status and interpretation time.
*/
int main() {
    string code;
    shared_ptr<environment> env = make_shared<environment>();
    double parsing_time, analysis_time;

//  Store code in the string variable.
    read_text(code);
//  Interpret the code and update the environment.
    tie(parsing_time, analysis_time) = interpret_text(code, env);
//  Display the environment.
    _display_locals(env.get());

//  Output a delimeter to separate the environment display from the time display.
    cout << "$$$";

//  Display the time taken to interpret.
    _display_time(parsing_time, analysis_time);

    return 0;
}
