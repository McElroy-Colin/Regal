#include "../langdef.hpp"

// temp
#include <iostream>

namespace {

    bool lookahead(TokenList token_list, TokenKey target_token) {
        Token current_token = token_list.front();

        return (extract<TokenKey>(current_token[0]) == target_token);
    }

    bool lookahead_any(TokenList token_list, TokenKeyArray target_tokens) {
        Token current_token = token_list.front();
        
        for (int target_index = 0; target_index < target_tokens.size(); target_index++) {
            if (lookahead(token_list, target_tokens[target_index])) {
                return true;
            }
        }
        return false;
    }

    Tuple<TokenList, Token> match_bypass(TokenList token_list, TokenKey target_token) {
        if (lookahead(token_list, target_token)) {
            Token head = token_list.front();

            token_list.pop_front();
            return std::make_tuple(token_list, head);
        } else {
            perror("Parsing failed");
            exit(EXIT_FAILURE);
        }
    }

    Tuple<TokenList, Token> match_bypass_any(TokenList token_list, TokenKeyArray target_tokens) {
        if (lookahead_any(token_list, target_tokens)) {
            Token head = token_list.front();

            token_list.pop_front();
            return std::make_tuple(token_list, head);
        } else {
            perror("Parsing failed");
            exit(EXIT_FAILURE);
        }
    }

}

ActionList parse_line(TokenList token_list) {
    
}