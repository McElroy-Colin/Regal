/*

Interpretation error class implementations.

*/

#include "inc_internal/display_utils.hpp"
#include "inc_internal/error_handling.hpp"

// Standard library aliases
using std::string, std::runtime_error, std::shared_ptr, std::int32_t, std::uint32_t, std::make_tuple, std::visit, std::to_string;

// interp_utils namespaces
using namespace TypingUtils;
using namespace TokenDef;
using namespace CodeTree;


        /*              FatalError implementation              */

FatalError::FatalError(const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + "FatalError") {}

FatalError::FatalError(const string& error_msg, const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + error_msg) {}


        /*              UnrecognizedInputError implementation              */

const string UnrecognizedInputError::extract_input(const string& input, const uint32_t start_index) noexcept {
    const uint32_t line_size = input.size();
    uint32_t prblm_token_index = start_index;

//  Bypass whitespace.
    while ((prblm_token_index < line_size) && !(
                (input[prblm_token_index] == ' ') || 
                (input[prblm_token_index] == '\t') || 
                (input[prblm_token_index] == NEWLINE_TOKEN)
            )) { prblm_token_index++; }

    return input.substr(start_index, prblm_token_index - start_index);
}
 
UnrecognizedInputError::UnrecognizedInputError(const uint32_t line_number) 
    : runtime_error("line " + to_string(line_number) + ": UnrecognizedInputError") {}

UnrecognizedInputError::UnrecognizedInputError(const string& error_msg, const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + error_msg) {}

UnrecognizedInputError::UnrecognizedInputError(const string& input, const uint32_t start_index, const uint32_t line_number) 
    : UnrecognizedInputError("\'" + extract_input(input, start_index) + "\' is not recognized as a valid symbol or token", line_number) {}


        /*              UnexpectedInputError implementation              */

UnexpectedInputError::UnexpectedInputError(const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + "UnexpectedInputError") {}

UnexpectedInputError::UnexpectedInputError(const string& error_msg, const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + error_msg) {}

UnexpectedInputError::UnexpectedInputError(const tokenKey expected_token, const bool literal)
    : runtime_error("expected " + (expected_token == tokenKey::LeftPar
                        ? "an expression"
                        : display_token(make_tuple(expected_token, false, 0), false))
                    + " but input ended") {}

UnexpectedInputError::UnexpectedInputError(const token& given_token, const tokenKey expected_token, const bool literal, const uint32_t line_number)
    : UnexpectedInputError("expected " + (expected_token == tokenKey::LeftPar 
                               ? "an expression" 
                               : display_token(make_tuple(expected_token, false, line_number), false)) 
                           + " but received " + display_token(given_token, literal), line_number) {}


        /*              IncorrectInputError implementation              */

IncorrectInputError::IncorrectInputError(const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + "IncorrectInputError") {}


IncorrectInputError::IncorrectInputError(const string& error_msg, const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + error_msg) {}


            /*              UnexpectedIndentError implementation              */

IncorrectIndentError::IncorrectIndentError(const uint32_t line_number) 
    : IncorrectInputError("UnexpectedIndentError", line_number) {}

IncorrectIndentError::IncorrectIndentError(const string& error_msg, const uint32_t line_number) 
    : IncorrectInputError(error_msg, line_number) {}

IncorrectIndentError::IncorrectIndentError(const TokenDef::tokenKey op, const uint32_t line_number) 
    : IncorrectInputError(display_token(make_tuple(op, false, line_number), true) + " statement expects an indented code block", line_number) {}


            /*              VariableInitializationError implementation              */

VariableInitializationError::VariableInitializationError(const uint32_t line_number) 
    : IncorrectInputError("VariableInitializationError", line_number) {}

VariableInitializationError::VariableInitializationError(const string& variable, const bool not_initalized, const uint32_t line_number)
    : IncorrectInputError("variable \'" + variable + "\' " + (not_initalized ? "not" : "already") + " initialized", line_number) {}


            /*              TypeMismatchError implementation              */

TypeMismatchError::TypeMismatchError(const uint32_t line_number) 
    : IncorrectInputError("TypeMismatchError", line_number) {}

TypeMismatchError::TypeMismatchError(const string& error_msg, const uint32_t line_number) 
    : IncorrectInputError(error_msg, line_number) {}

TypeMismatchError::TypeMismatchError(const tokenKey op, const bool literal, const dataType type1, const dataType type2, const bool unary, const uint32_t line_number)
    : IncorrectInputError(display_token(make_tuple(op, false, line_number), literal) + " operator expected " + (unary
                              ? "type " + display_type(type2, line_number)
                              : "combinable types") 
                          + " but received " + (unary
                              ? "type " + display_type(type1, line_number)
                              : "types " + display_type(type1, line_number) + " and " + display_type(type2, line_number)), line_number) {}


            /*              ExecutionError implementation              */

ExecutionError::ExecutionError(const uint32_t line_number) : runtime_error("line " + to_string(line_number) + ": ExecutionError") {}

ExecutionError::ExecutionError(const string& error_msg, const uint32_t line_number) 
    : runtime_error(_line_prefix(line_number) + error_msg) {}


            /*              OverflowError implementation              */

OverflowError::OverflowError(const uint32_t line_number) 
    : ExecutionError("OverflowError", line_number) {}

OverflowError::OverflowError(const string& error_msg, const uint32_t line_number) 
    : ExecutionError(error_msg, line_number) {}
