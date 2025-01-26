// File containing the Regal AST generator.

#include "../../../include/inc_langdef/langdef.hpp"
#include "../../../include/inc_debug/error_handling.hpp"


// parsing function definitions coded with the CFG.

Action parse_line(std::list<Token>& token_list);

// VARIABLE ASSIGNMENT:
Action parse_explicit_assignment(std::list<Token>& token_list);
Action parse_let_statement(std::list<Token>& token_list);
Action parse_now_statement(std::list<Token>& token_list);
Action parse_implicit_assignment(std::list<Token>& token_list);

Action parse_expression(std::list<Token>& token_list);

// BOOLEAN ARITHMETIC:
Action parse_or_expression(std::list<Token>& token_list);
Action parse_and_expression(std::list<Token>& token_list);
Action parse_not_expression(std::list<Token>& token_list);
Action parse_comparison_expression(std::list<Token>& token_list);

// NUMERICAL ARITHMETIC:
Action parse_additive_expression(std::list<Token>& token_list);
Action parse_multiplicative_expression(std::list<Token>& token_list);
Action parse_exponential_expression(std::list<Token>& token_list);

// LOW-LEVEL VALUES:
Action parse_minus_identifier_expression(std::list<Token>& token_list);
Action parse_primitive_expression(std::list<Token>& token_list);
Action parse_number_expression(std::list<Token>& token_list);
Action parse_boolean_expression(std::list<Token>& token_list);


// Anonymous namespace containing parsing helper functions.
namespace {

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

//      Handle incorrect token error.
        token_list.empty() 
            ? throw UnexpectedInputError(target_token, Literal) 
            : throw UnexpectedInputError(token_list.front(), target_token, Literal);
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

//      Handle incorrect token error.
        token_list.empty() 
            ? throw UnexpectedInputError(target_token, Literal) 
            : throw UnexpectedInputError(token_list.front(), target_token, Literal);
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

//      Handle incorrect token type error.
        token_list.empty() 
            ? throw UnexpectedInputError(target_tokens[0], Key) 
            : throw UnexpectedInputError(token_list.front(), target_tokens[0], Key);
    }

}

// The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG
// The CFG can be found in ~docs/docs_langdef/CFG.txt.
// All functions return Actions corresponding to nodes of the AST. 
// The Action definition can be found in ~/include/inc_langdef/langdef.hpp.

// Generate and return an AST of Actions from a list of tokens.
//      token_list: linked list of remaining tokens (input)
Action parse_line(std::list<Token>& token_list) {
    Action line;

    if (_lookahead_any(token_list, keyword_tokens)) {
        line = parse_explicit_assignment(token_list);   
    } else {
        line = parse_implicit_assignment(token_list);
    }

//  Handle if there are more tokens after parsing.
    token_list.empty() ?: throw UnexpectedInputError(token_list.front(), Literal);

    return line;
}

// Parse the assignment of a variable with an assignment keyword.
//      token_list: linked list of remaining tokens (input)
Action parse_explicit_assignment(std::list<Token>& token_list) {
    if (_lookahead(token_list, Let)) {
        return parse_let_statement(token_list);
    } else if (_lookahead(token_list, Now)) {
        return parse_now_statement(token_list);
    }

//  Handle incorrect token type error.
    token_list.empty() 
        ? throw UnexpectedInputError({Let}, Subset) 
        : throw UnexpectedInputError(token_list.front(), {Let}, Subset);
}

// Parse a 'let' assignment statement.
//      token_list: linked list of remaining tokens (input)
Action parse_let_statement(std::list<Token>& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    _match_bypass(token_list, Let);

//  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    _match_bypass(token_list, Equals);

//  Parse the expression after '='.
    expression = parse_expression(token_list);

    return std::make_shared<Assign>(variable, expression);
}

// Parse a 'now' reassignment statement.
//      token_list: linked list of remaining tokens (input)
Action parse_now_statement(std::list<Token>& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    _match_bypass(token_list, Now);

//  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    _match_bypass(token_list, Equals);

//  Parse the expression following '='.
    expression = parse_expression(token_list);

//  Parameter boolean is false since this is an explicit ('now') reassignment.
    return std::make_shared<Reassign>(variable, expression, false);
}

// Parse an implicit reassignment statement.
//      token_list: linked list of remaining tokens (input)
Action parse_implicit_assignment(std::list<Token>& token_list) {
    Action expression;
    Token variable_token;
    String variable;

    //  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = std::get<String>(variable_token[1]);

    _match_bypass(token_list, Equals);

//  Parse the expression after '='.
    expression = parse_expression(token_list);

//  Parameter boolean is true since this is an implicit reassignment.
    return std::make_shared<Reassign>(variable, expression, true);
}

// Parse any expression as defined in the Regal CFG.
//      token_list: linked list of remaining tokens (input)
Action parse_expression(std::list<Token>& token_list) {
    if (_lookahead(token_list, Nothing)) {
        return OtherNodes::ON_Nothing;
    }
    
    return parse_or_expression(token_list);
}

// The following three functions establish boolean order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a boolean expression potentially containing '|' or 'or'.
//      token_list: linked list of remaining tokens (input)
Action parse_or_expression(std::list<Token>& token_list) {
    Action and_expression;

//  Parse and store the expression before '|' or 'or'.
    and_expression = parse_and_expression(token_list);

    if (_lookahead(token_list, Or)) {
        Action or_expression;

        _match_bypass(token_list, Or);

//      Parse and store the expression after '|' or 'or'.
        or_expression = parse_or_expression(token_list);

        return std::make_shared<BinaryOperator>(Or, and_expression, or_expression);
    }

    return and_expression;
}

// Parse a boolean expression potentially containing '&' or 'and'.
//      token_list: linked list of remaining tokens (input)
Action parse_and_expression(std::list<Token>& token_list) {
    Action not_expression;

//  Parse and store the expression potentially containing '&' or 'and'.
    not_expression = parse_not_expression(token_list);

    if (_lookahead(token_list, And)) {
        Action and_expression;

        _match_bypass(token_list, And);

//      Parse and store the expression after '&' or 'and'.
        and_expression = parse_and_expression(token_list);

        return std::make_shared<BinaryOperator>(And, not_expression, and_expression);
    }

    return not_expression;
}

// Parse a boolean expression potentially containing '!' or 'not'.
//      token_list: linked list of remaining tokens (input)
Action parse_not_expression(std::list<Token>& token_list) {
    if (_lookahead(token_list, Not)) {
        Action comparison_expression;

        _match_bypass(token_list, Not);

//      Parse and store the expression after '!' or 'not'.
        comparison_expression = parse_comparison_expression(token_list);

        return std::make_shared<UnaryOperator>(Not, comparison_expression);
    }

    return parse_comparison_expression(token_list);
}

// Parse an expression potentially comparing values with '>', '<', or '='.
//      token_list: linked list of remaining tokens (input)
Action parse_comparison_expression(std::list<Token>& token_list) {
    Action additive_expression;
    std::vector<TokenKey> comparison_operators;

// Parse and store the expression before '>', '<', or '='.
    additive_expression = parse_additive_expression(token_list);

    comparison_operators = {Greater, Less, Equals};
    if (_lookahead_any(token_list, comparison_operators)) {
        Token operator_token;
        TokenKey operator_key;
        Action comparison_expression;

//      Bypass and store the comparison operator.
        _query_bypass_any(token_list, comparison_operators, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Parse and store the expression after '>', '<', or '='.
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
    
//  Parse and store the expression before '+' or '-'.
    multiplicative_expression = parse_multiplicative_expression(token_list);

    additive_tokens = {Plus, Minus};
    if (_lookahead_any(token_list, additive_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action additive_expression;

//      Bypass and store the additive operator.
        _query_bypass_any(token_list, additive_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Parse and store the expression after '+' or '-'.
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
    
//  Parse and store the expression before '*' or '/'.
    exponential_expression = parse_exponential_expression(token_list);

    multiplicative_tokens = {Mult, Div};
    if (_lookahead_any(token_list, multiplicative_tokens)) {
        Token operator_token;
        TokenKey operator_key;
        Action multiplicative_expression;

//      Bypass and store the multiplicative operator.
        _query_bypass_any(token_list, multiplicative_tokens, operator_token);
        operator_key = std::get<TokenKey>(operator_token[0]);

//      Parse and store the expression after '*' or '/'.
        multiplicative_expression = parse_multiplicative_expression(token_list);

        return std::make_shared<BinaryOperator>(operator_key, exponential_expression, multiplicative_expression);
    }
    
    return exponential_expression;
}

// Parse a mathematical expression potentially containing '**' (exponents).
//      token_list: linked list of remaining tokens (input)
Action parse_exponential_expression(std::list<Token>& token_list) {
    Action minus_identifier_expression;
    
//  Parse and store the expression before '**'.
    minus_identifier_expression = parse_minus_identifier_expression(token_list);

    if (_lookahead(token_list, Exp)) {
        Action exponential_expression;

        _match_bypass(token_list, Exp);

//      Parse and store the expression after '**'.
        exponential_expression = parse_exponential_expression(token_list);

        return std::make_shared<BinaryOperator>(Exp, minus_identifier_expression, exponential_expression);
    }

    return minus_identifier_expression;
}

// Parse a mathematical expression potentially starting with a negation.
//      token_list: linked list of remaining tokens (input)
Action parse_minus_identifier_expression(std::list<Token>& token_list) {
//  Check for a '-' attached to the expression.
//      e.g. '-(2+6)'
    if (_lookahead(token_list, Minus)) {
        Action primitive_expression;

        _match_bypass(token_list, Minus);

//      Parse the expression that the '-' is attached to.
        primitive_expression = parse_primitive_expression(token_list);

//      Multiply the subsequent expression by -1 to simulate negation.
        return std::make_shared<BinaryOperator>(Mult, std::make_shared<Integer>(-1), primitive_expression);
    }

    return parse_primitive_expression(token_list);
}

// Parse an expression with low-level values or encased expressions.
//      token_list: linked list of remaining tokens (input)
Action parse_primitive_expression(std::list<Token>& token_list) {
//  Check for different low-level values.
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

    _match_bypass(token_list, LeftPar);

//  Parse and store the encased expression.
    or_expression = parse_or_expression(token_list);

    _match_bypass(token_list, RightPar);

    return or_expression;
}

// Parse an expression containing just a number token.
//      token_list: linked list of remaining tokens (input)
Action parse_number_expression(std::list<Token>& token_list) {
    Token number_token;
    TokenKey number_type;
    
//  Bypass and identify the type of number token.
    _query_bypass_any(token_list, number_tokens, number_token);
    number_type = std::get<TokenKey>(number_token[0]);
    
    switch (number_type) {
        case Int: // currently, only integers are supported in Regal.
            int number;
            
            number = std::get<int>(number_token[1]);
            return std::make_shared<Integer>(number);
        default:
            throw FatalError("unrecognized number type in number expression");
    }
}

// Parse an expression containing just a boolean token.
//      token_list: linked list of remaining tokens (input)
Action parse_boolean_expression(std::list<Token>& token_list) {
    Token boolean_token;
    bool boolean_value;

//  Bypass and store the boolean value.
    _query_bypass(token_list, Bool, boolean_token);
    boolean_value = std::get<int>(boolean_token[1]);

    return std::make_shared<Boolean>(boolean_value);
}
