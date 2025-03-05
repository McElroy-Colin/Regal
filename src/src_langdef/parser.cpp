// File containing the Regal AST generator.

#include "inc_langdef/parser.hpp"

using std::string, std::list, std::array, std::shared_ptr, std::size_t, std::make_shared, std::get, std::advance;
using namespace TokenDef;
using namespace CodeTree;
using namespace TypingUtils;
using namespace DebugUtils;


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

//  Return true if the first element of the given list is any of the given tokens.
//      token_list: linked list of remaining tokens (input)
//      target_tokens: tokens to compare the head of the list against (input)
    template <size_t num>
    bool _lookahead_any(list<token>& token_list, const array<tokenKey, num>& target_tokens) {
        if (token_list.empty()) {
            return false;
        }
        
        for (tokenKey target : target_tokens) {
            if (_lookahead(token_list, target)) {
                return true;
            }
        }
        return false;
    }

//  Return true if the given index element of the given list is the given token.
//      token_list: linked list of remaining tokens (input)
//      target_token: token to compare the head of the list against (input)
//      degrees_ahead: index of element to compare against, 0 is the first element (input)
    bool _lookahead_many(list<token>& token_list, const tokenKey target_token, const unsigned int degrees_ahead) {
        list<token>::iterator iter = token_list.begin();
        advance(iter, degrees_ahead);

        if (iter == token_list.end()) {
            return false;
        }

        return (get<tokenKey>((*iter)[0]) == target_token);
    }

//  Return the integer amount of indent the newline in front of the given token list has.
//  This function will error if the first element of token_list is not a newline.
//      token_list: linked list of remaining tokens (input)
    int _retrieve_indent(list<token>& token_list) {
        if (_lookahead(token_list, tokenKey::Newline)) {
            return get<int>(token_list.front()[1]);
        }
        
//      Handle no newline.
        token_list.empty() 
            ? throw UnexpectedInputError(tokenKey::Newline, tokenDispOption::Literal) 
            : throw UnexpectedInputError(token_list.front(), tokenKey::Newline, tokenDispOption::Literal);
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
        (token_list.empty() || ((token_list.size() == 1) && _lookahead(token_list, tokenKey::Newline)))
            ? throw UnexpectedInputError(target_token, tokenDispOption::Key) 
            : throw UnexpectedInputError(token_list.front(), target_token, tokenDispOption::Literal);
    }

//  Pop the first element of the given list and store it if it matches the given token.
//      token_list: linked list of remaining tokens (input)
//      target_token: token to match the head of the list with (input)
//      matched_token: token matched with the list (output)
    void _query_bypass(list<token>& token_list, const tokenKey target_token, token& matched_token) {
        if (_lookahead(token_list, target_token)) {
            matched_token = token_list.front();
            token_list.pop_front();

            return;
        }

//      Handle incorrect token error.
        (token_list.empty() || ((token_list.size() == 1) && _lookahead(token_list, tokenKey::Newline))) 
            ? throw UnexpectedInputError(target_token, tokenDispOption::Key) 
            : throw UnexpectedInputError(token_list.front(), target_token, tokenDispOption::Literal);
    }

//  Pop the first element of the given list and store its data if it matches any of the given tokens.
//      token_list: linked list of remaining tokens (input)
//      target_tokens: tokens to match with the head of the list (input)
//      matched_token: token matched with the list (output)
    template <size_t num>
    void _query_bypass_any(list<token>& token_list, const array<tokenKey, num>& target_tokens, token& matched_token) {
        if (_lookahead_any(token_list, target_tokens)) {
            matched_token = token_list.front();
            token_list.pop_front();

            return;
        }

//  Handle incorrect token type error.
    (token_list.empty() || ((token_list.size() == 1) && _lookahead(token_list, tokenKey::Newline))) 
        ? throw UnexpectedInputError(target_tokens[0], tokenDispOption::Key) 
        : throw UnexpectedInputError(token_list.front(), target_tokens[0], tokenDispOption::Key);
    }
}

// The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG
// The CFG can be found in ~docs/docs_langdef/CFG.txt.
// All functions return nodes corresponding to nodes of the AST. 
// The shared_ptr<dataNode> definition can be found in ~/include/inc_langdef/langdef.hpp.

// Generate and return an AST of nodes from a list of tokens.
//      token_list: linked list of tokens (input)
shared_ptr<dataNode> parse_file(list<token>& token_list) {
//  -1 minimum indent indicates the file baseline scope.
    return parse_code_scope(token_list, -1);
}

// Parse a sequence of operations in the same scope recursively.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
shared_ptr<dataNode> parse_code_scope(list<token>& token_list, const int min_indent) {
    shared_ptr<dataNode> current_operation, code_scope;
    token newline_token;
    int post_operation_indent;

    _query_bypass(token_list, tokenKey::Newline, newline_token);
    
//  Parse this scope's current operation.
    if (_lookahead(token_list, tokenKey::If)) {
        current_operation = parse_if_block(token_list, get<int>(newline_token[1]));
    } else {
        current_operation = parse_inline_operation(token_list);
    }

//  Recursively exit the current scope if indent decrease.
    post_operation_indent = _retrieve_indent(token_list);
    if (post_operation_indent <= min_indent) {
        return current_operation;
    }

    code_scope = parse_code_scope(token_list, min_indent);

    return make_shared<codeScope>(current_operation, code_scope);
}

// Parse an if-else block of code.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
shared_ptr<dataNode> parse_if_block(list<token>& token_list, const int min_indent) {
    shared_ptr<valueData> expression;
    shared_ptr<dataNode> code_scope;
    int next_indent;

    _match_bypass(token_list, tokenKey::If);
    expression = parse_expression(token_list);

//  Ensure that the next line is more indented than the if statement.
    next_indent = _retrieve_indent(token_list);
    if (next_indent <= min_indent) {
        throw UnexpectedIndentError(tokenKey::If);
    }

    code_scope = parse_code_scope(token_list, min_indent);

//  Parse an else block if one exists.
    if (_lookahead_many(token_list, tokenKey::Else, 1)) {
        shared_ptr<dataNode> else_block = parse_else_block(token_list, min_indent);
        return make_shared<ifBlock>(expression, code_scope, else_block);
    }

    return make_shared<ifBlock>(expression, code_scope);
}

// Parse an else block attached to an if block.
//      token_list: linked list of remaining tokens (input)
//      min_index: current scope minimum indent requirement (input)
shared_ptr<dataNode> parse_else_block(list<token>& token_list, const int min_indent) {
    token newline_token;
    int next_indent;

    _query_bypass(token_list, tokenKey::Newline, newline_token);

//  Ensure that the 'else' indent amatches the 'if' indent.
    if (get<int>(newline_token[1]) != min_indent) {
        throw UnexpectedIndentError(tokenKey::Else, tokenKey::If);
    }
    
    _match_bypass(token_list, tokenKey::Else);

//  Allow for 'else if' chains.
    if(_lookahead(token_list, tokenKey::If)) {
        return parse_if_block(token_list, min_indent);
    }

//  Ensure that the next line is more indented than 'else'.
    next_indent = _retrieve_indent(token_list);
    if (next_indent <= min_indent) {
        throw UnexpectedIndentError(tokenKey::Else);
    }

    return parse_code_scope(token_list, min_indent);
}

// Parse an inline operation.
//      token_list: linked list of remaining tokens (input)
shared_ptr<dataNode> parse_inline_operation(list<token>& token_list) {
    return parse_assignment(token_list);
}

// Parse the assignment of a variable.
//      token_list: linked list of remaining tokens (input)
shared_ptr<dataNode> parse_assignment(list<token>& token_list) {
    if (_lookahead_any<2>(token_list, {tokenKey::Let, tokenKey::Now})) {
        return parse_explicit_assignment(token_list);
    } else if (_lookahead(token_list, tokenKey::Var)) {
        return parse_implicit_assignment(token_list);
    }

    throw UnexpectedInputError("expected an inline operation after \'" + display_token(token_list.front(), tokenDispOption::Literal) + "\'", false);
}

// Parse the assignment of a variable with an assignment keyword.
//      token_list: linked list of remaining tokens (input)
shared_ptr<dataNode> parse_explicit_assignment(list<token>& token_list) {
    if (_lookahead(token_list, tokenKey::Let)) {
        return parse_let_statement(token_list);
    } else {
        return parse_now_statement(token_list);
    }
}

// Parse a let assignment statement.
//      token_list: linked list of remaining tokens (input)
shared_ptr<dataNode> parse_let_statement(list<token>& token_list) {
    shared_ptr<valueData> expression;
    token variable_token;

//  Bypass tokens and store the variable.
    _match_bypass(token_list, tokenKey::Let);
    _query_bypass(token_list, tokenKey::Var, variable_token);
    _match_bypass(token_list, tokenKey::Bind);

//  Parse the expression after '='.
    expression = parse_expression(token_list);

    return make_shared<assignOp>(get<string>(variable_token[1]), expression);
}

// Parse a now reassignment statement.
//      token_list: linked list of remaining tokens (input)
shared_ptr<dataNode> parse_now_statement(list<token>& token_list) {
    shared_ptr<valueData> expression;
    token variable_token;

//  Bypass tokens and store the variable.
    _match_bypass(token_list, tokenKey::Now);
    _query_bypass(token_list, tokenKey::Var, variable_token);
    _match_bypass(token_list, tokenKey::Bind);

//  Parse the expression following '='.
    expression = parse_expression(token_list);

//  Parameter boolean is false since this is an explicit ('now') reassignment.
    return make_shared<reassignOp>(get<string>(variable_token[1]), expression, false);
}

// Parse an implicit reassignment statement.
//      token_list: linked list of remaining tokens (input)
shared_ptr<dataNode> parse_implicit_assignment(list<token>& token_list) {
    shared_ptr<valueData> expression;
    token variable_token;

//  Bypass tokens and store the variable.
    _query_bypass(token_list, tokenKey::Var, variable_token);
    _match_bypass(token_list, tokenKey::Bind);

//  Parse the expression after '='.
    expression = parse_expression(token_list);

//  Parameter boolean is true since this is an implicit reassignment.
    return make_shared<reassignOp>(get<string>(variable_token[1]), expression, true);
}

// Parse an expression, expressions evaluate to a value.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_expression(list<token>& token_list) {
    return parse_inline_if_expression(token_list);
}

// Parse a an inline ternary if statement.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_inline_if_expression(list<token>& token_list) {
    shared_ptr<valueData> comparison_expression1;

    comparison_expression1 = parse_comparison_expr(token_list);

//  Check for a ternary if statement.
    if (_lookahead(token_list, tokenKey::If)) {
        shared_ptr<valueData> comparison_expression2, expression;

//      Bypass tokens and expressions in the ternary if.
        _match_bypass(token_list, tokenKey::If);
        comparison_expression2 = parse_comparison_expr(token_list);
        _match_bypass(token_list, tokenKey::Else);
        expression = parse_expression(token_list);

        return make_shared<ternaryOp>(tokenKey::If, comparison_expression1, comparison_expression2, expression);
    }

    return comparison_expression1;
}

// The following five functions establish boolean order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse an expression potentially comparing values with a comparison operator.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_comparison_expr(list<token>& token_list) {
    shared_ptr<valueData> or_expr;
    constexpr size_t comparison_operator_count = 2;
    array<tokenKey, comparison_operator_count> comparison_operators;

//  Parse and store the expression before the comparison operator.
    or_expr = parse_or_expression(token_list);

    comparison_operators = {tokenKey::Equals, tokenKey::Is};
    if (_lookahead_any<comparison_operator_count>(token_list, comparison_operators)) {
        shared_ptr<valueData> comparison_expression;
        token operator_token;

//      Bypass and store the comparison operator.
        _query_bypass_any<comparison_operator_count>(token_list, comparison_operators, operator_token);

//      Parse and store the expression after the comparison operator.
        comparison_expression = parse_comparison_expr(token_list);

        return make_shared<binaryOp>(get<tokenKey>(operator_token[0]), or_expr, comparison_expression);
    }

    return or_expr;
}

// Parse a boolean expression potentially containing '|' or 'or'.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_or_expression(list<token>& token_list) {
    shared_ptr<valueData> xor_expression;
    constexpr size_t or_count = 2;
    array<tokenKey, or_count> or_operators;

//  Parse and store the expression before '|' or 'or'.
    xor_expression = parse_exclusive_or_expression(token_list);

    or_operators = {tokenKey::Or, tokenKey::OrW};
    if (_lookahead_any<or_count>(token_list, or_operators)) {
        shared_ptr<valueData> or_expression;
        token operator_token;

//      Bypass and store the OR operator.
        _query_bypass_any<or_count>(token_list, or_operators, operator_token);

//      Parse and store the expression after '|' or 'or'.
        or_expression = parse_or_expression(token_list);

        return make_shared<binaryOp>(get<tokenKey>(operator_token[0]), xor_expression, or_expression);
    }

    return xor_expression;
}

// Parse a boolean expression potentially containing '||' or 'xor'.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_exclusive_or_expression(list<token>& token_list) {
    shared_ptr<valueData> and_expression;
    constexpr size_t xor_count = 2;
    array<tokenKey, xor_count> xor_operators;

//  Parse and store the expression before '||' or 'xor'.
    and_expression = parse_and_expression(token_list);

    xor_operators = {tokenKey::Xor, tokenKey::XorW};
    if (_lookahead_any<xor_count>(token_list, xor_operators)) {
        shared_ptr<valueData> or_expression;
        token operator_token;

//      Bypass and store the XOR operator.
        _query_bypass_any<xor_count>(token_list, xor_operators, operator_token);

//      Parse and store the expression after '||' or 'xor'.
        or_expression = parse_or_expression(token_list);

        return make_shared<binaryOp>(get<tokenKey>(operator_token[0]), and_expression, or_expression);
    }

    return and_expression;
}

// Parse a boolean expression potentially containing '&' or 'and'.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_and_expression(list<token>& token_list) {
    shared_ptr<valueData> not_expression;
    constexpr size_t and_count = 2;
    array<tokenKey, and_count> and_operators;

//  Parse and store the expression potentially containing '&' or 'and'.
    not_expression = parse_not_expression(token_list);

    and_operators = {tokenKey::And, tokenKey::AndW};
    if (_lookahead_any<and_count>(token_list, and_operators)) {
        shared_ptr<valueData> and_expression;
        token operator_token;

//      Bypass and store the AND operator.
        _query_bypass_any<and_count>(token_list, and_operators, operator_token);

//      Parse and store the expression after '&' or 'and'.
        and_expression = parse_and_expression(token_list);

        return make_shared<binaryOp>(get<tokenKey>(operator_token[0]), not_expression, and_expression);
    }

    return not_expression;
}

// Parse a boolean expression potentially containing '!' or 'not'.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_not_expression(list<token>& token_list) {
    constexpr size_t not_count = 2;
    array<tokenKey, not_count> not_operators = {tokenKey::Not, tokenKey::NotW};
    if (_lookahead_any<not_count>(token_list, not_operators)) {
        shared_ptr<valueData> not_expression;
        token operator_token;

//      Bypass and store the NOT operator.
        _query_bypass_any<not_count>(token_list, not_operators, operator_token);

//      Parse and store the expression after '!' or 'not'.
        not_expression = parse_not_expression(token_list);

        return make_shared<unaryOp>(get<tokenKey>(operator_token[0]), not_expression);
    }

    return parse_numerical_comp_expr(token_list);
}

// Parse an expression potentially comparing numeric values with a numeric comparison operator.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_numerical_comp_expr(list<token>& token_list) {
    shared_ptr<valueData> additive_expression;
    array<tokenKey, numeric_comp_op_count> numeric_comp_ops;

//  Parse and store the expression before the comparison operator.
    additive_expression = parse_additive_expression(token_list);

    numeric_comp_ops = {tokenKey::Greater, tokenKey::Less, tokenKey::GrEqual, tokenKey::LessEqual};
    if (_lookahead_any<numeric_comp_op_count>(token_list, numeric_comp_ops)) {
        shared_ptr<valueData> numeric_comp_expr;
        token operator_token;

//      Bypass and store the comparison operator.
        _query_bypass_any<numeric_comp_op_count>(token_list, numeric_comp_ops, operator_token);

//      Parse and store the expression after the comparison operator.
        numeric_comp_expr = parse_numerical_comp_expr(token_list);

        return make_shared<binaryOp>(get<tokenKey>(operator_token[0]), additive_expression, numeric_comp_expr);
    }

    return additive_expression;
}

// The following three functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

// Parse a mathematical expression potentially containing '+' or '-'.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_additive_expression(list<token>& token_list) {
    shared_ptr<valueData> multiplicative_expression;
    constexpr size_t additive_count = 2;
    array<tokenKey, additive_count> additive_operators;
    
//  Parse and store the expression before '+' or '-'.
    multiplicative_expression = parse_multiplicative_expression(token_list);

    additive_operators = {tokenKey::Plus, tokenKey::Minus};
    if (_lookahead_any<additive_count>(token_list, additive_operators)) {
        shared_ptr<valueData> additive_expression;
        token operator_token;

//      Bypass and store the additive operator.
        _query_bypass_any<additive_count>(token_list, additive_operators, operator_token);

//      Parse and store the expression after '+' or '-'.
        additive_expression = parse_additive_expression(token_list);

        return make_shared<binaryOp>(get<tokenKey>(operator_token[0]), multiplicative_expression, additive_expression);
    }
    
    return multiplicative_expression;
}

// Parse a mathematical expression potentially containing '*' or '/'.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_multiplicative_expression(list<token>& token_list) {
    shared_ptr<valueData> exponential_expression;
    constexpr size_t multiplicative_count = 2;
    array<tokenKey, multiplicative_count> multiplicative_operators;
    
//  Parse and store the expression before '*' or '/'.
    exponential_expression = parse_exponential_expression(token_list);

    multiplicative_operators = {tokenKey::Mult, tokenKey::Div};
    if (_lookahead_any<multiplicative_count>(token_list, multiplicative_operators)) {
        shared_ptr<valueData> multiplicative_expression;
        token operator_token;

//      Bypass and store the multiplicative operator.
        _query_bypass_any<multiplicative_count>(token_list, multiplicative_operators, operator_token);

//      Parse and store the expression after '*' or '/'.
        multiplicative_expression = parse_multiplicative_expression(token_list);

        return make_shared<binaryOp>(get<tokenKey>(operator_token[0]), exponential_expression, multiplicative_expression);
    }
    
    return exponential_expression;
}

// Parse a mathematical expression potentially containing '**' (exponents).
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_exponential_expression(list<token>& token_list) {
    shared_ptr<valueData> minus_identifier_expression;
    
//  Parse and store the expression before '**'.
    minus_identifier_expression = parse_minus_identifier_expression(token_list);

    if (_lookahead(token_list, tokenKey::Exp)) {
        shared_ptr<valueData> exponential_expression;

        _match_bypass(token_list, tokenKey::Exp);

//      Parse and store the expression after '**'.
        exponential_expression = parse_exponential_expression(token_list);

        return make_shared<binaryOp>(tokenKey::Exp, minus_identifier_expression, exponential_expression);
    }

    return minus_identifier_expression;
}

// Parse a mathematical expression potentially starting with a negation.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_minus_identifier_expression(list<token>& token_list) {
//  Check for a '-' attached to the expression.
//      e.g. '-(2+6)'
    if (_lookahead(token_list, tokenKey::Minus)) {
        shared_ptr<valueData> primitive_expression;

        _match_bypass(token_list, tokenKey::Minus);

//      Parse the expression that the '-' is attached to.
        primitive_expression = parse_primitive_expression(token_list);

//      Multiply the subsequent expression by -1 to simulate negation.
        return make_shared<binaryOp>(tokenKey::Mult, make_shared<intContainer>(-1), primitive_expression);
    }

    return parse_primitive_expression(token_list);
}

// Parse an expression with low-level values or encased expressions.
//      token_list: linked list of remaining tokens (input)
shared_ptr<valueData> parse_primitive_expression(list<token>& token_list) {
//  Check for different low-level values.
    if (_lookahead(token_list, tokenKey::Var)) {
        token variable_token;

//      Bypass and store the variable name.
        _query_bypass(token_list, tokenKey::Var, variable_token);

        return make_shared<varContainer>(get<string>(variable_token[1]));

    } else if (_lookahead_any<number_type_count>(token_list, number_tokens)) {
        return parse_number_expression(token_list);
    } else if (_lookahead(token_list, tokenKey::Bool)) {
        return parse_boolean_expression(token_list);
    }
    shared_ptr<valueData> expression;

    _match_bypass(token_list, tokenKey::LeftPar);

//  Parse and store the encased expression.
    expression = parse_expression(token_list);

    _match_bypass(token_list, tokenKey::RightPar);

    return expression;
}

// Parse an expression containing just a number token.
//      token_list: linked list of remaining tokens (input)
shared_ptr<irreducibleData> parse_number_expression(list<token>& token_list) {
    token number_token;
    tokenKey number_type;
    
//  Bypass and identify the type of number token.
    _query_bypass_any<number_type_count>(token_list, number_tokens, number_token);
    number_type = get<tokenKey>(number_token[0]);
    
    switch (number_type) {
        case tokenKey::Int: // currently, only integers are supported in Regal.
            return make_shared<intContainer>(get<int>(number_token[1]));
        default:
            throw FatalError("unrecognized number type in number expression");
    }
}

// Parse an expression containing just a boolean token.
//      token_list: linked list of remaining tokens (input)
shared_ptr<irreducibleData> parse_boolean_expression(list<token>& token_list) {
    token bool_token;

//  Bypass and store the boolean value.
    _query_bypass(token_list, tokenKey::Bool, bool_token);

    return make_shared<boolContainer>(get<int>(bool_token[1]) ? true : false);
}
