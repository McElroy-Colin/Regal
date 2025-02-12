// File containing the Regal AST generator.

#include "../../../include/inc_langdef/langdef.hpp"
#include "../../../include/inc_debug/error_handling.hpp"

using std::list, std::vector, std::make_shared, std::get;
using namespace TokenDef;
using namespace CodeTree;
using namespace DebugUtils;

// parsing function definitions coded with the CFG.
syntaxNode parse_file(list<token>& token_list);
syntaxNode parse_code(list<token>& token_list, const int min_indent);
syntaxNode parse_line_init(list<token>& token_list, const int min_indent);

syntaxNode parse_if_block(list<token>& token_list, const int min_indent);
syntaxNode parse_else_block(list<token>& token_list, const int min_indent);

syntaxNode parse_code_block(list<token>& token_list, const int min_indent);

syntaxNode parse_single_operation(list<token>& token_list, const int min_indent);
syntaxNode parse_assignment(list<token>& token_list);
syntaxNode parse_inline_if_statement(list<token>& token_list);
syntaxNode parse_single_operation(list<token>& token_list);

// VARIABLE ASSIGNMENT:
syntaxNode parse_explicit_assignment(list<token>& token_list);
syntaxNode parse_let_statement(list<token>& token_list);
syntaxNode parse_now_statement(list<token>& token_list);
syntaxNode parse_implicit_assignment(list<token>& token_list);

syntaxNode parse_expression(list<token>& token_list);

// BOOLEAN ARITHMETIC:
syntaxNode parse_or_expression(list<token>& token_list);
syntaxNode parse_exclusive_or_expression(list<token>& token_list);
syntaxNode parse_and_expression(list<token>& token_list);
syntaxNode parse_not_expression(list<token>& token_list);
syntaxNode parse_comparison_expression(list<token>& token_list);

// NUMERICAL ARITHMETIC:
syntaxNode parse_additive_expression(list<token>& token_list);
syntaxNode parse_multiplicative_expression(list<token>& token_list);
syntaxNode parse_exponential_expression(list<token>& token_list);

// LOW-LEVEL VALUES:
syntaxNode parse_minus_identifier_expression(list<token>& token_list);
syntaxNode parse_primitive_expression(list<token>& token_list);
syntaxNode parse_number_expression(list<token>& token_list);
syntaxNode parse_boolean_expression(list<token>& token_list);

// Anonymous namespace containing parsing helper functions.
namespace {

//  Return true if the first element of the given list is the given token.
//      token_list: linked list of remaining tokens (input)
//      target_token: token to compare the head of the list against (input)
    bool _lookahead(list<token>& token_list, const tokenKey target_token) {
        if (token_list.empty()) {
            return false;
        }

        return (get<tokenKey>(token_list.front()[0]) == target_token);
    }


//  Return the integer amount of indent the newline in front of the given token list has.
//  This function will error if the first element of token_list is not a newline.
//      token_list: linked list of remaining tokens (input)
    int _retrieve_indent(list<token>& token_list) {
        if (_lookahead(token_list, Newline)) {
            return get<int>(token_list.front()[1]);
        }
        
//      Handle no newline.
        token_list.empty() 
            ? throw UnexpectedInputError(Newline, Literal) 
            : throw UnexpectedInputError(token_list.front(), Newline, Literal);
    }

//  Return true if the first element of the given list is any of the given tokens.
//      token_list: linked list of remaining tokens (input)
//      target_tokens: tokens to compare the head of the list against (input)
    bool _lookahead_any(list<token>& token_list, const vector<tokenKey>& target_tokens) {
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
    void _match_bypass(list<token>& token_list, const tokenKey target_token) {
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
    void _query_bypass(list<token>& token_list, const tokenKey target_token, token& matched_token) {
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
    void _query_bypass_any(list<token>& token_list, const vector<tokenKey>& target_tokens, token& matched_token) {
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
// All functions return nodes corresponding to nodes of the AST. 
// The syntaxNode definition can be found in ~/include/inc_langdef/langdef.hpp.

// Generate and return an AST of nodes from a list of tokens.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_file(list<token>& token_list) {
    syntaxNode code = parse_code(token_list, -1);

    if (!token_list.empty()) {
        throw UnexpectedInputError(token_list.front(), Literal);
    }

    return code;
}

// Parse a sequence of code blocks and operations recursively.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
syntaxNode parse_code(list<token>& token_list, const int min_indent) {
    syntaxNode line_init;
    
    line_init = parse_line_init(token_list, min_indent);

    if (!token_list.empty()) {
        syntaxNode code;

        code = parse_code(token_list, min_indent);

        return make_shared<codeBlock>(line_init, code);
    }

    return line_init;
}

// Parse a sequence of code blocks and operations recursively.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
syntaxNode parse_line_init(list<token>& token_list, const int min_indent) {
    token newline_token;

    _query_bypass(token_list, Newline, newline_token);

    if (_lookahead(token_list, If)) {
        return parse_if_block(token_list, get<int>(newline_token[1]));
    }

    return parse_code_block(token_list, min_indent);
}

// Parse an if-else block of code.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
syntaxNode parse_if_block(list<token>& token_list, const int min_indent) {
    syntaxNode or_expression, line_init;
    int next_indent;

    _match_bypass(token_list, If);

    or_expression = parse_or_expression(token_list);

//  Ensure that the next line is more indented than the if statement.
    next_indent = _retrieve_indent(token_list);
    if (next_indent <= min_indent) {
        throw MissingCodeBlockError("expected an indented code block");
    }

    line_init = parse_line_init(token_list, min_indent);

//  Parse an else block if one exists.
    if (_lookahead(token_list, Else)) {
        syntaxNode else_block = parse_else_block(token_list, min_indent);
        return make_shared<ifBlock>(or_expression, line_init, else_block);
    }

    return make_shared<ifBlock>(or_expression, line_init);
}

// Parse an else block attached to an if block.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
syntaxNode parse_else_block(list<token>& token_list, const int min_indent) {
    int next_indent;

    _match_bypass(token_list, Else);
    next_indent = _retrieve_indent(token_list);

    if (next_indent < min_indent) {
        throw MissingCodeBlockError("expected an indented code block");
    }

    return parse_line_init(token_list, min_indent);
}

// Parse a block of code recursively.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
syntaxNode parse_code_block(list<token>& token_list, const int min_indent) {
    syntaxNode single_operation;

    single_operation = parse_single_operation(token_list);

    if (_lookahead(token_list, Newline)) {
        syntaxNode line_init;
        int new_indent;
        
        new_indent = _retrieve_indent(token_list);

//      Check for the end of the scope or a final newline. TODO: remove final newlines during lexing.
        if (token_list.size() == 1) {
            _match_bypass(token_list, Newline);
            return single_operation;
        } else if (new_indent <= min_indent) {
            return single_operation;
        }

        line_init = parse_line_init(token_list, min_indent);

        return make_shared<codeBlock>(single_operation, line_init);
    }
    
    return single_operation;
}

// Generate and return an AST of nodes from a list of tokens.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_single_operation(list<token>& token_list) {
    return parse_assignment(token_list);
}

// Parse the assignment of a variable.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_assignment(list<token>& token_list) {
    if (_lookahead_any(token_list, {Let, Now})) {
        return parse_explicit_assignment(token_list);
    }

    return parse_implicit_assignment(token_list);
}

// Parse the assignment of a variable with an assignment keyword.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_explicit_assignment(list<token>& token_list) {
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
syntaxNode parse_let_statement(list<token>& token_list) {
    syntaxNode inline_if_statement;
    token variable_token;
    string variable;

    _match_bypass(token_list, Let);

//  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = get<string>(variable_token[1]);

    _match_bypass(token_list, Bind);

//  Parse the expression after '='.
    inline_if_statement = parse_inline_if_statement(token_list);

    return make_shared<assignOp>(variable, inline_if_statement);
}

// Parse a 'now' reassignment statement.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_now_statement(list<token>& token_list) {
    syntaxNode inline_if_statement;
    token variable_token;
    string variable;

    _match_bypass(token_list, Now);

//  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = get<string>(variable_token[1]);

    _match_bypass(token_list, Bind);

//  Parse the expression following '='.
    inline_if_statement = parse_inline_if_statement(token_list);

//  Parameter boolean is false since this is an explicit ('now') reassignment.
    return make_shared<reassignOp>(variable, inline_if_statement, false);
}

// Parse an implicit reassignment statement.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_implicit_assignment(list<token>& token_list) {
    syntaxNode inline_if_statement;
    token variable_token;
    string variable;

    //  Bypass and store the variable name.
    _query_bypass(token_list, Var, variable_token);
    variable = get<string>(variable_token[1]);

    _match_bypass(token_list, Bind);

//  Parse the expression after '='.
    inline_if_statement = parse_inline_if_statement(token_list);

//  Parameter boolean is true since this is an implicit reassignment.
    return make_shared<reassignOp>(variable, inline_if_statement, true);
}

// Parse a potentially inline ternary if statement.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_inline_if_statement(list<token>& token_list) {
    syntaxNode expression;

    expression = parse_expression(token_list);

//  Check for a ternary if statement.
    if (_lookahead(token_list, If)) {
        syntaxNode or_expression, inline_if_statement;

        _match_bypass(token_list, If);

        or_expression = parse_or_expression(token_list);

        _match_bypass(token_list, Else);

        inline_if_statement = parse_inline_if_statement(token_list);

        return make_shared<ternaryOp>(If, expression, or_expression, inline_if_statement);
    }

    return expression;
}

// Parse any expression as defined in the Regal CFG.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_expression(list<token>& token_list) {
    return parse_or_expression(token_list);
}

// The following four functions establish boolean order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a boolean expression potentially containing '|' or 'or'.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_or_expression(list<token>& token_list) {
    syntaxNode xor_expression;
    vector<tokenKey> or_operators;

//  Parse and store the expression before '|' or 'or'.
    xor_expression = parse_exclusive_or_expression(token_list);

    or_operators = {Or, OrW};
    if (_lookahead_any(token_list, or_operators)) {
        syntaxNode or_expression;
        token operator_token;
        tokenKey operator_key;

//      Bypass and store the OR operator.
        _query_bypass_any(token_list, or_operators, operator_token);
        operator_key = get<tokenKey>(operator_token[0]);

//      Parse and store the expression after '|' or 'or'.
        or_expression = parse_or_expression(token_list);

        return make_shared<binaryOp>(operator_key, xor_expression, or_expression);
    }

    return xor_expression;
}

// Parse a boolean expression potentially containing '||' or 'xor'.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_exclusive_or_expression(list<token>& token_list) {
    syntaxNode and_expression;
    vector<tokenKey> xor_operators;

//  Parse and store the expression before '||' or 'xor'.
    and_expression = parse_and_expression(token_list);

    xor_operators = {Xor, XorW};
    if (_lookahead_any(token_list, xor_operators)) {
        syntaxNode or_expression;
        token operator_token;
        tokenKey operator_key;

//      Bypass and store the XOR operator.
        _query_bypass_any(token_list, xor_operators, operator_token);
        operator_key = get<tokenKey>(operator_token[0]);

//      Parse and store the expression after '||' or 'xor'.
        or_expression = parse_or_expression(token_list);

        return make_shared<binaryOp>(operator_key, and_expression, or_expression);
    }

    return and_expression;
}

// Parse a boolean expression potentially containing '&' or 'and'.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_and_expression(list<token>& token_list) {
    syntaxNode not_expression;
    vector<tokenKey> and_operators;

//  Parse and store the expression potentially containing '&' or 'and'.
    not_expression = parse_not_expression(token_list);

    and_operators = {And, AndW};
    if (_lookahead_any(token_list, and_operators)) {
        syntaxNode and_expression;
        token operator_token;
        tokenKey operator_key;

//      Bypass and store the AND operator.
        _query_bypass_any(token_list, and_operators, operator_token);
        operator_key = get<tokenKey>(operator_token[0]);

//      Parse and store the expression after '&' or 'and'.
        and_expression = parse_and_expression(token_list);

        return make_shared<binaryOp>(operator_key, not_expression, and_expression);
    }

    return not_expression;
}

// Parse a boolean expression potentially containing '!' or 'not'.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_not_expression(list<token>& token_list) {
    vector<tokenKey> not_operators = {Not, NotW};
    if (_lookahead_any(token_list, not_operators)) {
        syntaxNode comparison_expression;
        token operator_token;
        tokenKey operator_key;

//      Bypass and store the AND operator.
        _query_bypass_any(token_list, not_operators, operator_token);
        operator_key = get<tokenKey>(operator_token[0]);

//      Parse and store the expression after '!' or 'not'.
        comparison_expression = parse_comparison_expression(token_list);

        return make_shared<unaryOp>(operator_key, comparison_expression);
    }

    return parse_comparison_expression(token_list);
}

// Parse an expression potentially comparing values with '>', '<', '=', or 'is'.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_comparison_expression(list<token>& token_list) {
    syntaxNode additive_expression;
    vector<tokenKey> comparison_operators;

// Parse and store the expression before '>', '<', '=', or 'is'.
    additive_expression = parse_additive_expression(token_list);

    comparison_operators = {Greater, Less, Equals, Is};
    if (_lookahead_any(token_list, comparison_operators)) {
        token operator_token;
        tokenKey operator_key;
        syntaxNode comparison_expression;

//      Bypass and store the comparison operator.
        _query_bypass_any(token_list, comparison_operators, operator_token);
        operator_key = get<tokenKey>(operator_token[0]);

//      Parse and store the expression after '>', '<', '=', or 'is'.
        comparison_expression = parse_comparison_expression(token_list);

        return make_shared<binaryOp>(operator_key, additive_expression, comparison_expression);
    }

    return additive_expression;
}

// The following three functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a mathematical expression potentially containing '+' or '-'.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_additive_expression(list<token>& token_list) {
    syntaxNode multiplicative_expression;
    vector<tokenKey> additive_tokens;
    
//  Parse and store the expression before '+' or '-'.
    multiplicative_expression = parse_multiplicative_expression(token_list);

    additive_tokens = {Plus, Minus};
    if (_lookahead_any(token_list, additive_tokens)) {
        token operator_token;
        tokenKey operator_key;
        syntaxNode additive_expression;

//      Bypass and store the additive operator.
        _query_bypass_any(token_list, additive_tokens, operator_token);
        operator_key = get<tokenKey>(operator_token[0]);

//      Parse and store the expression after '+' or '-'.
        additive_expression = parse_additive_expression(token_list);

        return make_shared<binaryOp>(operator_key, multiplicative_expression, additive_expression);
    }
    
    return multiplicative_expression;
}

// Parse a mathematical expression potentially containing '*' or '/'.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_multiplicative_expression(list<token>& token_list) {
    syntaxNode exponential_expression;
    vector<tokenKey> multiplicative_tokens;
    
//  Parse and store the expression before '*' or '/'.
    exponential_expression = parse_exponential_expression(token_list);

    multiplicative_tokens = {Mult, Div};
    if (_lookahead_any(token_list, multiplicative_tokens)) {
        token operator_token;
        tokenKey operator_key;
        syntaxNode multiplicative_expression;

//      Bypass and store the multiplicative operator.
        _query_bypass_any(token_list, multiplicative_tokens, operator_token);
        operator_key = get<tokenKey>(operator_token[0]);

//      Parse and store the expression after '*' or '/'.
        multiplicative_expression = parse_multiplicative_expression(token_list);

        return make_shared<binaryOp>(operator_key, exponential_expression, multiplicative_expression);
    }
    
    return exponential_expression;
}

// Parse a mathematical expression potentially containing '**' (exponents).
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_exponential_expression(list<token>& token_list) {
    syntaxNode minus_identifier_expression;
    
//  Parse and store the expression before '**'.
    minus_identifier_expression = parse_minus_identifier_expression(token_list);

    if (_lookahead(token_list, Exp)) {
        syntaxNode exponential_expression;

        _match_bypass(token_list, Exp);

//      Parse and store the expression after '**'.
        exponential_expression = parse_exponential_expression(token_list);

        return make_shared<binaryOp>(Exp, minus_identifier_expression, exponential_expression);
    }

    return minus_identifier_expression;
}

// Parse a mathematical expression potentially starting with a negation.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_minus_identifier_expression(list<token>& token_list) {
//  Check for a '-' attached to the expression.
//      e.g. '-(2+6)'
    if (_lookahead(token_list, Minus)) {
        syntaxNode primitive_expression;

        _match_bypass(token_list, Minus);

//      Parse the expression that the '-' is attached to.
        primitive_expression = parse_primitive_expression(token_list);

//      Multiply the subsequent expression by -1 to simulate negation.
        return make_shared<binaryOp>(Mult, make_shared<intContainer>(-1), primitive_expression);
    }

    return parse_primitive_expression(token_list);
}

// Parse an expression with low-level values or encased expressions.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_primitive_expression(list<token>& token_list) {
//  Check for different low-level values.
    if (_lookahead(token_list, Var)) {
        token variable_token;
        string variable;

//      Bypass and store the variable name.
        _query_bypass(token_list, Var, variable_token);
        variable = get<string>(variable_token[1]);

        return make_shared<varContainer>(variable);

    } else if (_lookahead_any(token_list, number_tokens)) {
        return parse_number_expression(token_list);
    } else if (_lookahead(token_list, Bool)) {
        return parse_boolean_expression(token_list);
    }
    syntaxNode inline_if_statement;

    _match_bypass(token_list, LeftPar);

//  Parse and store the encased statement.
    inline_if_statement = parse_inline_if_statement(token_list);

    _match_bypass(token_list, RightPar);

    return inline_if_statement;
}

// Parse an expression containing just a number token.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_number_expression(list<token>& token_list) {
    token number_token;
    tokenKey number_type;
    
//  Bypass and identify the type of number token.
    _query_bypass_any(token_list, number_tokens, number_token);
    number_type = get<tokenKey>(number_token[0]);
    
    switch (number_type) {
        case Int: // currently, only integers are supported in Regal.
            int number;
            
            number = get<int>(number_token[1]);
            return make_shared<intContainer>(number);
        default:
            throw FatalError("unrecognized number type in number expression");
    }
}

// Parse an expression containing just a boolean token.
//      token_list: linked list of remaining tokens (input)
syntaxNode parse_boolean_expression(list<token>& token_list) {
    token boolean_token;
    bool boolean_value;

//  Bypass and store the boolean value.
    _query_bypass(token_list, Bool, boolean_token);
    boolean_value = get<int>(boolean_token[1]);

    return make_shared<boolContainer>(boolean_value);
}
