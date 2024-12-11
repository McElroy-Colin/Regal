// File containing the Regal AST generator.

#include "../../../include/inc_langdef/langdef.hpp"

// Parsing function definitions.
Action parse_line(TokenList& token_list);
Action parse_keyword_statement(TokenList& token_list);
Action parse_let_statement(TokenList& token_list);
Action parse_now_statement(TokenList& token_list);
Action parse_expression(TokenList& token_list);
Action parse_base_expression(TokenList& token_list);
Action parse_nothing_expression(TokenList& token_list);
Action parse_math_expression(TokenList& token_list);
Action parse_additive_expression(TokenList& token_list);
Action parse_multiplicative_expression(TokenList& token_list);
Action parse_exponential_expression(TokenList& token_list);
Action parse_primitive_expression(TokenList& token_list);
Action parse_number_expression(TokenList& token_list);
Action parse_variable_expression(TokenList& token_list);

// Anonymous namespace containing parsing helper functions.
namespace {

//  Return true if the first element of the given list is the given token.
    bool lookahead(TokenList& token_list, TokenKey target_token) {
        if (token_list.empty()) {
            return false;
        }

        Token& current_token = token_list.front();

        return (std::get<TokenKey>(current_token[0]) == target_token);
    }

//  Return true if the first element of the given list is any of the given tokens.
    bool lookahead_any(TokenList& token_list, TokenKeyArray& target_tokens) {
        if (token_list.empty()) {
            return false;
        }
        
        for (int target_index = 0; target_index < target_tokens.size(); target_index++) {
            if (lookahead(token_list, target_tokens[target_index])) {
                return true;
            }
        }
        return false;
    }

//  Pop the first element of the given list if it matches the given token.
    void match_bypass(TokenList& token_list, TokenKey target_token) {
        if (lookahead(token_list, target_token)) {
            token_list.pop_front();

            return;
        } else {
            throw std::runtime_error("Parsing failed");
        }
    }

//  Pop the first element of the given list and store it if it matches the given token
    void query_bypass(TokenList& token_list, TokenKey target_token, Token& return_val) {
        if (lookahead(token_list, target_token)) {
            return_val = token_list.front();
            token_list.pop_front();

            return;
        } else {
            throw std::runtime_error("Parsing failed");
        }
    }

//  Pop the first element of the given list and store it if it matches any of the given tokens.
    void query_bypass_any(TokenList& token_list, TokenKeyArray& target_tokens, Token& return_val) {
        if (lookahead_any(token_list, target_tokens)) {
            return_val = token_list.front();
            token_list.pop_front();

            return;
        } else {
            throw std::runtime_error("Parsing failed");
        }
    }

}

// The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG.
// All functions return Actions corresponding to nodes of the AST. 
// The Action definition can be found in ~/include/inc_parser/parser.hpp

// Generate and return an AST of Actions from a list of tokens.
Action parse_line(TokenList& token_list) {
//  Currently, all lines of code must start with a keyword.
    return parse_keyword_statement(token_list);    
}

// Parse a statement beginning with a keyword.
Action parse_keyword_statement(TokenList& token_list) {
    if (lookahead(token_list, Let)) {
        return parse_let_statement(token_list);
    } else if (lookahead(token_list, Now)) {
        return parse_now_statement(token_list);
    } else {
        throw std::runtime_error("Parsing failed");
    }
}

// Parse a 'let' definition statement.
Action parse_let_statement(TokenList& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    match_bypass(token_list, Let); // Bypass 'let'

//  Bypass and store the variable name.
    query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    match_bypass(token_list, Bind); // Bypass '='

//  Parse the expression after '='.
    expression = parse_expression(token_list);

    return std::make_shared<Assign>(variable, expression);
}

// Parse a 'now' redefinition statement.
Action parse_now_statement(TokenList& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    match_bypass(token_list, Now); // Bypass 'now'

//  Bypass and store the variable name.
    query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    match_bypass(token_list, Bind); // Bypass '='

//  Parse the expression following '='.
    expression = parse_expression(token_list);

    return std::make_shared<Reassign>(variable, expression);
}

// Parse any expression as defined in the Regal CFG.
Action parse_expression(TokenList& token_list) {
    if (lookahead_any(token_list, keyword_tokens)) {
        return parse_keyword_statement(token_list);
//  A Base Expression will start with a number, variable, or '('.
    } else if (lookahead_any(token_list, number_tokens) || 
            lookahead(token_list, Var) || lookahead(token_list, LeftPar)) {
        return parse_base_expression(token_list);
    } else if (lookahead(token_list, Nothing)) {
        return parse_nothing_expression(token_list);
    } else {
        throw std::runtime_error("Parsing failed");
    }
}

// Parse a Base Expression.
Action parse_base_expression(TokenList& token_list) {
//  Currently unnecessary check, Base Expressions may have more options in future Regal updates.
    if (lookahead_any(token_list, number_tokens) || lookahead(token_list, Var) || 
            lookahead(token_list, LeftPar)) {
        return parse_math_expression(token_list);
    } else {
        throw std::runtime_error("Parsing failed");
    }
}

// Parse an expression of the token 'nothing'.
Action parse_nothing_expression(TokenList& token_list) {
    match_bypass(token_list, Nothing);

    return ON_Nothing;
}

// Parse a Math Expression.
Action parse_math_expression(TokenList& token_list) {
//  Currently unnecessary check, Math Expressions may have more options in future Regal updates.
    if (lookahead_any(token_list, number_tokens) || lookahead(token_list, Var) || 
            lookahead(token_list, LeftPar)) {
        return parse_additive_expression(token_list);
    } else {
        throw std::runtime_error("Parsing failed");
    }
}

// The following four functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a mathematical expression potentially containing '+' or '-'.
Action parse_additive_expression(TokenList& token_list) {
    Action multiplicative_expression;
    
//  Bypass and store the Multiplicative Expression before '+' or '-'.
    multiplicative_expression = parse_multiplicative_expression(token_list);

//  Check for '+' or '-'.
    if (lookahead_any(token_list, additive_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action additive_expression;

//      Bypass and store the additive operator.
        query_bypass_any(token_list, additive_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Bypass and store the Additive Expression after '+' or '-'.
        additive_expression = parse_additive_expression(token_list);

        return std::make_shared<BinaryOperator>(operator_key, multiplicative_expression, additive_expression);
        
        throw std::runtime_error("Parsing failed");
    }
    
    return multiplicative_expression;
}

// Parse a mathematical expression potentially containing '*' or '/'.
Action parse_multiplicative_expression(TokenList& token_list) {
    Action exponential_expression;
    
//  Bypass and store the Exponential Expression before '*' or '/'.
    exponential_expression = parse_exponential_expression(token_list);

//  Check for '*' or '/-'.
    if (lookahead_any(token_list, multiplicative_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action multiplicative_expression;

//      Bypass and store the multiplicative operator.
        query_bypass_any(token_list, multiplicative_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Bypass and store the Multiplicative Expression after '*' or '/'.
        multiplicative_expression = parse_multiplicative_expression(token_list);

        return std::make_shared<BinaryOperator>(operator_key, exponential_expression, multiplicative_expression);

        throw std::runtime_error("Parsing failed");
    }
    
    return exponential_expression;
}

// Parse a mathematical expression potentially containing '**' (exponents).
Action parse_exponential_expression(TokenList& token_list) {
    Action primitive_expression;
    
//  Bypass and store the Primitive Expression before '**'.
    primitive_expression = parse_primitive_expression(token_list);

//  Check for '**'.
    if (lookahead(token_list, Exp)) {
        Action exponential_expression;

//      Bypass the exponential operator.
        match_bypass(token_list, Exp);

//      Bypass and store the Exponential Expression after '**'.
        exponential_expression = parse_exponential_expression(token_list);

        return std::make_shared<BinaryOperator>(Exp, primitive_expression, exponential_expression);
    }

    return primitive_expression;
}

// Parse a Primitive Expression.
Action parse_primitive_expression(TokenList& token_list) {
    if (lookahead_any(token_list, number_tokens)) {
        return parse_number_expression(token_list);
    } else if (lookahead(token_list, Var)) {
        return parse_variable_expression(token_list);
//  Check for and parse an expression encased in parenthesis '()'.
    } else if (lookahead(token_list, LeftPar)) {
        Action expression;

        match_bypass(token_list, LeftPar); // Bypass '('

//      Bypass and store the encased expression.
        expression = parse_expression(token_list);

        match_bypass(token_list, RightPar); // Bypass ')'

        return expression;
    } else {
        throw std::runtime_error("Parsing failed");
    }
}

// Parse a Numeric Expression.
Action parse_number_expression(TokenList& token_list) {
    Token number_token;
    TokenKey number_type;
    
//  Bypass and identify the number type of the number token.
    query_bypass_any(token_list, number_tokens, number_token);
    number_type = std::get<TokenKey>(number_token[0]);
    
//  Check the type of the number.
    switch (number_type) {
        case Int: // currently, only integers are supported in Regal.
            int number;
            
            number = std::get<int>(number_token[1]);
            return std::make_shared<Integer>(number);
        default:
            throw std::runtime_error("Parsing failed");        
    }
}

// Parse a Variable Expression.
Action parse_variable_expression(TokenList& token_list) {
    Token variable_token;
    String variable;

//  Bypass and store the variable name.
    query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    return std::make_shared<Variable>(variable);
}
