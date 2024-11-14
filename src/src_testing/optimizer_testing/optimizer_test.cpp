#include <iostream>
#include <chrono>
#include "../../src_langdef/src_lexer/lexer.cpp"
#include "../../src_langdef/src_parser/parser.cpp"
#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"
#include "../../src_langdef/src_optimizer/optimizer.cpp"
#include "optimizer_copy.cpp"
#include "../parser_testing/parser_copy.hpp"
#include "../parser_testing/parser_copy.cpp"



int main() {
    String line = "let x = 4 * 2";
    VarMap stack;

    TokenList tokens = lex_string(line);
    Action parsed = parse_line(tokens);

    TokenList tokens_new = lex_string(line);
    Action_c parsed_new = parse_line_c(tokens_new);

    TokenList lexed2 = lex_string(line);

    auto new_start = std::chrono::high_resolution_clock::now();
    optimize_action_c(parsed_new, stack);
    auto new_end = std::chrono::high_resolution_clock::now();

    auto new_length = std::chrono::duration_cast<std::chrono::nanoseconds>(new_end - new_start).count();


    auto orig_start = std::chrono::high_resolution_clock::now();
    optimize_action(parsed, stack);
    auto orig_end = std::chrono::high_resolution_clock::now();

    auto orig_length = std::chrono::duration_cast<std::chrono::nanoseconds>(orig_end - orig_start).count();


    std::cout << "\nOriginal: " << orig_length << " ns\n" << std::endl;
    std::cout << "\nNew: " << new_length << " ns\n" << std::endl;

    return 0;
}