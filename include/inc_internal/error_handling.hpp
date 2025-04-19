/*

Class declarations for interpretation errors.

*/

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <stdexcept>

#include "inc_interpreter/interp_utils.hpp"


/*
    Create a display string prefix for error messages that contains the given line number.

    Parameters:
        line_number: line number to embed into the prefix string (input)
    
    Return the created string.
*/
inline const std::string _line_prefix(const std::uint32_t line_number) noexcept {
    return "[" + std::to_string(line_number) + "]: ";
}

// Severe logic mistakes in interpreter code. These should never throw.
class FatalError : public std::runtime_error {
    public: 
//      Default error message with a line number.
        explicit FatalError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        explicit FatalError(const std::string& error_msg, const std::uint32_t line_number);
};

// Undefined token errors.
class UnrecognizedInputError : public std::runtime_error {
    private:
/*
        Retrieve the collection of non-whitespace characters in the given string starting at the given index.

        This method assumes that the given string's size can fit into an unsigned 32-bit integer,
        i.e. the string has less than 4,294,967,295 characters.

        Parameters: 
            input: the string to extract a token from (input)
            start_index: index that the token starts on (input)
        
        Return the extracted token substring.
*/
        static const std::string extract_input(const std::string& input, const std::uint32_t start_index) noexcept;

    public: 
//      Default error message with a line number.
        explicit UnrecognizedInputError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        explicit UnrecognizedInputError(const std::string& error_msg, const std::uint32_t line_number);

/*
        Throw an error message when a token is unrecgonized.
        Extract nonwhitespace characters at the given start index to display with the message.

        Paramters: 
            input: string containing the unrecognized token
            start_index: first index of the unrecongized token in the input string
            line_number: line number of the unrecognized token
*/
        explicit UnrecognizedInputError(const std::string& input, const std::uint32_t start_index, const std::uint32_t line_number);
};

// Incorrect ordering of tokens.
class UnexpectedInputError : public std::runtime_error {
    public:
//      Default error message with a line number.
        explicit UnexpectedInputError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        explicit UnexpectedInputError(const std::string& error_msg, const std::uint32_t line_number);

/*
        Throw an error message when user input ends before an AST can be generated.

        Parameters:
            expected_token: next expected token determined by the AST generator
            literal: true if the expected_token should be displayed literally
*/
        explicit UnexpectedInputError(const TokenDef::tokenKey expected_token, const bool literal);

/*
        Throw an error message when a token is input out of order.
        Include the given token and the expected token in the message.

        Parameters:
            given_token: the token received by the AST generator
            expected_token: token expected from the AST generator
            literal: true if the given_token should be displayed literally
            line_number: line number of the given token
*/
        explicit UnexpectedInputError(const TokenDef::token& given_token, const TokenDef::tokenKey expected_token, const bool literal, const std::uint32_t line_number);
};

// Invalid input logic.
class IncorrectInputError : public std::runtime_error {
    public:
//      Default error message with a line number.
        explicit IncorrectInputError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        explicit IncorrectInputError(const std::string& error_msg, const std::uint32_t line_number);
};

// Error class representing a missing code block.
class IncorrectIndentError : public IncorrectInputError {
    public:
//      Default error message with a line number.
        explicit IncorrectIndentError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        explicit IncorrectIndentError(const std::string& error_msg, const std::uint32_t line_number);

/*
        Throw an error message when the given operator expected an indented code block and did not receive one.

        Parameters: 
            op: the operator that expected an indent
            line_number: line number of the operator
*/
        explicit IncorrectIndentError(const TokenDef::tokenKey op, const std::uint32_t line_number);
};

// Uninitialized or preinitialized variable errors.
class VariableInitializationError : public IncorrectInputError {
    public:
//      Default error message with a line number.
        VariableInitializationError(const std::uint32_t line_number);

/*
        Throw an error message when the given variable was initialized more than once or referenced without initializations.

        Parameters: 
            variable: variable with an initialization error
            not_initialized: true if the variable is being referenced without initialization
                             false if the variable is being initialized twice
            line_number: the line number of the variable
*/
        VariableInitializationError(const std::string& variable, const bool not_initialized, const std::uint32_t line_number);
};

// Type errors.
class TypeMismatchError : public IncorrectInputError {
    public:
//      Default error message with a line number.
        TypeMismatchError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        TypeMismatchError(const std::string& error_msg, const std::uint32_t line_number);

/*
        Throw an error message when an operator takes expressions of the incorrect type.

        Parameters:
            op: operator throwing an error
            literal: true if the operator should be displayed literally
            type1: type of the first expression in the operator
            type2: type of the second expression in the operator OR the expected type of the first expression 
            unary: true if the given operator acts as unary, making type2 the expected type
            line_number: the line number of the operator error
*/
        TypeMismatchError(const TokenDef::tokenKey op, const bool literal, const TypingUtils::dataType type1, const TypingUtils::dataType type2, 
                          const bool unary, const std::uint32_t line_number);

};

// Errors caught during code execution.
class ExecutionError : public std::runtime_error {
    public:
//      Default error message with a line number.
        ExecutionError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        ExecutionError(const std::string& error_msg, const std::uint32_t line_number);
};

// Data storage overflow errors.
class OverflowError : public ExecutionError {
    public:
//      Default error message with a line number.
        OverflowError(const std::uint32_t line_number);

//      Throw the given error message with the given line number.
        OverflowError(const std::string& error_msg, const std::uint32_t line_number);
};

#endif
