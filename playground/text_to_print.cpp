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


// Creates simple text environment to assign and print variables.
int main() {
    String inputText;
    String line;
    VarMap stack;

    std::cout << "\nRegal Playground V0.1:\n\n  i) type 'done' to quit\n  ii) type 'clear' to clear variable memory):\n\n";
    std::cout << "Enter code below line by line:\n\n";

    while (std::getline(std::cin, line)) {
        TokenList tokens;
        Action code;

        if (line == "done") {
            break;
//      The print function is currently hard-coded in.
        } else if ((line.substr(0, 6) == "print(") && (line[line.size() - 1] == ')')) {
            String var = line.substr(6, line.size() - 7);

            auto it = stack.find(var);
            if (it == stack.end()) {
                std::cout << var << " not initialized." << std::endl;
            } else {
                print(stack[var]);
            }
            
        } else if (line == "clear") {
            stack.clear();
        } else {
            try {
                lex_string(line, tokens);
                code = parse_line(tokens);
                optimize_action(code, stack);
                execute_action(code, stack);
            }
            catch (const std::exception& e) {
                std::cout << std::endl;
            }
        }
    }

    return 0;
}
