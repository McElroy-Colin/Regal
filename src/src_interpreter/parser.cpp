/*

Functions and helpers for generating an abstract syntax tree from a token list.
The AST generator follows the CFG in the docs folder one to one.

*/

#include "inc_interpreter/parser.hpp"

// Standard library aliases
using std::string, std::list, std::array, std::shared_ptr, std::uint8_t, std::int32_t, std::int64_t, 
      std::make_shared, std::get, std::advance, std::move, std::next;

// interp_utils namespaces
using namespace TypingUtils;
using namespace TokenDef;
using namespace CodeTree;


// Strictly in-file helper functions and structures for the AST generator.
namespace {

/*
    Determine whether the next token in the given token list is of the given target token type.
    Optionally allow a newline token in between the target and the front of the list. 
    If a newline precedes the target and the given boolean is true, pop the newline off the list.

    Parameters:
        token_list: list to check the front of (input/output)
        target_token: token key to compare with the key in the front of the token list (input)
        allow_newline: true if the first element can be a newline while the second is the target token (default false) (input)

    Return true if the given token list's front token has the given target token key,
    or if the given boolean is true and the given token list has a newline followed by the given target token key.
*/
    inline const bool _lookahead(list<token>& token_list, const tokenKey target_token, const bool allow_newline = false) noexcept {
//      Default false on an empty list.
        if (token_list.empty()) {
            return false;
        }

//      Retrieve the token key from the token at the front of the list.
        const tokenKey front_token = get<0>(token_list.front());

//      If we allow a newline before the token, ensure that when the first element is a newline, the next is the target.
        if (allow_newline &&
            (token_list.size() > 1) &&
            (front_token == tokenKey::Newline) &&
//          Use an inline iterator to get the next element after the front of the list, then get the token key at the 0 index of this token.
            (get<0>((*next(token_list.begin()))) == target_token)) {

//          If the newline and target were found, pop the newline.
            token_list.pop_front();

            return true;
        }

//      Otherwise, return the comparison.
        return front_token == target_token;
    }

/*
    Determine whether the next token in the given token list is any of the given target token types.
    Optionally allow a newline token in between a target and the front of the list.
    If a newline precedes a target and the given boolean is true, pop the newline off the list.
    This function depends on a unsigned 8-bit integer template that determines the size of the given target array.

    Parameters:
        token_list: list to check the front of (input/output)
        target_tokens: array of token keys to comapre with the key in the front of the token list (input)
        allow_newline: true if the first element can be a newline while the second is any target token (default false) (input)

    Return true if the given token list's front token has any of the given target token keys,
    or if the given boolean is true and the given token list has a newline followed by any of the given target token keys.
*/
    template <uint8_t num>
    inline const bool _lookahead_any(list<token>& token_list, const array<tokenKey, num>& target_tokens, const bool allow_newline = false) noexcept {
//      Default false on an empty list.
        if (token_list.empty()) {
            return false;
        }

//      Retrieve the first token key.
        const tokenKey front_token = get<0>(token_list.front());

//      If we allow a newline before the token, ensure that when the first element is a newline, the next is the target.
        if (allow_newline &&
            (token_list.size() > 1) &&
            (front_token == tokenKey::Newline)) {
//          Use an inline iterator to get the next element after the front of the list, then get the token key at the 0 index of this token.
            const tokenKey second_token = get<0>((*next(token_list.begin())));
            
//          Compare each target against the token after the newline.
            for (tokenKey target : target_tokens) {
                if (second_token == target) {
//                  Pop the newline if the second token matches.
                    token_list.pop_front();

                    return true;
                }
            }

            return false;
        }
    
//      Compare against each target key.
        for (tokenKey target : target_tokens) {
            if (front_token == target) {

                return true;
            }
        }
        return false;
    }

/*
    Determine whether the token at the given index in the given token list is of the given target token type.

    Parameters:
        token_list: list to check from (input)
        target_token: token key to comapre with the key in the given index of the token list (input)
        index: index of the token list to check, 0 is the first element (input)

    Return true if the given token list has the target token key at the given index.
*/
    inline const bool _lookahead_many(list<token>& token_list, const tokenKey target_token, const uint32_t index) noexcept {
//      Iterate to the given index in the token list.
        list<token>::iterator iter = token_list.begin();
        advance(iter, index);

//      Default to false if the given index was larger than the token list's size.
        if (iter == token_list.end()) {
            return false;
        }

//      Check the key at the given index against the target key.
        return (get<0>((*iter)) == target_token);
    }

/*
    Retrieve the 32-bit integer in a newline token at the head of the given token list.

    This function assumes that if the token has a newline token key, 
    it has a signed 32-bit integer in the 1 index.

    Throw an exception if the given token list does not have a newline token as the first element.

    Parameters:
        token_list: list of tokens to retrieve the indent from the first element (input)

    Return the indent value.
*/
    inline const int32_t _query_indent(list<token>& token_list) {
//      If there is a newline token, get its indent number.
        if (_lookahead(token_list, tokenKey::Newline)) {
            return get<int32_t>(get<1>(token_list.front()));
        }
        
//      Handle no newline. Adjust the error message depending on if there are more tokens.
        if (token_list.empty()) {
            throw UnexpectedInputError(tokenKey::Newline, true);
        }

        const token& front = token_list.front();
        throw UnexpectedInputError(front, tokenKey::Newline, true, get<2>(front));
    }

/*
    Extract a reference to the first element of the given list of tokens if it matches the given target key.

    Throw an exception if the token list does not start with the given target key.

    Parameters:
        token_list: list of tokens to retrieve the front of (input)

    Return a reference to the front of the token list.
*/
    inline const token& _query(list<token>& token_list, const tokenKey target_token) {
        if (_lookahead(token_list, target_token)) {
            return token_list.front();
        }

//      Adjust the error message for when the token list is empty/only has a newline left.
        if (token_list.empty() || ((token_list.size() == 1) && _lookahead(token_list, tokenKey::Newline))) {
            throw UnexpectedInputError(target_token, false);
        }

        const token& front = token_list.front();
        throw UnexpectedInputError(front, target_token, true, get<2>(front));
    }

/*
    Pop the first element of the given token list if it matches the given token.
    Optionally allow the first element to be a newline while the second matches the given token.
    In this case, pop both elements if they are found and the given boolean is true.

    Throw an exception if the first element does not match the given token.

    Parameters:
        token_list: list of tokens to match the first element of (input/output)
        target_token: token key to match with the front of the given list (input)
        allow_newline: true if the first element can be a newline while the second is the target token (default false) (input)
*/
    inline const void _match_bypass(list<token>& token_list, const tokenKey target_token, const bool allow_newline = false) {
//      Ensure the relevent element of the token list matches the target. Potentially prop a preceding newline.
        if (_lookahead(token_list, target_token, allow_newline)) {
            token_list.pop_front();

            return;
        }

//      Adjust the error message for when the token list is empty/only has a newline left.
        if (token_list.empty() || ((token_list.size() == 1) && _lookahead(token_list, tokenKey::Newline))) {
            throw UnexpectedInputError(target_token, false);
        }

        const token& front = token_list.front();
        throw UnexpectedInputError(front, target_token, true, get<2>(front));
    }

/*
    Pop and store the first element of the given token list.

    This function assumes that the given token list is nonempty.

    Parameters:
        token_list: list of tokens to match the first element of (input/output)
        retrieved_token: token object to store the first element of the token list (output)
*/
    inline const void _retrieve_bypass(list<token>& token_list, token& retrieved_token) noexcept {
        retrieved_token = move(token_list.front());
        token_list.pop_front();

        return;
    }

    /*
    Pop the first element of the given token list and extract its line number.

    This function assumes that the given token list is nonempty.

    Parameters:
        token_list: list of tokens to match the first element of (input/output)
    
    Return the line number of the popped token.
*/
    inline const uint32_t _linenum_bypass(list<token>& token_list) noexcept {
        const uint32_t line_number = get<2>(token_list.front());
        token_list.pop_front();

        return line_number;
    }

/*
    Pop and store the first element of the given token list if it matches the given token.
    Optionally allow the first element to be a newline while the second matches the given token.
    In this case, store the second element and pop both if they are found and the given boolean is true.

    Throw an exception if the first element does not match the given token.

    Parameters:
        token_list: list of tokens to match the first element of (input/output)
        target_token: token key to match with the front of the given list (input)
        matched_token: token object to store the first element of the token list (output)
        allow_newline: true if the first element can be a newline while the second is the target token (default false) (input)
*/
    inline const void _query_bypass(list<token>& token_list, const tokenKey target_token, token& matched_token, const bool allow_newline = false) {
        if (_lookahead(token_list, target_token, allow_newline)) {
//          Store the relevent element and pop it. Potentially prop a preceding newline.
            matched_token = move(token_list.front());
            token_list.pop_front();

            return;
        }

//      Adjust the error message for when the token list is empty/only has a newline left.
        if (token_list.empty() || ((token_list.size() == 1) && _lookahead(token_list, tokenKey::Newline))) {
            throw UnexpectedInputError(target_token, false);
        }

        const token& front = token_list.front();
        throw UnexpectedInputError(front, target_token, true, get<2>(front));
    }

}


/*
The AST generator consists of recursive functions that correspond to nonterminals in the Regal CFG
The CFG can be found in the ~/docs directory.
All functions return nodes corresponding to nodes of an AST. 
The node definitions can be found in CodeTree from interp_utils.hpp.
*/

shared_ptr<dataNode> parse_file(list<token>& token_list) {
//  Handle an empty input, the lexer adds a newline by default.
    if ((token_list.size() == 1) && (_lookahead(token_list, tokenKey::Newline))) {
        exit(EXIT_SUCCESS);
    }

//  Global indent sets the file baseline scope.
    return parse_code_scope(token_list, GLOBAL_INDENT);
}

shared_ptr<dataNode> parse_code_scope(list<token>& token_list, const int32_t min_indent) {
    shared_ptr<dataNode> current_operation;
    token newline_token;

    _query_bypass(token_list, tokenKey::Newline, newline_token);
    
//  Parse this scope's current operation.
    if (_lookahead(token_list, tokenKey::If)) {
//      An If-Else block instantiates a new scope, so pass the next indent as the new minimum.
        current_operation = parse_if_block(token_list, get<int32_t>(get<1>(newline_token)));
    } else {
        current_operation = parse_inscope_operation(token_list);
    }

//  Recursively exit the current scope if indent decreases.
    if (_query_indent(token_list) <= min_indent) {
        return current_operation;
    }

//  Recursively continue in the current scope until the indent decreases or the token list ends.
//  The global scope ends when the final newline token is queried and the indent reaches a minimum.
    const shared_ptr<dataNode> code_scope = parse_code_scope(token_list, min_indent);

//  Default the line number to 0 since a code scope only stores code.
    return make_shared<codeScope>(0, current_operation, code_scope);
}

shared_ptr<dataNode> parse_if_block(list<token>& token_list, const int32_t min_indent) {
//  Bypass 'if', store its line number, and parse the boolean condition.
    const uint32_t if_linenum = _linenum_bypass(token_list);
    const shared_ptr<valueData> expression = parse_expression(token_list);

//  Ensure that the next line is more indented than 'if'.
    if (_query_indent(token_list) <= min_indent) {
        throw IncorrectIndentError(tokenKey::If, if_linenum);
    }

//  Parse the code under the 'if' statement.
    const shared_ptr<dataNode> code_scope = parse_code_scope(token_list, min_indent);

//  Retrieve the indent after the 'if' statement's scope.
    const int32_t next_indent = _query_indent(token_list);

//  Check for an 'else' block.
    if (_lookahead_many(token_list, tokenKey::Else, 1) && (next_indent == min_indent)) {
        const shared_ptr<dataNode> else_block = parse_else_block(token_list, min_indent);
        return make_shared<ifBlock>(if_linenum, expression, code_scope, else_block);
    } 

//      If 'else' was of lower indent, assume it is part of a parent scope.
//          i.e. deal with 'else' further up in the recursion.
//      'else' could not be higher indent or it would have thrown an exception in the parse code scope function.

    return make_shared<ifBlock>(if_linenum, expression, code_scope);
}

shared_ptr<dataNode> parse_else_block(list<token>& token_list, const int32_t min_indent) {
//  Bypass the newline and 'else' tokens. The indent has already been checked to be correct.
    _match_bypass(token_list, tokenKey::Newline);
//  Note, this element was already checked to be 'else' in the 'if' block parsing function.
    token_list.pop_front();

//  Allow for 'else if' chains.
    if(_lookahead(token_list, tokenKey::If)) {
        return parse_if_block(token_list, min_indent);
    }

//  Ensure that a newline follows the 'if' scope and retrieve its token.
    const token& newline_token = _query(token_list, tokenKey::Newline);

//  Ensure that the next line is more indented than 'else'.
    if (get<int32_t>(get<1>(newline_token)) <= min_indent) {
        throw IncorrectIndentError(tokenKey::Else, get<2>(newline_token));
    }

    return parse_code_scope(token_list, min_indent);
}

shared_ptr<dataNode> parse_inscope_operation(list<token>& token_list) {
    return parse_assignment(token_list);
}

shared_ptr<dataNode> parse_assignment(list<token>& token_list) {
    if (_lookahead(token_list, tokenKey::Assign)) {
        return parse_explicit_assignment(token_list);
    } else if (_lookahead(token_list, tokenKey::Var)) {
        return parse_implicit_assignment(token_list);
    }

//  Token list is assumed to be nonempty since this function was called from parse code scope where the size is checked.
    const token& front = token_list.front();
    throw UnexpectedInputError("expected an operation instead of " + display_token(front, true), get<2>(front));
}

shared_ptr<dataNode> parse_explicit_assignment(list<token>& token_list) {
    token variable_token;

//  Bypass the assignment token, store the variable, and bypass the '=' token.
//  The assignment token was already checked to be this first element in the parse assignment function.
    token_list.pop_front();
    _query_bypass(token_list, tokenKey::Var, variable_token);
    _match_bypass(token_list, tokenKey::Bind);

//  Parse the expression to assign.
    const shared_ptr<valueData> expression = parse_expression(token_list);

//  Pass the line number, variable name, and expression for assignment.
    return make_shared<assignOp>(get<2>(variable_token), get<string>(get<1>(variable_token)), expression);
}

shared_ptr<dataNode> parse_implicit_assignment(list<token>& token_list) {
    token variable_token;

//  Bypass and store the variable, then bypass '='.
//  The first token was already checked to be a variable token in the parse assignment function.
    _retrieve_bypass(token_list, variable_token);
    _match_bypass(token_list, tokenKey::Bind);

//  Parse the expression to assign.
    const shared_ptr<valueData> expression = parse_expression(token_list);

//  Pass the line number, variable name, and expression for assignment.    
    return make_shared<reassignOp>(get<2>(variable_token), get<string>(get<1>(variable_token)), expression);
}


// Note, tokens in expressions can be separated by newlines, but they do not affect scope.
// So, lookahead and bypass functions will have a true boolean in the third parameter to indicate an
// optional newline before the target token. A lookahead with this boolean as true will pop the preceding
// newline token if found, so a subsequent bypass will not need this boolean to be true.

shared_ptr<valueData> parse_expression(list<token>& token_list) {
    return parse_ternary_if_expression(token_list);
}

shared_ptr<valueData> parse_ternary_if_expression(list<token>& token_list) {
//  Parse the first expression.
    const shared_ptr<valueData> equative_expression = parse_equative_expr(token_list);

//  Check for a ternary 'if' statement.
    if (_lookahead(token_list, tokenKey::If)) {
//      Parse tokens and expressions in the ternary 'if'.
//      'if' cannot have a newline before it, since it would then be indistinguishable from an 'if' block.
        const uint32_t if_linenum = _linenum_bypass(token_list);
        const shared_ptr<valueData> expression1 = parse_expression(token_list);
        _match_bypass(token_list, tokenKey::Else, true);
        const shared_ptr<valueData> expression2 = parse_expression(token_list);

        return make_shared<ternaryOp>(if_linenum, tokenKey::If, equative_expression, expression1, expression2);
    }

    return equative_expression;
}

// The following five functions establish boolean order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

shared_ptr<valueData> parse_equative_expr(list<token>& token_list) {
//  Parse and store the expression before the equative operator.
    const shared_ptr<valueData> or_expr = parse_or_expression(token_list);

    if (_lookahead_any<2>(token_list, {tokenKey::Equals, tokenKey::Is}, true)) {
        token operator_token;

//      Bypass and store the equative operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after the equative operator.
        const shared_ptr<valueData> equative_expression = parse_equative_expr(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<binaryOp>(get<2>(operator_token), get<0>(operator_token), or_expr, equative_expression);
    }

    return or_expr;
}

shared_ptr<valueData> parse_or_expression(list<token>& token_list) {
//  Parse and store the expression before OR.
    const shared_ptr<valueData> xor_expression = parse_exclusive_or_expression(token_list);

    if (_lookahead_any<2>(token_list, {tokenKey::Or, tokenKey::OrW}, true)) {
        token operator_token;

//      Bypass and store the OR operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after OR.
        const shared_ptr<valueData> or_expression = parse_or_expression(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<binaryOp>(get<2>(operator_token), get<0>(operator_token), xor_expression, or_expression);
    }

    return xor_expression;
}

shared_ptr<valueData> parse_exclusive_or_expression(list<token>& token_list) {
//  Parse and store the expression before XOR.
    const shared_ptr<valueData> and_expression = parse_and_expression(token_list);

    if (_lookahead_any<2>(token_list, {tokenKey::Xor, tokenKey::XorW}, true)) {
        token operator_token;

//      Bypass and store the XOR operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after XOR.
        const shared_ptr<valueData> or_expression = parse_or_expression(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<binaryOp>(get<2>(operator_token), get<0>(operator_token), and_expression, or_expression);
    }

    return and_expression;
}

shared_ptr<valueData> parse_and_expression(list<token>& token_list) {
//  Parse and store the expression potentially containing AND.
    const shared_ptr<valueData> not_expression = parse_not_expression(token_list);

    if (_lookahead_any<2>(token_list, {tokenKey::And, tokenKey::AndW}, true)) {
        token operator_token;

//      Bypass and store the AND operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after AND.
        const shared_ptr<valueData> and_expression = parse_and_expression(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<binaryOp>(get<2>(operator_token), get<0>(operator_token), not_expression, and_expression);
    }

    return not_expression;
}

shared_ptr<valueData> parse_not_expression(list<token>& token_list) {
//  NOT is unary, so check for an operator before parsing any expression.

    if (_lookahead_any<2>(token_list, {tokenKey::Not, tokenKey::NotW}, true)) {
        token operator_token;

//      Bypass and store the NOT operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after NOT.
        const shared_ptr<valueData> not_expression = parse_not_expression(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<unaryOp>(get<2>(operator_token), get<0>(operator_token), not_expression);
    }

    return parse_comparative_expr(token_list);
}

shared_ptr<valueData> parse_comparative_expr(list<token>& token_list) {
//  Parse and store the expression before the comparison operator.
    const shared_ptr<valueData> additive_expression = parse_additive_expression(token_list);

//  Comparative operators are defined in interp_utils.hpp.
    if (_lookahead_any<comparative_op_count>(token_list, comparative_ops, true)) {
        token operator_token;

//      Bypass and store the comparative operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after the comparative operator.
        const shared_ptr<valueData> numeric_comp_expr = parse_comparative_expr(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<binaryOp>(get<2>(operator_token), get<0>(operator_token), additive_expression, numeric_comp_expr);
    }

    return additive_expression;
}

// The following three functions establish mathematical order of operations in Regal. 
// These functions are coded one-to-one with the Regal CFG.

shared_ptr<valueData> parse_additive_expression(list<token>& token_list) {   
//  Parse and store the expression before the additive operator.
    const shared_ptr<valueData> multiplicative_expression = parse_multiplicative_expression(token_list);

    if (_lookahead_any<2>(token_list, {tokenKey::Plus, tokenKey::Minus}, true)) {
        token operator_token;

//      Bypass and store the additive operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after the additive operator.
        const shared_ptr<valueData> additive_expression = parse_additive_expression(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<binaryOp>(get<2>(operator_token), get<0>(operator_token), multiplicative_expression, additive_expression);
    }
    
    return multiplicative_expression;
}

shared_ptr<valueData> parse_multiplicative_expression(list<token>& token_list) {
//  Parse and store the expression before the multiplicative operator.
    const shared_ptr<valueData> exponential_expression = parse_exponential_expression(token_list);

    if (_lookahead_any<2>(token_list, {tokenKey::Mult, tokenKey::Div}, true)) {
        token operator_token;

//      Bypass and store the multiplicative operator.
        _retrieve_bypass(token_list, operator_token);

//      Parse and store the expression after the multiplicative operator.
        const shared_ptr<valueData> multiplicative_expression = parse_multiplicative_expression(token_list);

//      Pass the operator from the operator token to be executed.
        return make_shared<binaryOp>(get<2>(operator_token), get<0>(operator_token), exponential_expression, multiplicative_expression);
    }
    
    return exponential_expression;
}

shared_ptr<valueData> parse_exponential_expression(list<token>& token_list) {
//  Parse and store the expression before '**'.
    const shared_ptr<valueData> minus_identifier_expression = parse_minus_identifier_expression(token_list);

    if (_lookahead(token_list, tokenKey::Exp, true)) {
//      Bypass the '**' and store its line number.
        const uint32_t exp_linenum = _linenum_bypass(token_list);

//      Parse and store the expression after '**'.
        const shared_ptr<valueData> exponential_expression = parse_exponential_expression(token_list);

        return make_shared<binaryOp>(exp_linenum, tokenKey::Exp, minus_identifier_expression, exponential_expression);
    }

    return minus_identifier_expression;
}

shared_ptr<valueData> parse_minus_identifier_expression(list<token>& token_list) {
//  Check for a '-' attached to the expression.
//      e.g. '-(2+6)'
//  Do not allow a newline separating '-' and its expression.
    if (_lookahead(token_list, tokenKey::Minus)) {
        const uint32_t minus_linenum = _linenum_bypass(token_list);

//      Parse the expression that the '-' is attached to.
        const shared_ptr<valueData> primitive_expression = parse_primitive_expression(token_list);

//      Convert the expression to 0 - expr to simulate negation.
        return make_shared<binaryOp>(minus_linenum, tokenKey::Minus, make_shared<int32Container>(minus_linenum, 0), primitive_expression);
    }

    return parse_primitive_expression(token_list);
}

shared_ptr<valueData> parse_primitive_expression(list<token>& token_list) {
//  Check for different low-level values.
    if (_lookahead(token_list, tokenKey::Var, true)) {
        token variable_token;

//      Bypass and store the variable name.
        _retrieve_bypass(token_list, variable_token);

//      Pass the variable name as a string.
        return make_shared<varContainer>(get<2>(variable_token), get<string>(get<1>(variable_token)));

//  Number types/tokens are defined in interp_utils.hpp.
    } else if (_lookahead_any<number_type_count>(token_list, number_tokens, true)) {
        return parse_number_expression(token_list);
    } else if (_lookahead(token_list, tokenKey::Bool, true)) {
        return parse_boolean_expression(token_list);
    }

//  Otherwise, assume it is some expression encased in parenthesis.

    _match_bypass(token_list, tokenKey::LeftPar, true);

//  Parse and store the encased expression.
    const shared_ptr<valueData> expression = parse_expression(token_list);

    _match_bypass(token_list, tokenKey::RightPar, true);

    return expression;
}

shared_ptr<irreducibleData> parse_number_expression(list<token>& token_list) {
    token number_token;
    
//  Bypass and identify the type of number token.
//  This element was already checked to be a number token in the primitive expression parsing function.
    _retrieve_bypass(token_list, number_token);
    const tokenKey number_type = get<0>(number_token);
    const uint32_t number_linenum = get<2>(number_token);
    
//  Return the correct number type. Pass the number value to the relevant constructor.
//  Note, all numbers are treated as unsigned integers since negatives are parsed separately.
//  Numbers will be stored as signed when opimization occurs. Integer values are guaranteed to 
//  be within the signed range as this was checked during lexing.
    switch (number_type) {
        case tokenKey::Int32:
            return make_shared<int32Container>(number_linenum, get<uint32_t>(get<1>(number_token)));
        case tokenKey::Int64:
            return make_shared<int64Container>(number_linenum, get<uint64_t>(get<1>(number_token)));
        case tokenKey::Float32:
            return make_shared<float32Container>(number_linenum, get<float>(get<1>(number_token)));
        case tokenKey::Float64:
            return make_shared<float64Container>(number_linenum, get<double>(get<1>(number_token)));
        default:
            throw FatalError("unrecognized number type in number expression", number_linenum);
    }
}

shared_ptr<irreducibleData> parse_boolean_expression(list<token>& token_list) {
    token bool_token;

//  Bypass and store the boolean value.
//  This element was laready checked to be a boolean token in the primitive expression parsing function.
    _retrieve_bypass(token_list, bool_token);

//  Pass the boolean value to the constructor.
    return make_shared<boolContainer>(get<2>(bool_token), get<bool>(get<1>(bool_token)));
}
