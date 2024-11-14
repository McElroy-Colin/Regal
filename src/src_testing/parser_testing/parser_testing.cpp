#include <iostream>
#include <chrono>
#include "../../../include/inc_langdef/inc_parser/parser.hpp"
#include "../../src_langdef/src_parser/parser.cpp"
#include "../../src_langdef/src_lexer/lexer.cpp"
#include "parser_copy.cpp"

int main() {
    String line = "let x = 2*(4 - 3) ** (9 / 4 * 3 + 2 * 5)";

    TokenList lexed = lex_string(line);

    auto orig_start = std::chrono::high_resolution_clock::now();
    parse_line(lexed);
    auto orig_end = std::chrono::high_resolution_clock::now();

    auto orig_length = std::chrono::duration_cast<std::chrono::nanoseconds>(orig_end - orig_start).count();

    TokenList lexed2 = lex_string(line);

    auto new_start = std::chrono::high_resolution_clock::now();
    parse_line_c(lexed2);
    auto new_end = std::chrono::high_resolution_clock::now();

    auto new_length = std::chrono::duration_cast<std::chrono::nanoseconds>(new_end - new_start).count();


    std::cout << "\nOriginal: " << orig_length << " ns\n" << std::endl;
    std::cout << "New: " << new_length << " ns\n" << std::endl;

    return 0;
}