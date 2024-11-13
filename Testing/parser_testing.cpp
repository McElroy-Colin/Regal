#include "../LangDef/Parser/parser.cpp"
#include "../LangDef/Lexer/lexer.cpp"
#include <iostream>

int main() {
    String line = "let x = 2*(4 - 3)";
    TokenList lexed = lex_string(line);

    Action parsed = parse_line(lexed);

    return 0;
}