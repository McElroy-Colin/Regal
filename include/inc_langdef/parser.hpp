// Header file containing dependencies and declarations for the Regal AST generator.

#ifndef PARSER_HPP
#define PARSER_HPP

#include <list>
#include <iterator>
#include "inc_langdef/langdef.hpp"
#include "inc_debug/error_handling.hpp"
#include "inc_debug/compiler_debug.hpp"


// Parsing function definitions coded with the CFG.

// SCOPE ANALYSIS
std::shared_ptr<CodeTree::dataNode> parse_file(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::dataNode> parse_code_scope(std::list<TokenDef::token>& token_list, const int min_indent);

// MULTI-LINE OPERATIONS
std::shared_ptr<CodeTree::dataNode> parse_if_block(std::list<TokenDef::token>& token_list, const int min_indent);
std::shared_ptr<CodeTree::dataNode> parse_else_block(std::list<TokenDef::token>& token_list, const int min_indent);

// SINGLE-LINE INITIALIZER
std::shared_ptr<CodeTree::dataNode> parse_inline_operation(std::list<TokenDef::token>& token_list);

// VARIABLE ASSIGNMENT:
std::shared_ptr<CodeTree::dataNode> parse_assignment(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::dataNode> parse_explicit_assignment(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::dataNode> parse_let_statement(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::dataNode> parse_now_statement(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::dataNode> parse_implicit_assignment(std::list<TokenDef::token>& token_list);

// EXPRESSIONS
std::shared_ptr<CodeTree::valueData> parse_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_inline_if_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_value_expression(std::list<TokenDef::token>& token_list);

// BOOLEAN ARITHMETIC:
std::shared_ptr<CodeTree::valueData> parse_comparison_expr(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_or_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_exclusive_or_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_and_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_not_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_numerical_comp_expr(std::list<TokenDef::token>& token_list);

// NUMERICAL ARITHMETIC:
std::shared_ptr<CodeTree::valueData> parse_additive_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_multiplicative_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_exponential_expression(std::list<TokenDef::token>& token_list);

// LOW-LEVEL VALUES:
std::shared_ptr<CodeTree::valueData> parse_minus_identifier_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::valueData> parse_primitive_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::irreducibleData> parse_number_expression(std::list<TokenDef::token>& token_list);
std::shared_ptr<CodeTree::irreducibleData> parse_boolean_expression(std::list<TokenDef::token>& token_list);

#endif