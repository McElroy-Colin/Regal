// File containing the Regal AST generator.

#include "parser.hpp"

// Parsing function definitions.
Action parse_line(TokenList token_list);
Pair<TokenList, Action> parse_keyword_statement(TokenList token_list);
Pair<TokenList, Action> parse_let_statement(TokenList token_list);
Pair<TokenList, Action> parse_now_statement(TokenList token_list);
Pair<TokenList, Action> parse_expression(TokenList token_list);
Pair<TokenList, Action> parse_base_expression(TokenList token_list);
Pair<TokenList, Action> parse_nothing_expression(TokenList token_list);
Pair<TokenList, Action> parse_math_expression(TokenList token_list);
Pair<TokenList, Action> parse_additive_expression(TokenList token_list);
Pair<TokenList, Action> parse_multiplicative_expression(TokenList token_list);
Pair<TokenList, Action> parse_exponential_expression(TokenList token_list);
Pair<TokenList, Action> parse_primitive_expression(TokenList token_list);
Pair<TokenList, Action> parse_number_expression(TokenList token_list);
Pair<TokenList, Action> parse_variable_expression(TokenList token_list);

// Anonymous namespce containing parsing helper functions.
namespace {

//  Check that the first element of the given list is the given token.
    bool lookahead(TokenList token_list, TokenKey target_token) {
        if (token_list.empty()) {
            return false;
        }

        Token current_token = token_list.front();

        return (std::get<TokenKey>(current_token[0]) == target_token);
    }

//  Check that the first element of the given list is any of the given tokens.
    bool lookahead_any(TokenList token_list, TokenKeyArray target_tokens) {
        if (token_list.empty()) {
            return false;
        }

        Token current_token = token_list.front();
        
        for (int target_index = 0; target_index < target_tokens.size(); target_index++) {
            if (lookahead(token_list, target_tokens[target_index])) {
                return true;
            }
        }
        return false;
    }

//  Pop the first element of the given list if it matches the given token. 
//  Return the rest of the list and the popped token.
    Pair<TokenList, Token> match_bypass(TokenList token_list, TokenKey target_token) {
        if (lookahead(token_list, target_token)) {
            Token head = token_list.front();

            token_list.pop_front();
            return std::make_pair(token_list, head);
        } else {
            perror("Parsing failed");
            exit(EXIT_FAILURE);
        }
    }

//  Pop the first element of the given list if it matches any of the given tokens. 
//  Return the rest of the list and the popped token.
    Pair<TokenList, Token> match_bypass_any(TokenList token_list, TokenKeyArray target_tokens) {
        if (lookahead_any(token_list, target_tokens)) {
            Token head = token_list.front();

            token_list.pop_front();
            return std::make_pair(token_list, head);
        } else {
            perror("Parsing failed");
            exit(EXIT_FAILURE);
        }
    }

}

// The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG.
// All functions after parse_line (below) return the remaining tokens and Actions corresponding to 
// nodes of the AST. The Action definition can be found in ./parser.hpp

// Generate an AST from a list of tokens. Return the parsed AST.
Action parse_line(TokenList token_list) {
//  Currently, all lines of code must start with a keyword.
    return parse_keyword_statement(token_list).second;    
}

// Parse a statement beginning with a keyword.
Pair<TokenList, Action> parse_keyword_statement(TokenList token_list) {
    if (lookahead(token_list, Let)) {
        return parse_let_statement(token_list);
    } else if (lookahead(token_list, Now)) {
        return parse_now_statement(token_list);
    } else {
        perror("Parsing failed.");
        exit(EXIT_FAILURE);
    }
}

// Parse a 'let' definition statement.
Pair<TokenList, Action> parse_let_statement(TokenList token_list) {
    Pair<TokenList, Token> tokens_variable;
    Pair<TokenList, Action> tokens_expression;
    Action expression;
    String variable;

    token_list = match_bypass(token_list, Let).first; // Bypass 'let'

//  Bypass and store the variable name.
    tokens_variable = match_bypass(token_list, Var);
    token_list = tokens_variable.first;
    variable = std::get<String>(tokens_variable.second[1]);

    token_list = match_bypass(token_list, Bind).first; // Bypass '='

//  Parse the expression after '='.
    tokens_expression = parse_expression(token_list);
    token_list = tokens_expression.first; // Bypass tokens in the expression.
    expression = tokens_expression.second; // Store the expression.

    return std::make_pair(token_list, std::make_shared<Assign>(variable, expression));
}

// Parse a 'now' redefinition statement.
Pair<TokenList, Action> parse_now_statement(TokenList token_list) {
    Pair<TokenList, Token> tokens_variable;
    Pair<TokenList, Action> tokens_expression;
    Action expression;
    String variable;

    token_list = match_bypass(token_list, Now).first; // Bypass 'now'

//  Bypass and store the variable name.
    tokens_variable = match_bypass(token_list, Var);
    token_list = tokens_variable.first;
    variable = std::get<String>(tokens_variable.second[1]);

    token_list = match_bypass(token_list, Bind).first; // Bypass '='

//  Parse the expression following '='.
    tokens_expression = parse_expression(token_list);
    token_list = tokens_expression.first; // Bypass tokens in the expression.
    expression = tokens_expression.second; // Store the expression.

    return std::make_pair(token_list, std::make_shared<Reassign>(variable, expression));
}

// Parse any expression as defined in the Regal CFG.
Pair<TokenList, Action> parse_expression(TokenList token_list) {
    if (lookahead_any(token_list, keyword_tokens)) {
        return parse_keyword_statement(token_list);
//  A Base Expression will start with a number, variable, or '('.
    } else if (lookahead_any(token_list, number_tokens) || 
            lookahead(token_list, Var) || lookahead(token_list, LeftPar)) {
        return parse_base_expression(token_list);
    } else if (lookahead(token_list, Nothing)) {
        return parse_nothing_expression(token_list);
    } else {
        perror("Parsing failed.");
        exit(EXIT_FAILURE);
    }
}

// Parse a Base Expression.
Pair<TokenList, Action> parse_base_expression(TokenList token_list) {
//  Currently unnecessary check, Base Expressions may have more options in future Regal updates.
    if (lookahead_any(token_list, number_tokens) || lookahead(token_list, Var) || 
            lookahead(token_list, LeftPar)) {
        return parse_math_expression(token_list);
    } else {
        perror("Parsing failed.");
        exit(EXIT_FAILURE);
    }
}

// Parse an expression of the token 'nothing'.
Pair<TokenList, Action> parse_nothing_expression(TokenList token_list) {
    token_list = match_bypass(token_list, Nothing).first;

    return std::make_pair(token_list, ON_nothing);
}

// Parse a Math Expression.
Pair<TokenList, Action> parse_math_expression(TokenList token_list) {
//  Currently unnecessary check, Math Expressions may have more options in future Regal updates.
    if (lookahead_any(token_list, number_tokens) || lookahead(token_list, Var) || 
            lookahead(token_list, LeftPar)) {
        return parse_additive_expression(token_list);
    } else {
        perror("Parsing failed.");
        exit(EXIT_FAILURE);
    }
}

// The following four functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a mathematical expression potentially containing '+' or '-'.
Pair<TokenList, Action> parse_additive_expression(TokenList token_list) {
    Pair<TokenList, Action> tokens_multexpr;
    Action multiplicative_expression;
    
//  Bypass and store the Multiplicative Expression before '+' or '-'.
    tokens_multexpr = parse_multiplicative_expression(token_list);
    token_list = tokens_multexpr.first;
    multiplicative_expression = tokens_multexpr.second;

//  Check for '+' or '-'.
    if (lookahead_any(token_list, additive_tokens)) {
        Pair<TokenList, Action> tokens_addexpr;
        Pair<TokenList, Token> tokens_operator;
        TokenKey operator_token;
        Action additive_expression;
        Operator additive_operator;

//      Bypass and store the additive operator.
        tokens_operator = match_bypass_any(token_list, additive_tokens);
        token_list = tokens_operator.first;
        operator_token = std::get<TokenKey>(tokens_operator.second[0]);

//      Bypass and store the Additive Expression after '+' or '-'.
        tokens_addexpr = parse_additive_expression(token_list);
        token_list = tokens_addexpr.first;
        additive_expression = tokens_addexpr.second;

//      Convert the operator token to an Operator value (defined in ./parser.hpp).
        switch (operator_token) {
            case Plus:
                additive_operator = Add;
                break;
            case Minus:
                additive_operator = Subtract;
                break;
            default:
                perror("Parsing failed.");
                exit(EXIT_FAILURE);
        }

        return std::make_pair(token_list, 
            std::make_shared<BinaryOperator>(additive_operator, multiplicative_expression, additive_expression));
    } else {
        return std::make_pair(token_list, multiplicative_expression);
    }
}

// Parse a mathematical expression potentially containing '*' or '/'.
Pair<TokenList, Action> parse_multiplicative_expression(TokenList token_list) {
    Pair<TokenList, Action> tokens_expexpr;
    Action exponential_expression;
    
//  Bypass and store the Exponential Expression before '*' or '/'.
    tokens_expexpr = parse_exponential_expression(token_list);
    token_list = tokens_expexpr.first;
    exponential_expression = tokens_expexpr.second;

//  Check for '*' or '/-'.
    if (lookahead_any(token_list, multiplicative_tokens)) {
        Pair<TokenList, Action> tokens_multexpr;
        Pair<TokenList, Token> tokens_operator;
        TokenKey operator_token;
        Action multiplicative_expression;
        Operator multiplicative_operator;

//      Bypass and store the multiplicative operator.
        tokens_operator = match_bypass_any(token_list, multiplicative_tokens);
        token_list = tokens_operator.first;
        operator_token = std::get<TokenKey>(tokens_operator.second[0]);

//      Bypass and store the Multiplicative Expression after '*' or '/'.
        tokens_multexpr = parse_multiplicative_expression(token_list);
        token_list = tokens_multexpr.first;
        multiplicative_expression = tokens_multexpr.second;

//      Convert the operator token to an Operator value (defined in ./parser.hpp).
        switch (operator_token) {
            case Mult:
                multiplicative_operator = Multiply;
                break;
            case Div:
                multiplicative_operator = Divide;
                break;
            default:
                perror("Parsing failed.");
                exit(EXIT_FAILURE);
        }

        return std::make_pair(token_list, 
            std::make_shared<BinaryOperator>(multiplicative_operator, exponential_expression, multiplicative_expression));
    } else {
        return std::make_pair(token_list, exponential_expression);
    }
}

// Parse a mathematical expression potentially containing '**' (exponents).
Pair<TokenList, Action> parse_exponential_expression(TokenList token_list) {
    Pair<TokenList, Action> tokens_primexpr;
    Action primitive_expression;
    
//  Bypass and store the Primitive Expression before '**'.
    tokens_primexpr = parse_primitive_expression(token_list);
    token_list = tokens_primexpr.first;
    primitive_expression = tokens_primexpr.second;

//  Check for '**'.
    if (lookahead(token_list, Exp)) {
        Pair<TokenList, Action> tokens_expexpr;
        Action exponential_expression;

//      Bypass the exponential operator.
        token_list = match_bypass(token_list, Exp).first;

//      Bypass and store the Exponential Expression after '**'.
        tokens_expexpr = parse_exponential_expression(token_list);
        token_list = tokens_expexpr.first;
        exponential_expression = tokens_expexpr.second;

        return std::make_pair(token_list, 
            std::make_shared<BinaryOperator>(Exponential, primitive_expression, exponential_expression));
    } else {
        return std::make_pair(token_list, primitive_expression);
    }
}

// Parse a Primitive Expression.
Pair<TokenList, Action> parse_primitive_expression(TokenList token_list) {
    if (lookahead_any(token_list, number_tokens)) {
        return parse_number_expression(token_list);
    } else if (lookahead(token_list, Var)) {
        return parse_variable_expression(token_list);
//  Check for and parse an expression encased in parenthesis '()'.
    } else if (lookahead(token_list, LeftPar)) {
        Pair<TokenList, Action> tokens_expression;
        Action expression;

        token_list = match_bypass(token_list, LeftPar).first; // Bypass '('

//      Bypass and store the encased expression.
        tokens_expression = parse_expression(token_list);
        token_list = tokens_expression.first;
        expression = tokens_expression.second;

        token_list = match_bypass(token_list, RightPar).first; // Bypass ')'

        return std::make_pair(token_list, expression);
    } else {
        perror("Parsing failed.");
        exit(EXIT_FAILURE);
    }
}

// Parse a Numeric Expression.
Pair<TokenList, Action> parse_number_expression(TokenList token_list) {
    Pair<TokenList, Token> tokens_number;
    TokenKey number_type;
    
//  Bypass and store the token containing the number.
    tokens_number = match_bypass_any(token_list, number_tokens);
    token_list = tokens_number.first;
    number_type = std::get<TokenKey>(tokens_number.second[0]);
    
//  Check the type of the number.
    switch (number_type) {
        case Int: // currently, only integers are supported in Regal.
            int number;
            
            number = std::get<int>(tokens_number.second[1]);
            return std::make_pair(token_list, std::make_shared<Integer>(number));
        default:
            perror("Parsing failed.");
            exit(EXIT_FAILURE);        
    }
}

// Parse a Variable Expression.
Pair<TokenList, Action> parse_variable_expression(TokenList token_list) {
    Pair<TokenList, Token> tokens_variable;
    String variable;

//  Bypass and store the variable name.
    tokens_variable = match_bypass(token_list, Var);
    token_list = tokens_variable.first;
    variable = std::get<String>(tokens_variable.second[1]);

    return std::make_pair(token_list, std::make_shared<Variable>(variable));
}