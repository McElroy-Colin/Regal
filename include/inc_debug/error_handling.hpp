// Header file containing custom error structures for debugging compiler source code.

#include "../inc_langdef/langdef.hpp"

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

// TODO: update error throws (they chain currently with messages)

// Error class representing severe logic mistakes in compiler code. These should never throw.
class FatalError : public std::runtime_error {
    public: 
//      Default constructor.
        FatalError() : std::runtime_error("FatalError") {}

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (input)
        FatalError(const String& error_msg, const bool derived = true) 
            : std::runtime_error(derived ? error_msg : "FatalError: " + error_msg) {}
};

// Error class representing the use of an undefined token.
class UnrecognizedInputError : public std::runtime_error {
    private:
//      Extract and store the next token starting at the given line position from the given line.
//          line: text to extract token from (input)
//          line_pos: starting index of the target token in the line (input)
//          result: string to store the extracted token (output)
        String extract_input(const String& line, const int line_pos) {
            int prblm_token_index = line_pos;

//          Bypass whitespace.
            while (!((line[prblm_token_index] == ' ') ||
                    (line[prblm_token_index] == '\t') ||
                    (line[prblm_token_index] == '\r') ||
                    (line[prblm_token_index] == '\f'))) { 
                prblm_token_index++; 
            }

            return line.substr(line_pos, prblm_token_index - line_pos);
        }

    public: 
//      Default constructor.
        UnrecognizedInputError() : std::runtime_error("UnrecognizedInputError") {}

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (input)
        UnrecognizedInputError(const String& error_msg, const bool derived = true) 
            : std::runtime_error(derived ? error_msg : "UnrecognizedInputError: " + error_msg) {}

//      Constructor taking a line and position to extract a problematic token.
//          line: text to extract token from (input)
//          line_pos: starting index of the target token in the line (input)
        UnrecognizedInputError(const String& line, const int line_pos) 
            : std::runtime_error("UnrecognizedInputError: \'" 
                + extract_input(line, line_pos) + "\' is not recognized as a valid symbol or token") {}
};

// Error class representing incorrect ordering of tokens.
class UnexpectedInputError : public std::runtime_error {
    private:
        String received, expected;

    public:
//      Default constructor.
        UnexpectedInputError() : std::runtime_error("UnexpectedInputError") {}

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (input)
        UnexpectedInputError(const String& error_msg, const bool derived = true) 
            : std::runtime_error(derived ? error_msg : "UnexpectedInputError: " + error_msg) {}

//      Constructor to handle when user input ends before parsing recognizes a line.
//          expected_token: token key that was expected by the parser (input)
//          display_option: controls how to display the given token as in display_token (input)
        UnexpectedInputError(const TokenKey expected_token, const DisplayOption display_option)
            : std::runtime_error("UnexpectedInputError: expected \'" 
                + display_token({expected_token}, display_option) + "\' but input ended") {}

//      Constructor taking parameters to create an expected versus received error message.
//          given_token: token that was input and is incorrect in parsing (input) 
//          expected_token: token key that was expected by the parser (input)
//          display_option: controls how to display the given tokens as in display_token (input)
        UnexpectedInputError(const Token& given_token, const TokenKey expected_token, const DisplayOption display_option)
            : std::runtime_error("UnexpectedInputError: expected \'" 
                + display_token({expected_token}, display_option) + "\' but received \'" 
                + display_token(given_token, display_option) + "\'") {}

//      Constructor to handle extra tokens after a parser has completed a line.
//          extra_token: first token after parsing completed (input)
//          display_option: controls how to display the given token as in display_token (input)
        UnexpectedInputError(const Token& extra_token, const DisplayOption display_option) 
            : std::runtime_error("UnexpectedInputError: excess tokens starting at \'" 
                + display_token(extra_token, display_option) + "\'") {}
};

// Error class representing invalid input logic.
class IncorrectInputError : public std::runtime_error {
    public:
//      Default constructor.
        IncorrectInputError() : std::runtime_error("IncorrectInputError") {}

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (input)
        IncorrectInputError(const String& error_msg, const bool derived = true) 
            : std::runtime_error(derived ? error_msg : "IncorrectInputError: " + error_msg) {}
};

// Error class representing uninitialized variable errors.
class VariableNotInitializedError : public IncorrectInputError {
    public: 
//      Default constructor.
        VariableNotInitializedError() : IncorrectInputError("VariableNotInitializedError") {}

//      Constructor to handle an uninitialized variable or display an error message.
//          variable: variable name or error message to display (input)
//          error_msg: true if variable argument is an error message, 
//                     false if variable argument is a variable name (input)
        VariableNotInitializedError(const String& variable, const bool error_msg = false) 
            : IncorrectInputError("VariableNotInitializedError: " + error_msg 
                ? variable 
                : "variable \'" + variable + "\' not initialized") {}
};

// Error class representing duplicate initialized variables.
class VariablePreInitializedError : public IncorrectInputError {
    public:
//      Default constructor.
        VariablePreInitializedError() : IncorrectInputError("VariablePreInitializedError") {}

//      Constructor to handle a duplicate initialized variable or display an error message.
//          variable: variable name or error message to display (input)
//          error_msg: true if variable argument is an error message, 
//                     false if variable argument is a variable name (input)
        VariablePreInitializedError(const String& variable, const bool error_msg = false) 
            : IncorrectInputError("VariablePreInitializedError: " + error_msg 
                ? variable 
                : "variable \'" + variable + "\' already initialized") {}
};

// Error class representing invalid operator uses.
class InavlidOperatorError : public IncorrectInputError {
    public:
//      Default constructor.
        InavlidOperatorError() : IncorrectInputError("InvalidOperatorError") {}

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (input)
        InavlidOperatorError(const String& error_msg, const bool derived = true) 
            : IncorrectInputError(derived ? error_msg : "InvalidOperatorError: " + error_msg) {}

//      Constructor to handle invalid applications of an operator to data.
//          data_type: data using the operator (input)
//          op: operator token key being used incorrectly (input)
        InavlidOperatorError(const Action& data_type, const TokenKey op)
            : IncorrectInputError("InvalidOperatorError: " + std::visit([](const auto& d) { return d->disp(Literal); }, data_type) 
            + " using \'" + display_token({op}, Literal) + "\' operator") {}

//      Constructor to handle an operator taking invalid data. 
//          op: operator token key being used incorrectly (input)
        InavlidOperatorError(const TokenKey op) 
            : IncorrectInputError("InvalidOperatorError: \'" 
                + display_token({op}, Literal) + "\' operator used with invalid type") {}
};

// Error class representing division by 0.
class DivisionByZeroError : public IncorrectInputError {
    public:
//      Default constructor.
        DivisionByZeroError() : IncorrectInputError("DivisionByZeroError") {}

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (input)
        DivisionByZeroError(const String& error_msg, const bool derived) 
            : IncorrectInputError(derived ? error_msg : "DivisionByZeroError: " + error_msg) {}
};

// Error class representing mismatching type errors.
class TypeMismatchError : public IncorrectInputError {
    private:
        String operator_str;
    public:
//      Default constructor.
        TypeMismatchError() : IncorrectInputError("TypeMismatchError") {}

//      Constructor taking a message.
//          error_msg: error message to display (input)
//          derived: true if the error is called from a derived error (input)
        TypeMismatchError(const String& error_msg, const bool derived = true) 
            : IncorrectInputError(derived ? error_msg : "TypeMismatchError: " + error_msg) {}

//      Constructor handling mismatched types on a given operator.
//          op: operator token key wwith mismatched types (input)
        TypeMismatchError(const TokenKey op) 
            : IncorrectInputError("TypeMismatchError: \'" + display_token({op}, Literal) + "\' operator mismatched types") {}

        TypeMismatchError(Action& data_type) 
            : IncorrectInputError("TypeMismatchError: if statement expected a boolean condition but received " 
            + std::visit([](const auto& d) { return d->disp(Literal); }, data_type)) {}
};

// Error class representing an implicit reassignment type mismatch error.
class ImplicitMismatchError : public TypeMismatchError {
    public: 
//      Default constructor.
        ImplicitMismatchError() : TypeMismatchError("ImplicitMismatchError") {}

//      Constructor to handle an implicit reassignment type mismatch or display an error message.
//          variable: variable name or error message to display (input)
//          error_msg: true if variable argument is an error message, 
//                     false if variable argument is a variable name (input)
        ImplicitMismatchError(const String& variable, const bool error_msg = false) 
            : TypeMismatchError(error_msg 
                ? variable 
                : "ImplicitMismatchError: impicit reassignment of variable \'" 
                    + variable + "\' must be of the same type.") {}
};

// Error class representing an explicit reassignment type mismatch error.
class ExplicitMismatchError : public TypeMismatchError {
    public: 
//      Default constructor.
        ExplicitMismatchError() : TypeMismatchError("ExplicitMismatchError") {}

//      Constructor to handle an explicit reassignment type mismatch or display an error message.
//          variable: variable name or error message to display (input)
//          error_msg: true if variable argument is an error message, 
//                     false if variable argument is a variable name (input)
        ExplicitMismatchError(const String& variable, const bool error_msg = false) 
            : TypeMismatchError(error_msg 
                ? variable 
                : "ExplicitMismatchError: explicit reassignment of variable \'" 
                    + variable + "\' must be of a different type.") {}
};

#endif