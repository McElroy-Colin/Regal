// This file contains a simple text envirnoment to assign and print variables.
//   Variables can take any integer or matheatical expression of integers. 
//   Assigned variables will be recognized and parsed, so variables can 
//   reference other variables.
//   Entering 'done' closes the environment.
//   Entering 'clear' wipes current memory/assigned variables.


#include "../include/inc_langdef/inc_optimizer/optimizer.hpp"
#include "../include/inc_langdef/langdef.hpp"
#include "../stdlib/stdio.hpp"

#include "../src/src_langdef/src_lexer/lexer.cpp"
#include "../src/src_langdef/src_parser/parser.cpp"
#include "../src/src_langdef/src_optimizer/optimizer.cpp"
#include "../src/src_langdef/src_executor/executor.cpp"

namespace {

    void _move_cursor(int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H";
    }

    void _clear_next_rows(const int rows) {
        for (int i = 0; i < rows; i++) {
            std::cout << "\033[K";
            if (i < rows - 1) {
                std::cout << "\033[B\033[0G";
            }
        }
    }

    void _display_error(String& line, String& message, const int line_pos) {
        _clear_next_rows(4);
        _move_cursor(line_pos, 5);
        std::cout << "\n\n    \"" << line << "\"";
        std::cout << "\n\033[31m" << "    " << message << "\033[0m";
        _move_cursor(line_pos, 5);
        _clear_next_rows(1);
    }

}

void generate_playground() {
    String inputText;
    String line;
    VarMap stack;
    int line_size, line_number, line_pos;
    bool printed, errored;

    std::cout << "\033[2J\033[1;1H";
    std::cout << "\nRegal Playground V0.1:\n\n  i) type 'done' to quit\n  ii) type 'clear' to clear variable memory\n\n";
    std::cout << "Enter code below line by line:\n\n";

    line_number = 1;
    line_pos = 9;
    printed = false;
    errored = false;

    while (true) {
        TokenList tokens;
        Action code;

        if (printed) {
            _move_cursor(line_pos, 1);
            printed = false;
        } else {
            _move_cursor(line_pos, 1);
        }

        _clear_next_rows(1);
        std::cout << line_number << ">  ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        line_number += 1;
        line_pos += 1;
        
        line_size = line.size();

        if (line == "done") {
            if (errored) {
                _move_cursor(line_pos + 4, 1);
            } else {
                _move_cursor(line_pos + 1, 1);
            }

            break;
        }

        errored = false;

//      The print function is currently hard-coded in.
        if ((line.substr(0, 6) == "print(") && (line[line_size - 1] == ')')) {
            String var = line.substr(6, line_size - 7);
            auto it = stack.find(var);
            if (it == stack.end()) {
                String error_msg = "Variable \'" + var + "\' not initialized";
                _display_error(line, error_msg, line_pos - 1);
                line_number -= 1;
                line_pos -= 1;
            } else {
                _clear_next_rows(4);
                _move_cursor(line_pos, 5);

                std::cout << std::endl << "    ";
                print(stack[var]);
                std::cout << std::endl;
                
                line_pos += 3;
                printed = true;
            }
            
        } else if (line == "clear") {
            stack.clear();

            line_number = 1;
            line_pos = 9;

            std::cout << "\033[" << 9 << ";1H";
            std::cout << "\033[J";
        } else {
            try {
                lex_string(line, tokens);
                code = parse_line(tokens);
                optimize_action(code, stack);
                execute_action(code, stack);

                _clear_next_rows(4);
            }
            catch (const std::exception& e) {
                String error_msg = e.what();
                _display_error(line, error_msg, line_pos - 1);

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

    generate_playground();

    return 0;
}
