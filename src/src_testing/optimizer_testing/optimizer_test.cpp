#include <iostream>
#include "../../src_langdef/src_lexer/lexer.cpp"
#include "../../src_langdef/src_parser/parser.cpp"
#include "../../../include/inc_langdef/inc_optimizer/optimizer.hpp"
#include "../../src_langdef/src_optimizer/optimizer.cpp"



int main() {
    String line = "let x = 4 * (2 + 3 * 7)";
    TokenList tokens = lex_string(line);
    Action parsed = parse_line(tokens);

    VarMap stack;
    bool optimized = optimize_action(parsed, stack);

    return 0;
}