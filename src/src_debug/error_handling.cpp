// File containing custom error structures for debugging interpreter source code.

#include "inc_debug/compiler_debug.hpp"
#include "inc_debug/error_handling.hpp"

using std::string, std::runtime_error, std::shared_ptr, std::visit;
using namespace TokenDef;
using namespace CodeTree;
using namespace TypingUtils;
using namespace DebugUtils;


//  Default constructor.
FatalError::FatalError() : runtime_error("FatalError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
//      derived: true if the error is called from a derived error (input)
FatalError::FatalError(const string& error_msg, const bool derived) 
    : runtime_error(derived ? error_msg : "FatalError: " + error_msg) {}

//  Return the next token starting at the given line position from the given line.
//      line: text to extract token from (input)
//      line_pos: starting index of the target TokenDef::token in the line (input)
string UnrecognizedInputError::extract_input(const string& line, const int line_pos) {
    int prblm_token_index = line_pos;

//  Bypass whitespace.
    while (!((line[prblm_token_index] == ' ') || (line[prblm_token_index] == '\t'))) { 
        prblm_token_index++; 
    }

    return line.substr(line_pos, prblm_token_index - line_pos);
}
 
//  Default constructor.
UnrecognizedInputError::UnrecognizedInputError() : runtime_error("UnrecognizedInputError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
//      derived: true if the error is called from a derived error (input)
UnrecognizedInputError::UnrecognizedInputError(const string& error_msg, const bool derived) 
    : runtime_error(derived ? error_msg : "UnrecognizedInputError: " + error_msg) {}

//  Constructor taking a line and position to extract a problematic token.
//      line: text to extract token from (input)
//      line_pos: starting index of the target token in the line (input)
UnrecognizedInputError::UnrecognizedInputError(const string& line, const int line_pos) 
    : runtime_error("UnrecognizedInputError: \'" 
        + extract_input(line, line_pos) + "\' is not recognized as a valid symbol or token") {}


//  Default constructor.
UnexpectedInputError::UnexpectedInputError() : runtime_error("UnexpectedInputError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
//      derived: true if the error is called from a derived error (input)
UnexpectedInputError::UnexpectedInputError(const string& error_msg, const bool derived) 
    : runtime_error(derived ? error_msg : "UnexpectedInputError: " + error_msg) {}

//  Constructor to handle when user input ends before parsing recognizes a line.
//      expected_token: token key that was expected by the parser (input)
//      display_option: controls how to display the given token as in display_token (input)
UnexpectedInputError::UnexpectedInputError(const tokenKey expected_token, const tokenDispOption display_option)
    : runtime_error("UnexpectedInputError: expected \'" 
        + display_token({expected_token}, display_option) + "\' but input ended") {}

//  Constructor taking parameters to create an expected versus received error message.
//      given_token: token that was input and is incorrect in parsing (input) 
//      expected_token: token key that was expected by the parser (input)
//      display_option: controls how to display the given tokens as in display_token (input)
UnexpectedInputError::UnexpectedInputError(const token& given_token, const tokenKey expected_token, const tokenDispOption display_option)
    : runtime_error("UnexpectedInputError: expected \'" 
        + display_token({expected_token}, display_option) + "\' but received \'" 
        + display_token(given_token, display_option) + "\'") {}

//  Constructor to handle extra tokens after a parser has completed a line.
//      extra_token: first token after parsing completed (input)
//      display_option: controls how to display the given token as in display_token (input)
UnexpectedInputError::UnexpectedInputError(const token& extra_token, const tokenDispOption display_option) 
    : runtime_error("UnexpectedInputError: excess tokens starting at \'" 
        + display_token(extra_token, display_option) + "\'") {}


//  Default constructor.
IncorrectInputError::IncorrectInputError() : runtime_error("IncorrectInputError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
//      derived: true if the error is called from a derived error (input)
IncorrectInputError::IncorrectInputError(const string& error_msg, const bool derived) 
    : runtime_error(derived ? error_msg : "IncorrectInputError: " + error_msg) {}


//  Default constructor.
UnexpectedIndentError::UnexpectedIndentError() : IncorrectInputError("UnexpectedIndentError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
UnexpectedIndentError::UnexpectedIndentError(const string& error_msg) 
    : IncorrectInputError("UnexpectedIndentError: " + error_msg) {}

//      Constructor handling unindented code blocks.
//          op: operator that expects an indent (input)
UnexpectedIndentError::UnexpectedIndentError(const TokenDef::tokenKey op) 
        : IncorrectInputError(display_token({op}, tokenDispOption::Literal) + " statement expects an indented code block") {}

//      Constructor handling non-matching operator indents.
//          op1: operator initially used (input)
//          op2: operator that expects matching idnent with op1 (input)
UnexpectedIndentError::UnexpectedIndentError(const TokenDef::tokenKey op1, const TokenDef::tokenKey op2) 
        : IncorrectInputError("\'" + display_token({op2}, tokenDispOption::Literal) + "\' should have matching indent to \'" 
            + display_token({op1}, tokenDispOption::Literal) + "\'") {}

//  Default constructor.
VariableNotInitializedError::VariableNotInitializedError() : IncorrectInputError("VariableNotInitializedError") {}

//  Constructor to handle an uninitialized variable or display an error message.
//      variable: variable name or error message to display (input)
//      error_msg: true if variable argument is an error message, 
//                 false if variable argument is a variable name (input)
VariableNotInitializedError::VariableNotInitializedError(const string& variable, const bool error_msg) 
    : IncorrectInputError("VariableNotInitializedError: " + (error_msg 
        ? variable 
        : "variable \'" + variable + "\' not initialized")) {}


//  Default constructor.
VariablePreInitializedError::VariablePreInitializedError() : IncorrectInputError("VariablePreInitializedError") {}

//  Constructor to handle a duplicate initialized variable or display an error message.
//      variable: variable name or error message to display (input)
//      error_msg: true if variable argument is an error message, 
//      false if variable argument is a variable name (input)
VariablePreInitializedError::VariablePreInitializedError(const string& variable, const bool error_msg) 
    : IncorrectInputError("VariablePreInitializedError: " + (error_msg 
        ? variable 
        : "variable \'" + variable + "\' already initialized")) {}


//  Default constructor.
DivisionByZeroError::DivisionByZeroError() : IncorrectInputError("DivisionByZeroError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
//      derived: true if the error is called from a derived error (input)
DivisionByZeroError::DivisionByZeroError(const string& error_msg, const bool derived) 
    : IncorrectInputError(derived ? error_msg : "DivisionByZeroError: " + error_msg) {}


//  Default constructor.
TypeMismatchError::TypeMismatchError() : IncorrectInputError("TypeMismatchError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
//      derived: true if the error is called from a derived error (default true) (input)
TypeMismatchError::TypeMismatchError(const string& error_msg, const bool derived) 
    : IncorrectInputError(derived ? error_msg : "TypeMismatchError: " + error_msg) {}

//  Constructor for a binary operator receiving mismatched types.
//      op: operator being used (input)
//      type1: type of first expression (input)
//      type2: type of second expresion (input)
TypeMismatchError::TypeMismatchError(const tokenKey op, const dataType type1, const dataType type2)
    : IncorrectInputError("TypeMismatchError: \'" + display_token({op}, tokenDispOption::Literal) + "\' operator expected matching types but recieved types \'" 
        + display_type(type1) + "\' and \'" + display_type(type2) + "\'" ) {}

//  Default constructor.
InavlidOperatorError::InavlidOperatorError() : TypeMismatchError("InvalidOperatorError") {}

//  Constructor taking a message.
//      error_msg: error message to display (input)
InavlidOperatorError::InavlidOperatorError(const string& error_msg) 
    : TypeMismatchError("InvalidOperatorError: " + error_msg) {}

//  Constructor to handle invalid applications of an operator to data.
//      op: operator being used incorrectly (input)
//      type: type of data being applied to (input)
InavlidOperatorError::InavlidOperatorError(const TokenDef::tokenKey op, const TypingUtils::dataType type) 
        : TypeMismatchError("\'" + display_token({op}, tokenDispOption::Literal) + "\' operator cannot be applied to type " + display_type(type)) {}

//      Constructor for a unary operator receiving the incorrect type.
//          op: operator being used (input)
//          expected_type: expected type for operator (input)
//          received_type: type used with operator (input)
InavlidOperatorError::InavlidOperatorError(const TokenDef::tokenKey op, const TypingUtils::dataType expected_type, const TypingUtils::dataType received_type)
        : TypeMismatchError("unary operator \'" + display_token({op}, tokenDispOption::Literal) + "\' expected an expression of type "
            + display_type(expected_type) + " but received type " + display_type(received_type)) {}


//  Default constructor.
ConditionMismatchError::ConditionMismatchError() : TypeMismatchError("ConditionMismatchError") {}

//  Constructor handling a non-boolean if condition.
//      received_type: non-boolean type in if condition (input)
//      ternary: true if the if statement is a ternary expression (input)
ConditionMismatchError::ConditionMismatchError(const dataType received_type, const bool ternary) 
    : TypeMismatchError(ternary 
        ? "ternary if expression expects a condition of type " + display_type(dataType::BoolT) + " but received type " + display_type(received_type)
        : "if statement expects a condition of type " + display_type(dataType::BoolT) + " but received type " + display_type(received_type)) {}


//  Default constructor.
ImplicitMismatchError::ImplicitMismatchError() : TypeMismatchError("ImplicitMismatchError") {}

//  Constructor to handle an implicit reassignment type mismatch or display an error message.
//      variable: variable name or error message to display (input)
//      error_msg: true if variable argument is an error message, 
//                 false if variable argument is a variable name (input)
ImplicitMismatchError::ImplicitMismatchError(const string& variable, const dataType expected_type, const dataType received_type) 
    : TypeMismatchError("TypeMismatchError: implicit reassignment of variable \'" + variable + "\' expected type " 
        + display_type(expected_type) + " but received type " + display_type(received_type)) {}


//  Default constructor.
ExplicitMismatchError::ExplicitMismatchError() : TypeMismatchError("ExplicitMismatchError") {}

//  Constructor to handle an explicit reassignment type mismatch or display an error message.
//      variable: variable name or error message to display (input)
//      error_msg: true if variable argument is an error message, 
//                 false if variable argument is a variable name (input)
ExplicitMismatchError::ExplicitMismatchError(const string& variable, const bool error_msg) 
    : TypeMismatchError(error_msg 
        ? variable 
        : "ExplicitMismatchError: explicit reassignment of variable \'" 
            + variable + "\' must be of a different type.") {}

//  Constructor to handle an explicit reassignment type mismatch or display an error message.
//      variable: variable name or error message to display (input)

ExplicitMismatchError::ExplicitMismatchError(const string& variable, const dataType duplicate) 
    : TypeMismatchError("ExplicitMismatchError: explicit reassignment of variable \'" + variable 
        + "\' expected a type different from "+ display_type(duplicate)) {}