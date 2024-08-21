#include "../langdef.hpp"
#include "parser.hpp"
#include "../Lexer/lexer.cpp"
#include <iostream>
#include <chrono>


bool lookahead(TokenList& token_list, TokenKey target_token) {
    if (token_list.empty()) {
        return false;
    }

    Token& current_token = token_list.front();

    return (std::get<TokenKey>(current_token[0]) == target_token);
}


Token& match_bypass_quick(TokenList& token_list, TokenKey target_token) {
    if (lookahead(token_list, target_token)) {
        Token& head = token_list.front();

        token_list.pop_front();

        return head;
    } else {
        perror("Parsing failed");
        exit(EXIT_FAILURE);
    }
}

Token match_bypass_slow(TokenList& token_list, TokenKey target_token) {
    if (lookahead(token_list, target_token)) {
        Token head = token_list.front();

        token_list.pop_front();

        return head;
    } else {
        perror("Parsing failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    TokenList tokens = lex_string("let x = 4");



    Token fast = match_bypass_quick(tokens, Let);



    TokenList tokens = lex_string("let x = 4");



    Token slow = match_bypass_slow(tokens, Let);

    

}