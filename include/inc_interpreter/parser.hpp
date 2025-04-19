/*

Function declarations for AST generation. 
The AST generator follows the CFG in the docs folder one to one.

*/

#ifndef PARSER_HPP
#define PARSER_HPP

#include <list>
#include <iterator>

#include "inc_interpreter/interp_utils.hpp"
#include "inc_internal/error_handling.hpp"
#include "inc_internal/display_utils.hpp"


                        /*              SCOPE ANALYSIS              */

/*
Construct an abstract syntax tree (AST) from a list of tokens.
The AST consists of nodes connected with pointers. Nodes are class instances
defined in CodeTree from interp_utils.hpp and edges are class variable pointers to other nodes.

This function assumes that the given token list is nonempty. Moreover, the first element must be a 
newline token that looks like (tokenKey::Newline, int32_t, line number), where the integer represents the 
amount of indent of the first line of code.
Every token list should end with a newline token that has the global minimum indent, otherwise this 
function will not successfully generate an AST.

Exit the program if the given token list has size 1 where that element is a newline token.

Throw an exception if 
    a token does not align with the ordering defined in the CFG,
    the token list ends before an ordered AST can be generated,
    a scope initalizer (like an 'if' statement) does not have at least one line of indented code,
        i.e. the first token after a scope initializer is not a newline with indent larger than the 
             current minimum indent, or
    an 'else' token is not matching indent to its corresponding 'if' token.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to generate an AST from (input)

Return a shared pointer to the root class instance of the AST.
*/
std::shared_ptr<CodeTree::dataNode> parse_file(std::list<TokenDef::token>& token_list);

/*
Parse a sequence of operations in the same scope recursively.

This function assumes that the given token list is nonempty. Moreover, the first element must be a 
newline token that looks like (tokenKey::Newline, int32_t, line number), where the integer represents the 
amount of indent of the first line of code.
Every token list should end with a newline token that has the global minimum indent, otherwise this 
function will not successfully return.

Throw an exception if 
    a token does not align with the ordering defined in the CFG,
    the token list ends before an ordered AST can be generated,
    a scope initalizer (like an 'if' statement) does not have at least one line of indented code,
        i.e. the first token after a scope initializer is not a newline with indent larger than the 
             current minimum indent, or
    an 'else' token is not matching indent to its corresponding 'if' token.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)
    min_index: current scope indentation level (input)

Return a shared pointer to the first operation of the current scope.
*/
std::shared_ptr<CodeTree::dataNode> parse_code_scope(std::list<TokenDef::token>& token_list, const std::int32_t min_indent);


                        /*              SCOPE INITIALIZING OPERATIONS              */

/*
Parse an If-Else block of code.

This function assumes that the given token list is nonempty. Moreover, the first element must be an If 
token.

Throw an exception if 
    a token does not align with the ordering defined in the CFG,
    the token list ends before an ordered AST can be generated,
    the 'if' statement does not have at least one line of indented code,
    a later scope initalizer (like an 'if' statement) does not have at least one line of indented code,
        i.e. the first token after a scope initializer is not a newline with indent larger than the 
             current minimum indent, or
    an 'else' token is not matching indent to its corresponding 'if' token.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)
    min_index: current scope indentation level (input)

Return a shared pointer to the 'if' block.
*/
std::shared_ptr<CodeTree::dataNode> parse_if_block(std::list<TokenDef::token>& token_list, const std::int32_t min_indent);

/*
Parse an 'else' block of code.

This function assumes that the given token list is nonempty. Moreover, the first two elements
must be a newline token followed by an 'else' token.

Throw an exception if 
    a token does not align with the ordering defined in the CFG,
    the token list ends before an ordered AST can be generated,
    the 'else' statement does not have at least one line of indented code,
    a later scope initalizer (like an 'if' statement) does not have at least one line of indented code,
        i.e. the first token after a scope initializer is not a newline with indent larger than the 
             current minimum indent, or
    an 'else' token is not matching indent to its corresponding 'if' token.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)
    min_index: current scope indentation level (input)

Return a shared pointer to the 'else' block.
*/
std::shared_ptr<CodeTree::dataNode> parse_else_block(std::list<TokenDef::token>& token_list, const std::int32_t min_indent);


                        /*              INSCOPE OPERATIONS              */

/*
Parse any operation that does not introduce a new scope.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the inscope operation.
*/
std::shared_ptr<CodeTree::dataNode> parse_inscope_operation(std::list<TokenDef::token>& token_list);


                        /*              VARIABLES              */

/*
Parse a variable assignment operation.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the assignment operation.
*/
std::shared_ptr<CodeTree::dataNode> parse_assignment(std::list<TokenDef::token>& token_list);

/*
Parse an explicit variable assignment using 'let'.

This function assumes that the given token list is nonempty. Morevoer,
the first token must be the assignment keyword ('let').

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the explicit assignment operation.
*/
std::shared_ptr<CodeTree::dataNode> parse_explicit_assignment(std::list<TokenDef::token>& token_list);

/*
Parse an implicit variable assignment.

This function assumes that the given token list is nonempty. Moreover,
the first token must be a variable token that looks like (tokenKey::Var, "var_name", line_num).

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the implicit assignment operation.
*/
std::shared_ptr<CodeTree::dataNode> parse_implicit_assignment(std::list<TokenDef::token>& token_list);


                        /*              EXPRESSIONS              */
                    
/*
Parse an expression.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the expression.
*/
std::shared_ptr<CodeTree::valueData> parse_expression(std::list<TokenDef::token>& token_list);

/*
Parse a ternary 'if' expression.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the ternary 'if' expression.
*/
std::shared_ptr<CodeTree::valueData> parse_ternary_if_expression(std::list<TokenDef::token>& token_list);


                        /*              BOOLEAN ARITHMETIC              */

/*
Parse an expression that equates two values.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the equative expression.
*/
std::shared_ptr<CodeTree::valueData> parse_equative_expr(std::list<TokenDef::token>& token_list);

/*
Parse an expression that performs a boolean OR.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the OR expression.
*/
std::shared_ptr<CodeTree::valueData> parse_or_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression that performs a boolean XOR.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the XOR expression.
*/
std::shared_ptr<CodeTree::valueData> parse_exclusive_or_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression that performs a boolean AND.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the AND expression.
*/
std::shared_ptr<CodeTree::valueData> parse_and_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression that performs a boolean NOT.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the NOT expression.
*/
std::shared_ptr<CodeTree::valueData> parse_not_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression that compares two numbers.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the comparative expression.
*/
std::shared_ptr<CodeTree::valueData> parse_comparative_expr(std::list<TokenDef::token>& token_list);


                        /*              NUMERICAL ARITHMETIC              */

/*
Parse an expression that combines two numbers additively.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the additive expression.
*/
std::shared_ptr<CodeTree::valueData> parse_additive_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression that combines two numbers multiplicatively.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the multiplicative expression.
*/
std::shared_ptr<CodeTree::valueData> parse_multiplicative_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression that exponentiates two numbers.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the exponential expression.
*/
std::shared_ptr<CodeTree::valueData> parse_exponential_expression(std::list<TokenDef::token>& token_list);


                        /*              LOW-LEVEL VALUES              */

/*
Parse a numeric expression preceded with a '-'.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the negated expression.
*/
std::shared_ptr<CodeTree::valueData> parse_minus_identifier_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression containing a primitive value or variable.

This function assumes that the given token list is nonempty.

Throw an exception if 
    a token does not align with the ordering defined in the CFG or
    the token list ends before an inscope operation can be parsed.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the primitive expression.
*/
std::shared_ptr<CodeTree::valueData> parse_primitive_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression containing a single number.

This function assumes that the given token list is nonempty. Morevoer,
the first element must be a number token that looks like {number key, number value}.

Throw an exception if the next token is not a number token.
Throw a fatal error if a number token is not recognized (is not implemented).

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the number expression.
*/
std::shared_ptr<CodeTree::irreducibleData> parse_number_expression(std::list<TokenDef::token>& token_list);

/*
Parse an expression containing a single boolean.

This function assumes that the given token list is nonempty.

Throw an exception if the next token is not a bool token.

Parameters:
    token_list: list of tokens to parse (input)

Return a shared pointer to the boolean expression.
*/
std::shared_ptr<CodeTree::irreducibleData> parse_boolean_expression(std::list<TokenDef::token>& token_list);

#endif