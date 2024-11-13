#include <iostream>
#include "../../src_langdef/src_parser/parser.cpp"
#include "../../src_langdef/src_lexer/lexer.cpp"
#include "parser_copy.hpp"

using String = std::string;

int main() {
    String line = "let x = 2*(4 - 3)";
    TokenList lexed = lex_string(line);

    Action parsed = parse_line(lexed);

    return 0;
}