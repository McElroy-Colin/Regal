// This file contains a simple text envirnoment to assign and print variables.
//   Variables can take any integer or mathematical expression of integers. 
//   Assigned variables will be recognized and parsed, so variables can 
//   reference other variables.
//   Print a variable "var" with 'print(var)'.
//   Entering 'done' closes the environment.
//   Entering 'clear' wipes current memory/assigned variables.


#include "../include/inc_langdef/optimizer.hpp"
#include "../include/inc_langdef/langdef.hpp"
#include "../stdlib/stdio.hpp"

#include "../src/src_langdef/src_lexer/lexer.cpp"
#include "../src/src_langdef/src_parser/parser.cpp"
#include "../src/src_langdef/src_optimizer/optimizer.cpp"
#include "../src/src_langdef/src_executor/executor.cpp"

// Anonymous namespace for helper functions.
namespace {

//  Move the terminal cursor to the given row and column.
//      row: row to move the cursor to (input)
//      col: column to move the cursor to (input)
    void _move_cursor(const int row, const int col) {
        std::cout << "\033[" << row << ";" << col << "H";
    }

//  Clear the given number of rows after the cursor position. 
//  The current row only clears text after (to the right of) the cursor.
//      rows: the number of rows, including the current row, after the cursor to clear (input)
    void _clear_next_rows(const int rows) {
        for (int i = 0; i < rows; i++) {
            std::cout << "\033[K";
            if (i < rows - 1) {
                std::cout << "\033[B\033[0G";
            }
        }
    }

//  Display a given line error message.
//      line: the problematic line of code (input)
//      message: the error message to display (input)
//      suffix_length: length of the suffix on line numbers in the environment (input)
//          (e.g. '1>  let x = 4', ">  " is the suffix)
    void _display_error(const String& line, const String& message, const int line_pos, const int suffix_length) {
        _clear_next_rows(4);
        _move_cursor(line_pos - 1, suffix_length + 1);
        std::cout << "\n\n" << String(suffix_length + 1, ' ') << "\"" << line << "\"";
        std::cout << "\n\033[31m" << String(suffix_length + 1, ' ') << message << "\033[0m";
        _move_cursor(line_pos - 1, suffix_length + 1);
        _clear_next_rows(1);
    }

}

// Generate a simple text environment in the terminal to write lines of Regal code.
//      display_msg: UI message to display at the top of the environment (input)
//      linenum_suffix: text after the line number but before code (input)
//          (e.g. '1>  let x = 4', ">  " is the linenum_suffix)
void generate_playground(const String& display_msg, const String& linenum_suffix) {
//  Display the UI message.
    int ui_lines = 1;

    for (char c : display_msg) {
        if (c == '\n') {
            ui_lines += 1;
        }
    }

    std::cout << display_msg;


//  Initialize variables for text environment loop.
    String curr_line;
    std::map<String, Action> stack;

    int line_size;
    int line_number = 1;
    int line_pos = ui_lines;

    const int suffix_length = linenum_suffix.length();

    bool errored = false;

//  Loop to generate a live text environment.
    while (true) {
        std::list<Token> tokens;
        Action code;

//      Clear current line and read the next input.
        _move_cursor(line_pos, 1);
        _clear_next_rows(1);
        std::cout << line_number << linenum_suffix;
        if (!std::getline(std::cin, curr_line)) {
            break;
        }

        line_number += 1;
        line_pos += 1;

        line_size = curr_line.size();

        if (curr_line == "done") {
            if (errored) {
                _move_cursor(line_pos + 4, 1);
            } else {
                _move_cursor(line_pos + 1, 1);
            }

            break;
        }

        errored = false;

//      The print function is currently hard-coded in.
        if ((curr_line.substr(0, 6) == "print(") && (curr_line[line_size - 1] == ')')) {
            String var = curr_line.substr(6, line_size - 7);
            auto it = stack.find(var);
            if (it == stack.end()) {
                String error_msg = "variable \'" + var + "\' not initialized";
                _display_error(curr_line, error_msg, line_pos, suffix_length);

                line_number -= 1;
                line_pos -= 1;

                errored = true;
            } else {
                _clear_next_rows(4);
                _move_cursor(line_pos, 0);
                std::cout << String(suffix_length + 1, ' ');
                print(stack[var], "\033[32m");
                
                line_pos += 1;
            }
            
        } else if (curr_line == "clear") {
            stack.clear();

            line_number = 1;
            line_pos = ui_lines;

            std::cout << "\033[" << ui_lines << ";1H\033[J";
//      If no specific command is given, parse the line as Regal code.
        } else {
            try {
                lex_string(curr_line, tokens);
                code = parse_line(tokens);
                optimize_action(code, stack);
                execute_action(code, stack);

                _clear_next_rows(4);
            }
            catch (const std::exception& e) {
                String error_msg = e.what();
                _display_error(curr_line, error_msg, line_pos, suffix_length);

                line_number -= 1;
                line_pos -= 1;
                
                errored = true;
            }
        }
    }

    return;
}


// Creates simple text environment to assign and print variables.
int main() {
//  Environment parameters are malleable.
    const String pground_display_msg = 
        "\033[2J\033[1;1H\nRegal Playground V0.1:\n\n"

        "  i) type 'done' to quit\n"
        "  ii) type 'clear' to clear variable memory\n\n"

        "Enter code below line by line:\n\n";

    const String linenum_suffix = ">  ";

//  With this configuration, a line will look like:
//      1>  [code]

    generate_playground(pground_display_msg, linenum_suffix);

    return 0;
}
