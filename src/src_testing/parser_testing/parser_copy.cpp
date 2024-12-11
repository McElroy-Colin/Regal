// File containing the Regal AST generator.

#include "parser_copy.hpp"

// Parsing function definitions.
Action_c parse_line_c(TokenList& token_list);
Action_c parse_keyword_statement_c(TokenList& token_list);
Action_c parse_let_statement_c(TokenList& token_list);
Action_c parse_now_statement_c(TokenList& token_list);
Action_c parse_expression_c(TokenList& token_list);
Action_c parse_base_expression_c(TokenList& token_list);
Action_c parse_nothing_expression_c(TokenList& token_list);
Action_c parse_math_expression_c(TokenList& token_list);
Action_c parse_additive_expression_c(TokenList& token_list);
Action_c parse_multiplicative_expression_c(TokenList& token_list);
Action_c parse_exponential_expression_c(TokenList& token_list);
Action_c parse_primitive_expression_c(TokenList& token_list);
Action_c parse_number_expression_c(TokenList& token_list);
Action_c parse_variable_expression_c(TokenList& token_list);

// Anonymous namespce containing parsing helper functions.
namespace {

//  Return true if the first element of the given list is the given token.
    bool lookahead_c(TokenList& token_list, TokenKey target_token) {
        if (token_list.empty()) {
            return false;
        }

        Token& current_token = token_list.front();

        return (std::get<TokenKey>(current_token[0]) == target_token);
    }

//  Return true if the first element of the given list is any of the given tokens.
    bool lookahead_any_c(TokenList& token_list, TokenKeyArray& target_tokens) {
        if (token_list.empty()) {
            return false;
        }
        
        for (int target_index = 0; target_index < target_tokens.size(); target_index++) {
            if (lookahead_c(token_list, target_tokens[target_index])) {
                return true;
            }
        }
        return false;
    }

//  Pop the first element of the given list if it matches the given token.
    void match_bypass_c(TokenList& token_list, TokenKey target_token) {
        if (lookahead_c(token_list, target_token)) {
            token_list.pop_front();

            return;
        } else {
            std::cerr << "Parsing failed" << std::endl;
            throw std::runtime_error();;
        }
    }

//  Pop the first element of the given list and store it if it matches the given token
    void query_bypass_c(TokenList& token_list, TokenKey target_token, Token& return_val) {
        if (lookahead_c(token_list, target_token)) {
            return_val = token_list.front();
            token_list.pop_front();

            return;
        } else {
            std::cerr << "Parsing failed" << std::endl;
            throw std::runtime_error();;
        }
    }

//  Pop the first element of the given list and store it if it matches any of the given tokens.
    void query_bypass_any_c(TokenList& token_list, TokenKeyArray& target_tokens, Token& return_val) {
        if (lookahead_any_c(token_list, target_tokens)) {
            return_val = token_list.front();
            token_list.pop_front();
            
            return;
        } else {
            std::cerr << "Parsing failed" << std::endl;
            throw std::runtime_error();;
        }
    }

}

// The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG.
// All functions return Actions corresponding to nodes of the AST. 
// The Action definition can be found in ./parser.hpp

// Generate and return an AST of Actions from a list of tokens.
Action_c parse_line_c(TokenList& token_list) {
//  Currently, all lines of code must start with a keyword.
    return parse_keyword_statement_c(token_list);    
}

// Parse a statement beginning with a keyword.
Action_c parse_keyword_statement_c(TokenList& token_list) {
    if (lookahead_c(token_list, Let)) {
        return parse_let_statement_c(token_list);
    } else if (lookahead_c(token_list, Now)) {
        return parse_now_statement_c(token_list);
    } else {
        perror("Parsing failed");
        throw std::runtime_error();;
    }
}

// Parse a 'let' definition statement.
Action_c parse_let_statement_c(TokenList& token_list) {
    Action_c expression;
    Token variable_token;
    String variable;


    match_bypass_c(token_list, Let); // Bypass 'let'

//  Bypass and store the variable name.
    query_bypass_c(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    match_bypass_c(token_list, Bind); // Bypass '='

//  Parse the expression after '='.
    expression = parse_expression_c(token_list);

    return std::make_shared<Assign_c>(variable, expression);
}

// Parse a 'now' redefinition statement.
Action_c parse_now_statement_c(TokenList& token_list) {
    Action_c expression;
    Token variable_token;
    String variable;

    match_bypass_c(token_list, Now); // Bypass 'now'

//  Bypass and store the variable name.
    query_bypass_c(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    match_bypass_c(token_list, Bind); // Bypass '='

//  Parse the expression following '='.
    expression = parse_expression_c(token_list);

    return std::make_shared<Reassign_c>(variable, expression);
}

// Parse any expression as defined in the Regal CFG.
Action_c parse_expression_c(TokenList& token_list) {
    if (lookahead_any_c(token_list, keyword_tokens)) {
        return parse_keyword_statement_c(token_list);
//  A Base Expression will start with a number, variable, or '('.
    } else if (lookahead_any_c(token_list, number_tokens) || 
            lookahead_c(token_list, Var) || lookahead_c(token_list, LeftPar)) {
        return parse_base_expression_c(token_list);
    } else if (lookahead_c(token_list, Nothing)) {
        return parse_nothing_expression_c(token_list);
    } else {
        perror("Parsing failed");
        throw std::runtime_error();;
    }
}

// Parse a Base Expression.
Action_c parse_base_expression_c(TokenList& token_list) {
//  Currently unnecessary check, Base Expressions may have more options in future Regal updates.
    if (lookahead_any_c(token_list, number_tokens) || lookahead_c(token_list, Var) || 
            lookahead_c(token_list, LeftPar)) {
        return parse_math_expression_c(token_list);
    } else {
        perror("Parsing failed");
        throw std::runtime_error();;
    }
}

// Parse an expression of the token 'nothing'.
Action_c parse_nothing_expression_c(TokenList& token_list) {
    match_bypass_c(token_list, Nothing);

    return ON_nothing_c;
}

// Parse a Math Expression.
Action_c parse_math_expression_c(TokenList& token_list) {
//  Currently unnecessary check, Math Expressions may have more options in future Regal updates.
    if (lookahead_any_c(token_list, number_tokens) || lookahead_c(token_list, Var) || 
            lookahead_c(token_list, LeftPar)) {
        return parse_additive_expression_c(token_list);
    } else {
        perror("Parsing failed");
        throw std::runtime_error();;
    }
}

// The following four functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a mathematical expression potentially containing '+' or '-'.
Action_c parse_additive_expression_c(TokenList& token_list) {
    Action_c multiplicative_expression;
    
//  Bypass and store the Multiplicative Expression before '+' or '-'.
    multiplicative_expression = parse_multiplicative_expression_c(token_list);

//  Check for '+' or '-'.
    if (lookahead_any_c(token_list, additive_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action_c additive_expression;

//      Bypass and store the additive operator.
        query_bypass_any_c(token_list, additive_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Bypass and store the Additive Expression after '+' or '-'.
        additive_expression = parse_additive_expression_c(token_list);

//      Ensure that the operator is additive.
        if ((operator_key == Plus) || (operator_key == Minus)) {
            return std::make_shared<BinaryOperator_c>(operator_key, multiplicative_expression, additive_expression);
        }

        perror("Parsing failed");
        throw std::runtime_error();;
    }
    
    return multiplicative_expression;
}

// Parse a mathematical expression potentially containing '*' or '/'.
Action_c parse_multiplicative_expression_c(TokenList& token_list) {
    Action_c exponential_expression;
    
//  Bypass and store the Exponential Expression before '*' or '/'.
    exponential_expression = parse_exponential_expression_c(token_list);

//  Check for '*' or '/-'.
    if (lookahead_any_c(token_list, multiplicative_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action_c multiplicative_expression;

//      Bypass and store the multiplicative operator.
        query_bypass_any_c(token_list, multiplicative_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Bypass and store the Multiplicative Expression after '*' or '/'.
        multiplicative_expression = parse_multiplicative_expression_c(token_list);

//      Ensure that the operator is multiplicative.
        if ((operator_key == Mult) || (operator_key == Div)) {
            return std::make_shared<BinaryOperator_c>(operator_key, exponential_expression, multiplicative_expression);
        }
        
        perror("Parsing failed");
        throw std::runtime_error();;
    }
    
    return exponential_expression;
}

// Parse a mathematical expression potentially containing '**' (exponents).
Action_c parse_exponential_expression_c(TokenList& token_list) {
    Action_c primitive_expression;
    
//  Bypass and store the Primitive Expression before '**'.
    primitive_expression = parse_primitive_expression_c(token_list);

//  Check for '**'.
    if (lookahead_c(token_list, Exp)) {
        Action_c exponential_expression;

//      Bypass the exponential operator.
        match_bypass_c(token_list, Exp);

//      Bypass and store the Exponential Expression after '**'.
        exponential_expression = parse_exponential_expression_c(token_list);

        return std::make_shared<BinaryOperator_c>(Exp, primitive_expression, exponential_expression);
    }

    return primitive_expression;
}

// Parse a Primitive Expression.
Action_c parse_primitive_expression_c(TokenList& token_list) {
    if (lookahead_any_c(token_list, number_tokens)) {
        return parse_number_expression_c(token_list);
    } else if (lookahead_c(token_list, Var)) {
        return parse_variable_expression_c(token_list);
//  Check for and parse an expression encased in parenthesis '()'.
    } else if (lookahead_c(token_list, LeftPar)) {
        Action_c expression;

        match_bypass_c(token_list, LeftPar); // Bypass '('

//      Bypass and store the encased expression.
        expression = parse_expression_c(token_list);

        match_bypass_c(token_list, RightPar); // Bypass ')'

        return expression;
    } else {
        perror("Parsing failed");
        throw std::runtime_error();;
    }
}

// Parse a Numeric Expression.
Action_c parse_number_expression_c(TokenList& token_list) {
    Token number_token;
    TokenKey number_type;
    
//  Bypass and identify the number type of the number token.
    query_bypass_any_c(token_list, number_tokens, number_token);
    number_type = std::get<TokenKey>(number_token[0]);
    
//  Check the type of the number.
    switch (number_type) {
        case Int: // currently, only integers are supported in Regal.
            int number;
            
            number = std::get<int>(number_token[1]);
            return std::make_shared<Integer_c>(number);
        default:
            perror("Parsing failed");
            throw std::runtime_error();;        
    }
}

// Parse a Variable Expression.
Action_c parse_variable_expression_c(TokenList& token_list) {
    Token variable_token;
    String variable;

//  Bypass and store the variable name.
    query_bypass_c(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    return std::make_shared<Variable_c>(variable);
}