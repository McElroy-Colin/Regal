// Header file containing custom error structures for debugging interpreter source code.

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <stdexcept>
#include "inc_langdef/langdef.hpp"


// Error class representing severe logic mistakes in interpreter code. These should never throw.
class FatalError : public std::runtime_error {
    public: 
//      Default constructor.
        FatalError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (default true) (input)
        FatalError(const std::string& error_msg, const bool derived = true);
};

// Error class representing the use of an undefined TokenDef::token.
class UnrecognizedInputError : public std::runtime_error {
    private:
//      Return the next token starting at the given line position from the given line.
//          line: text to extract token from (input)
//          line_pos: starting index of the target TokenDef::token in the line (input)
        std::string extract_input(const std::string& line, const int line_pos);

    public: 
//      Default constructor.
        UnrecognizedInputError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (default true) (input)
        UnrecognizedInputError(const std::string& error_msg, const bool derived = true);

//      Constructor taking a line and position to extract a problematic TokenDef::token.
//          line: text to extract token from (input)
//          line_pos: starting index of the target TokenDef::token in the line (input)
        UnrecognizedInputError(const std::string& line, const int line_pos);
};

// Error class representing incorrect ordering of tokens.
class UnexpectedInputError : public std::runtime_error {
    private:
        std::string received, expected;

    public:
//      Default constructor.
        UnexpectedInputError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (default true) (input)
        UnexpectedInputError(const std::string& error_msg, const bool derived = true);

//      Constructor to handle when user input ends before parsing recognizes a line.
//          expected_token: token key that was expected by the parser (input)
//          display_option: controls how to display the given TokenDef::token as in DebugUtils::display_token (input)
        UnexpectedInputError(const TokenDef::tokenKey expected_token, const DebugUtils::tokenDispOption display_option);

//      Constructor taking parameters to create an expected versus received error message.
//          given_token: token that was input and is incorrect in parsing (input) 
//          expected_token: token key that was expected by the parser (input)
//          display_option: controls how to display the given tokens as in DebugUtils::display_token (input)
        UnexpectedInputError(const TokenDef::token& given_token, const TokenDef::tokenKey expected_token, const DebugUtils::tokenDispOption display_option);

//      Constructor to handle extra tokens after a parser has completed a line.
//          extra_token: first token after parsing completed (input)
//          display_option: controls how to display the given TokenDef::token as in DebugUtils::display_token (input)
        UnexpectedInputError(const TokenDef::token& extra_token, const DebugUtils::tokenDispOption display_option);
};

// Error class representing invalid input logic.
class IncorrectInputError : public std::runtime_error {
    public:
//      Default constructor.
        IncorrectInputError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (default true) (input)
        IncorrectInputError(const std::string& error_msg, const bool derived = true);
};

// Error class representing a missing code block.
class UnexpectedIndentError : public IncorrectInputError {
    public:
//      Default constructor.
        UnexpectedIndentError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
        UnexpectedIndentError(const std::string& error_msg);

//      Constructor handling unindented code blocks.
//          op: operator that expects an indent (input)
        UnexpectedIndentError(const TokenDef::tokenKey op);

//      Constructor handling non-matching operator indents.
//          op1: operator initially used (input)
//          op2: operator that expects matching idnent with op1 (input)
        UnexpectedIndentError(const TokenDef::tokenKey op1, const TokenDef::tokenKey op2);
};

// Error class representing uninitialized variable errors.
class VariableNotInitializedError : public IncorrectInputError {
    public: 
//      Default constructor.
        VariableNotInitializedError();

//      Constructor to handle an uninitialized variable or display an error message.
//          variable: variable name or error message to display (input)
//          error_msg: true if variable argument is an error message, 
//                     false if variable argument is a variable name (default false) (input)
        VariableNotInitializedError(const std::string& variable, const bool error_msg = false);
};

// Error class representing duplicate initialized variables.
class VariablePreInitializedError : public IncorrectInputError {
    public:
//      Default constructor.
        VariablePreInitializedError();

//      Constructor to handle a duplicate initialized variable or display an error message.
//          variable: variable name or error message to display (input)
//          error_msg: true if variable argument is an error message, 
//                     false if variable argument is a variable name (default false) (input)
        VariablePreInitializedError(const std::string& variable, const bool error_msg = false);
};

// Error class representing division by 0.
class DivisionByZeroError : public IncorrectInputError {
    public:
//      Default constructor.
        DivisionByZeroError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (default true) (input)
        DivisionByZeroError(const std::string& error_msg, const bool derived = true);
};

// Error class representing mismatching type errors.
class TypeMismatchError : public IncorrectInputError {
    private:
        std::string operator_str;
    public:
//      Default constructor.
        TypeMismatchError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (default true) (input)
        TypeMismatchError(const std::string& error_msg, const bool derived = true);

//      Constructor for a binary operator receiving mismatched types.
//          op: operator being used (input)
//          type1: type of first expression (input)
//          type2: type of second expresion (input)
        TypeMismatchError(const TokenDef::tokenKey op, const TypingUtils::dataType type1, const TypingUtils::dataType type2);

};

// Error class representing invalid operator uses.
class InavlidOperatorError : public TypeMismatchError{
    public:
//      Default constructor.
        InavlidOperatorError();

//      Constructor taking a message.
//          error_msg: error message to display (input)
        InavlidOperatorError(const std::string& error_msg);

//      Constructor to handle invalid applications of an operator to data.
//          op: operator being used incorrectly (input)
//          type: type of data being applied to (input)
        InavlidOperatorError(const TokenDef::tokenKey op, const TypingUtils::dataType type);

//      Constructor for a unary operator receiving the incorrect type.
//          op: operator being used (input)
//          expected_type: expected type for operator (input)
//          received_type: type used with operator (input)
        InavlidOperatorError(const TokenDef::tokenKey op, const TypingUtils::dataType expected_type, const TypingUtils::dataType received_type);

//      Constructor to handle an operator taking invalid data. 
//          op: operator token key being used incorrectly (input)
        InavlidOperatorError(const TokenDef::tokenKey op);
};

// Error class representing boolean condition typing errors.
class ConditionMismatchError: public TypeMismatchError {
    public:
//      Default constructor.
        ConditionMismatchError();

//      Constructor handling a non-boolean if condition.
//          received_type: non-boolean type in if condition (input)
//          ternary: true if the if statement is a ternary expression (input)
        ConditionMismatchError(const TypingUtils::dataType received_type, const bool ternary);
};

// Error class representing an implicit reassignment type mismatch error.
class ImplicitMismatchError : public TypeMismatchError {
    public: 
//      Default constructor.
        ImplicitMismatchError();

//      Constructor to handle an implicit reassignment type mismatch.
//          variable: variable name (input)
//          expected_type: previous type of variable (input)
//          received_type: type of expression being reassigned to the variable (input)
        ImplicitMismatchError(const std::string& variable, const TypingUtils::dataType expected_type, const TypingUtils::dataType received_type);
};

// Error class representing an explicit reassignment type mismatch error.
class ExplicitMismatchError : public TypeMismatchError {
    public: 
//      Default constructor.
        ExplicitMismatchError();

//      Constructor to handle an explicit reassignment type mismatch or display an error message.
//          variable: variable name or error message to display (input)
//          error_msg: true if variable argument is an error message, 
//                     false if variable argument is a variable name (default false) (input)
        ExplicitMismatchError(const std::string& variable, const bool error_msg = false);

        ExplicitMismatchError(const std::string& variable, const TypingUtils::dataType duplicate);
};

#endif
