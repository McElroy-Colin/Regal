// File containing the Regal AST generator.

#include "../../../include/inc_langdef/langdef.hpp"


// Parsing function definitions.

Action parse_line(std::list<Token>& token_list);
Action parse_keyword_statement(std::list<Token>& token_list);
Action parse_assignment_statement(std::list<Token>& token_list);
Action parse_let_statement(std::list<Token>& token_list);
Action parse_now_statement(std::list<Token>& token_list);
Action parse_expression(std::list<Token>& token_list);
Action parse_or_expression(std::list<Token>& token_list);
Action parse_and_expression(std::list<Token>& token_list);
Action parse_not_expression(std::list<Token>& token_list);
Action parse_comparison_expression(std::list<Token>& token_list);
Action parse_additive_expression(std::list<Token>& token_list);
Action parse_multiplicative_expression(std::list<Token>& token_list);
Action parse_exponential_expression(std::list<Token>& token_list);
Action parse_minus_identifier_expression(std::list<Token>& token_list);
Action parse_primitive_expression(std::list<Token>& token_list);
Action parse_number_expression(std::list<Token>& token_list);
Action parse_boolean_expression(std::list<Token>& token_list);


// Anonymous namespace containing parsing helper functions.
namespace {

//  Throw an exception containing parsing error information.
//      expected_key: token that should have appeared (input)
//      received_key: token that appeared instead (input)
//      exp_subset: true if the expected token represents a subset (input)
//      end_of_line: true if there is no received token (input)
    void _display_error(const TokenKey expected_key, const TokenKey received_key, const bool exp_subset = false, const bool end_of_line = false) {
        String expected, received;

        display_token(expected_key, expected, exp_subset);

        const String error_msg = end_of_line 
            ? "expected \'" + expected + "\' but line ended"
            : "expected \'" + expected + "\' but got \'" + (display_token(received_key, received), received) + "\'";

        throw std::runtime_error(error_msg);
    }


//  Return true if the first element of the given list is the given token.
//      token_list: linked list of remaining tokens (input)
//      target_token: token to compare the head of the list against (input)
    bool _lookahead(std::list<Token>& token_list, const TokenKey target_token) {
        if (token_list.empty()) {
            return false;
        }

        return (std::get<TokenKey>(token_list.front()[0]) == target_token);
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
        }
        
        if (token_list.empty()) {
            _display_error(target_token, Nothing, false, true);
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
        }

        if (token_list.empty()) {
            _display_error(target_token, Nothing, false, true);
        } else {
            _display_error(target_token, std::get<TokenKey>(token_list.front()[0]));
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
        }

        if (token_list.empty()) {
            _display_error(target_tokens[0], Nothing, true, true);
        } else {
            _display_error(target_tokens[0], std::get<TokenKey>(token_list.front()[0]), true, false);
        }
    }

}

// The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG.
// All functions return Actions corresponding to nodes of the AST. 
// The Action definition can be found in ~/include/inc_parser/parser.hpp

// Generate and return an AST of Actions from a list of tokens.
//      token_list: linked list of remaining tokens (input)
Action parse_line(std::list<Token>& token_list) {
    Action line;

    if (_lookahead_any(token_list, keyword_tokens)) {
        line = parse_keyword_statement(token_list);   
    } else {
        line = parse_assignment_statement(token_list);
    }

    if (!token_list.empty()) {
        String token_disp;
        TokenKey next_token = std::get<TokenKey>(token_list.front()[0]);
        const String error_msg = "excess tokens starting at \'" + (display_token(next_token, token_disp), token_disp) + "\'";
        throw std::runtime_error(error_msg);
    }

    return line;
}

// Parse a statement beginning with a keyword.
//      token_list: linked list of remaining tokens (input)
Action parse_keyword_statement(std::list<Token>& token_list) {
    if (_lookahead(token_list, Let)) {
        return parse_let_statement(token_list);
    } else if (_lookahead(token_list, Now)) {
        return parse_now_statement(token_list);
    } else {
        throw std::runtime_error("line of code must start with a keyword");
    }
}

Action parse_assignment_statement(std::list<Token>& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    //  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    _match_bypass(token_list, Equals); // Bypass '='

//  Parse the expression after '='.
    expression = parse_expression(token_list);

    return std::make_shared<Reassign>(variable, expression, true);
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

    _match_bypass(token_list, Equals); // Bypass '='

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

    _match_bypass(token_list, Equals); // Bypass '='

//  Parse the expression following '='.
    expression = parse_expression(token_list);

    return std::make_shared<Reassign>(variable, expression, false);
}

// Parse any expression as defined in the Regal CFG.
//      token_list: linked list of remaining tokens (input)
Action parse_expression(std::list<Token>& token_list) {
    if (_lookahead_any(token_list, keyword_tokens)) {
        return parse_keyword_statement(token_list);
//  A Base Expression will link to many different types of expressions.
    } else if (_lookahead(token_list, Nothing)) {
        return ON_Nothing;
    }
    
    return parse_or_expression(token_list);
}

// The following three functions establish boolean order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

Action parse_or_expression(std::list<Token>& token_list) {
    Action and_expression;

    and_expression = parse_and_expression(token_list);

    if (_lookahead(token_list, Or)) {
        Action or_expression;

        _match_bypass(token_list, Or);

        or_expression = parse_or_expression(token_list);

        return std::make_shared<BinaryOperator>(Or, and_expression, or_expression);
    }

    return and_expression;
}

Action parse_and_expression(std::list<Token>& token_list) {
    Action not_expression;

    not_expression = parse_not_expression(token_list);

    if (_lookahead(token_list, And)) {
        Action and_expression;

        _match_bypass(token_list, And);

        and_expression = parse_and_expression(token_list);

        return std::make_shared<BinaryOperator>(And, not_expression, and_expression);
    }

    return not_expression;
}

Action parse_not_expression(std::list<Token>& token_list) {
    if (_lookahead(token_list, Not)) {
        Action comparison_expression;

        _match_bypass(token_list, Not);

        comparison_expression = parse_comparison_expression(token_list);

        return std::make_shared<UnaryOperator>(Not, comparison_expression);
    }

    return parse_comparison_expression(token_list);
}

Action parse_comparison_expression(std::list<Token>& token_list) {
    Action additive_expression;
    std::vector<TokenKey> comparison_operators;

    additive_expression = parse_additive_expression(token_list);

    comparison_operators = {Greater, Less, Equals};
    if (_lookahead_any(token_list, comparison_operators)) {
        Token operator_token;
        TokenKey operator_key;
        Action comparison_expression;

        _query_bypass_any(token_list, comparison_operators, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

        comparison_expression = parse_comparison_expression(token_list);

        return std::make_shared<BinaryOperator>(operator_key, additive_expression, comparison_expression);
    }

    return additive_expression;
}

// The following three functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a mathematical expression potentially containing '+' or '-'.
//      token_list: linked list of remaining tokens (input)
Action parse_additive_expression(std::list<Token>& token_list) {
    Action multiplicative_expression;
    std::vector<TokenKey> additive_tokens;
    
//  Bypass and store the Multiplicative Expression before '+' or '-'.
    multiplicative_expression = parse_multiplicative_expression(token_list);

//  Check for '+' or '-'.
    additive_tokens = {Plus, Minus};
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
    }
    
    return multiplicative_expression;
}

// Parse a mathematical expression potentially containing '*' or '/'.
//      token_list: linked list of remaining tokens (input)
Action parse_multiplicative_expression(std::list<Token>& token_list) {
    Action exponential_expression;
    std::vector<TokenKey> multiplicative_tokens;
    
//  Bypass and store the Exponential Expression before '*' or '/'.
    exponential_expression = parse_exponential_expression(token_list);

//  Check for '*' or '/'.
    multiplicative_tokens = {Mult, Div};
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
    }
    
    return exponential_expression;
}

// Parse a mathematical expression potentially containing '**' (exponents).
//      token_list: linked list of remaining tokens (input)
Action parse_exponential_expression(std::list<Token>& token_list) {
    Action minus_identifier_expression;
    
//  Bypass and store the Value Expression before '**'.
    minus_identifier_expression = parse_minus_identifier_expression(token_list);

//  Check for '**'.
    if (_lookahead(token_list, Exp)) {
        Action exponential_expression;

//      Bypass the exponential operator.
        _match_bypass(token_list, Exp);

//      Bypass and store the Exponential Expression after '**'.
        exponential_expression = parse_exponential_expression(token_list);

        return std::make_shared<BinaryOperator>(Exp, minus_identifier_expression, exponential_expression);
    }

    return minus_identifier_expression;
}

// Parse a mathematical expression containing fundamental values.
//      token_list: linked list of remaining tokens (input)
Action parse_minus_identifier_expression(std::list<Token>& token_list) {
//  Check for a '-' attached to the value.
//      e.g. '-(2+6)'
    if (_lookahead(token_list, Minus)) {
        Action primitive_expression;

//      Bypass the minus sign.
        _match_bypass(token_list, Minus);

//      Parse the expression that the '-' is attached to.
        primitive_expression = parse_primitive_expression(token_list);

//      Multiply the subsequent expression by -1.
        return std::make_shared<BinaryOperator>(Mult, std::make_shared<Integer>(-1), primitive_expression);
    }

    return parse_primitive_expression(token_list);
}

// Parse a Primitive Expression.
//      token_list: linked list of remaining tokens (input)
Action parse_primitive_expression(std::list<Token>& token_list) {
    if (_lookahead(token_list, Var)) {
        Token variable_token;
        String variable;

//      Bypass and store the variable name.
        _query_bypass(token_list, Var, variable_token);
        variable = std::get<String>(variable_token[1]);

        return std::make_shared<Variable>(variable);

    } else if (_lookahead_any(token_list, number_tokens)) {
        return parse_number_expression(token_list);
    } else if (_lookahead(token_list, Bool)) {
        return parse_boolean_expression(token_list);
    }
    Action or_expression;

    _match_bypass(token_list, LeftPar); // Bypass '('

//      Bypass and store the encased expression.
    or_expression = parse_or_expression(token_list);

    _match_bypass(token_list, RightPar); // Bypass ')'

    return or_expression;
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

Action parse_boolean_expression(std::list<Token>& token_list) {
    Token boolean_token;
    bool boolean_value;

    _query_bypass(token_list, Bool, boolean_token);
    boolean_value = std::get<int>(boolean_token[1]);

    return std::make_shared<Boolean>(boolean_value);
}
