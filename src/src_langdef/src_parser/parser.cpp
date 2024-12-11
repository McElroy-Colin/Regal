// File containing the Regal AST generator.

#include "../../../include/inc_langdef/langdef.hpp"


// Parsing function definitions.

Action parse_line(std::list<Token>& token_list);
Action parse_keyword_statement(std::list<Token>& token_list);
Action parse_let_statement(std::list<Token>& token_list);
Action parse_now_statement(std::list<Token>& token_list);
Action parse_expression(std::list<Token>& token_list);
Action parse_base_expression(std::list<Token>& token_list);
Action parse_nothing_expression(std::list<Token>& token_list);
Action parse_math_expression(std::list<Token>& token_list);
Action parse_additive_expression(std::list<Token>& token_list);
Action parse_multiplicative_expression(std::list<Token>& token_list);
Action parse_exponential_expression(std::list<Token>& token_list);
Action parse_primitive_expression(std::list<Token>& token_list);
Action parse_number_expression(std::list<Token>& token_list);
Action parse_variable_expression(std::list<Token>& token_list);

// Anonymous namespace containing parsing helper functions.
namespace {

//  Throw an exception containing parsing error information.
//      expected_key: token that should have appeared (input)
//      received_key: token that appeared instead (input)
//      exp_subset: true if the expected token represents a subset (input)
    void _display_error(const TokenKey expected_key, const TokenKey received_key, const bool exp_subset = false) {
        String expected, received;

        display_token(expected_key, expected, exp_subset);
        display_token(received_key, received);

        const String error_msg = "expected \'" + expected + "\' but got \'" + received + "\'";
        throw std::runtime_error(error_msg);
    }


//  Return true if the first element of the given list is the given token.
//      token_list: linked list of remaining tokens (input)
//      target_token: token to compare the head of the list against (input)
    bool _lookahead(std::list<Token>& token_list, const TokenKey target_token) {
        if (token_list.empty()) {
            return false;
        }

        Token& current_token = token_list.front();

        return (std::get<TokenKey>(current_token[0]) == target_token);
    }

//  Return true if the first element of the given list is any of the given tokens.
//      token_list: linked list of remaining tokens (input)
//      target_tokens: tokens to compare the head of the list against (input)
    bool _lookahead_any(std::list<Token>& token_list, const std::vector<TokenKey>& target_tokens) {
        if (token_list.empty()) {
            return false;
        }
        
        for (int target_index = 0; target_index < target_tokens.size(); target_index++) {
            if (_lookahead(token_list, target_tokens[target_index])) {
                return true;
            }
        }
        return false;
    }

//  Pop the first element of the given list if it matches the given token.
//      token_list: linked list of remaining tokens (input)
//      target_token: token to match the head of the list with (input)
    void _match_bypass(std::list<Token>& token_list, const TokenKey target_token) {
        if (_lookahead(token_list, target_token)) {
            token_list.pop_front();

            return;
        } else {
            _display_error(target_token, std::get<TokenKey>(token_list.front()[0]));
        }
    }

//  Pop the first element of the given list and store it if it matches the given token.
//      token_list: linked list of remaining tokens (input)
//      target_token: token to match the head of the list with (input)
//      matched_token: token and data matched with the list (output)
    void _query_bypass(std::list<Token>& token_list, const TokenKey target_token, Token& matched_token) {
        if (_lookahead(token_list, target_token)) {
            matched_token = token_list.front();
            token_list.pop_front();

            return;
        } else {
            _display_error(target_token, std::get<TokenKey>(matched_token[0]));
        }
    }

//  Pop the first element of the given list and store it if it matches any of the given tokens.
//      token_list: linked list of remaining tokens (input)
//      target_tokens: tokens to match with the head of the list (input)
//      matched_token: token and data matched with the list (output)
    void _query_bypass_any(std::list<Token>& token_list, const std::vector<TokenKey>& target_tokens, Token& matched_token) {
        if (_lookahead_any(token_list, target_tokens)) {
            matched_token = token_list.front();
            token_list.pop_front();

            return;
        } else {
            _display_error(target_tokens[0], std::get<TokenKey>(matched_token[0]), true);
        }
    }

}

// The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG.
// All functions return Actions corresponding to nodes of the AST. 
// The Action definition can be found in ~/include/inc_parser/parser.hpp

// Generate and return an AST of Actions from a list of tokens.
//      token_list: linked list of remaining tokens (input)
Action parse_line(std::list<Token>& token_list) {
//  Currently, all lines of code must start with a keyword.
    return parse_keyword_statement(token_list);    
}

// Parse a statement beginning with a keyword.
//      token_list: linked list of remaining tokens (input)
Action parse_keyword_statement(std::list<Token>& token_list) {
    if (_lookahead(token_list, Let)) {
        return parse_let_statement(token_list);
    } else if (_lookahead(token_list, Now)) {
        return parse_now_statement(token_list);
    } else {
        throw std::runtime_error("parsing failed");
    }
}

// Parse a 'let' definition statement.
//      token_list: linked list of remaining tokens (input)
Action parse_let_statement(std::list<Token>& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    _match_bypass(token_list, Let); // Bypass 'let'

//  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    _match_bypass(token_list, Bind); // Bypass '='

//  Parse the expression after '='.
    expression = parse_expression(token_list);

    return std::make_shared<Assign>(variable, expression);
}

// Parse a 'now' redefinition statement.
//      token_list: linked list of remaining tokens (input)
Action parse_now_statement(std::list<Token>& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    _match_bypass(token_list, Now); // Bypass 'now'

//  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    _match_bypass(token_list, Bind); // Bypass '='

//  Parse the expression following '='.
    expression = parse_expression(token_list);

    return std::make_shared<Reassign>(variable, expression);
}

// Parse any expression as defined in the Regal CFG.
//      token_list: linked list of remaining tokens (input)
Action parse_expression(std::list<Token>& token_list) {
    if (_lookahead_any(token_list, keyword_tokens)) {
        return parse_keyword_statement(token_list);
//  A Base Expression will start with a number, variable, or '('.
    } else if (_lookahead_any(token_list, number_tokens) || 
            _lookahead(token_list, Var) || _lookahead(token_list, LeftPar)) {
        return parse_base_expression(token_list);
    } else if (_lookahead(token_list, Nothing)) {
        return parse_nothing_expression(token_list);
    } else {
        throw std::runtime_error("parsing failed");
    }
}

// Parse a Base Expression.
//      token_list: linked list of remaining tokens (input)
Action parse_base_expression(std::list<Token>& token_list) {
//  Currently unnecessary check, Base Expressions may have more options in future Regal updates.
    if (_lookahead_any(token_list, number_tokens) || _lookahead(token_list, Var) || 
            _lookahead(token_list, LeftPar)) {
        return parse_math_expression(token_list);
    } else {
        throw std::runtime_error("parsing failed");
    }
}

// Parse an expression of the token 'nothing'.
//      token_list: linked list of remaining tokens (input)
Action parse_nothing_expression(std::list<Token>& token_list) {
    _match_bypass(token_list, Nothing);

    return ON_Nothing;
}

// Parse a Math Expression.
//      token_list: linked list of remaining tokens (input)
Action parse_math_expression(std::list<Token>& token_list) {
//  Currently unnecessary check, Math Expressions may have more options in future Regal updates.
    if (_lookahead_any(token_list, number_tokens) || _lookahead(token_list, Var) || 
            _lookahead(token_list, LeftPar)) {
        return parse_additive_expression(token_list);
    } else {
        throw std::runtime_error("parsing failed");
    }
}

// The following four functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a mathematical expression potentially containing '+' or '-'.
//      token_list: linked list of remaining tokens (input)
Action parse_additive_expression(std::list<Token>& token_list) {
    Action multiplicative_expression;
    
//  Bypass and store the Multiplicative Expression before '+' or '-'.
    multiplicative_expression = parse_multiplicative_expression(token_list);

//  Check for '+' or '-'.
    if (_lookahead_any(token_list, additive_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action additive_expression;

//      Bypass and store the additive operator.
        _query_bypass_any(token_list, additive_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Bypass and store the Additive Expression after '+' or '-'.
        additive_expression = parse_additive_expression(token_list);

        return std::make_shared<BinaryOperator>(operator_key, multiplicative_expression, additive_expression);
        
        throw std::runtime_error("parsing failed");
    }
    
    return multiplicative_expression;
}

// Parse a mathematical expression potentially containing '*' or '/'.
//      token_list: linked list of remaining tokens (input)
Action parse_multiplicative_expression(std::list<Token>& token_list) {
    Action exponential_expression;
    
//  Bypass and store the Exponential Expression before '*' or '/'.
    exponential_expression = parse_exponential_expression(token_list);

//  Check for '*' or '/-'.
    if (_lookahead_any(token_list, multiplicative_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action multiplicative_expression;

//      Bypass and store the multiplicative operator.
        _query_bypass_any(token_list, multiplicative_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Bypass and store the Multiplicative Expression after '*' or '/'.
        multiplicative_expression = parse_multiplicative_expression(token_list);

        return std::make_shared<BinaryOperator>(operator_key, exponential_expression, multiplicative_expression);

        throw std::runtime_error("parsing failed");
    }
    
    return exponential_expression;
}

// Parse a mathematical expression potentially containing '**' (exponents).
//      token_list: linked list of remaining tokens (input)
Action parse_exponential_expression(std::list<Token>& token_list) {
    Action primitive_expression;
    
//  Bypass and store the Primitive Expression before '**'.
    primitive_expression = parse_primitive_expression(token_list);

//  Check for '**'.
    if (_lookahead(token_list, Exp)) {
        Action exponential_expression;

//      Bypass the exponential operator.
        _match_bypass(token_list, Exp);

//      Bypass and store the Exponential Expression after '**'.
        exponential_expression = parse_exponential_expression(token_list);

        return std::make_shared<BinaryOperator>(Exp, primitive_expression, exponential_expression);
    }

    return primitive_expression;
}

// Parse a Primitive Expression.
//      token_list: linked list of remaining tokens (input)
Action parse_primitive_expression(std::list<Token>& token_list) {
    if (_lookahead_any(token_list, number_tokens)) {
        return parse_number_expression(token_list);
    } else if (_lookahead(token_list, Var)) {
        return parse_variable_expression(token_list);
//  Check for and parse an expression encased in parenthesis '()'.
    } else if (_lookahead(token_list, LeftPar)) {
        Action expression;

        _match_bypass(token_list, LeftPar); // Bypass '('

//      Bypass and store the encased expression.
        expression = parse_expression(token_list);

        _match_bypass(token_list, RightPar); // Bypass ')'

        return expression;
    } else {
        throw std::runtime_error("parsing failed");
    }
}

// Parse a Numeric Expression.
//      token_list: linked list of remaining tokens (input)
Action parse_number_expression(std::list<Token>& token_list) {
    Token number_token;
    TokenKey number_type;
    
//  Bypass and identify the number type of the number token.
    _query_bypass_any(token_list, number_tokens, number_token);
    number_type = std::get<TokenKey>(number_token[0]);
    
//  Check the type of the number.
    switch (number_type) {
        case Int: // currently, only integers are supported in Regal.
            int number;
            
            number = std::get<int>(number_token[1]);
            return std::make_shared<Integer>(number);
        default:
            throw std::runtime_error("parsing failed");        
    }
}

// Parse a Variable Expression.
//      token_list: linked list of remaining tokens (input)
Action parse_variable_expression(std::list<Token>& token_list) {
    Token variable_token;
    String variable;

//  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    return std::make_shared<Variable>(variable);
}
