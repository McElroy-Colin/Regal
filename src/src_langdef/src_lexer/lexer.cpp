// File containing the Regal lexer function alongside any additional helper code.

#include "../../../include/inc_langdef/langdef.hpp"


// Anonymous namespace containing helper functions/macros for the Regal lexer function.
namespace {

    #define is_whitespace(c) (c == ' ') || (c == '\t') || (c == '\r') || (c == '\f')
    #define is_integer(c) (c >= '0') && (c <= '9')
    #define is_label(c) ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || is_integer(c)

//  Match sequential whitespace characters in the given string starting at the given index. 
//  Return the index succeeding the final whitespace character.
//      line: the line of code (input)
//      string_index: the first index to match with whitespace (input)
    int match_whitespace(const String& line, const int string_index) {
        int whitespace_index;

        for (whitespace_index = string_index; is_whitespace(line[whitespace_index]); whitespace_index++);
        return whitespace_index;
    }

//  Match sequential integer characters in the given string starting at the given index.
//  Store both the index succeeding the final integer character and the described integer.
//  This function assumes that the first character at the given index is a number.
//      line: the line of code (input)
//      string_index: the first index to match with integers (input)
//      integer_token: the index succeeding the matched integer and the integer value (output)
    void match_integer(String& line, const int string_index, std::vector<int>& integer_token) {
        int integer_index, integer_value;

        for (integer_index = string_index; is_integer(line[integer_index]); integer_index++);
        integer_value = stoi(line.substr(string_index, (integer_index - string_index)));
        integer_token = { integer_index, integer_value };
        return;
    }

//  Match a custom variable label in the given string at the given index.
//  Store both the index succeeding the label and the label text.
//  This function assumes that the first character at the given index is a label but not a number.
//      line: the line of code (input)
//      string_index: the first index to match with a label (input)
//      label_token: the index succeeding the matched label and the label string (output)
    void match_label(String& line, const int string_index, Token& label_token) {
        String label;
        int label_index;

        for (label_index = string_index; is_label(line[label_index]); label_index++);
        label = line.substr(string_index, (label_index - string_index));
        label_token = { label_index, label };
        return;
    }

//  Match the given target string to the given line starting at the given index.
//  Return the index succeeding the target if matched or the original given index if not matched, 
//  returned index also succeeds any following whitespace if the given boolean is true.
//      line: the line of code (input)
//      string_index: the first index to match with a token (input)
//      end_in_whitespace: true if the matched token should be succeeded by whitespace (input)
    int match_token(const String& line, const String& target, const int string_index, const bool end_in_whitespace) {
        int word_index;

        for (word_index = string_index; line[word_index] 
            && (line[word_index] == target[word_index - string_index]); word_index++);
        if (target[word_index - string_index]) {
            return string_index; // return the original index if the entire target was not matched
        } else if (end_in_whitespace) {
            if ((is_whitespace(line[word_index])) || (!line[word_index])) {
                return match_whitespace(line, word_index);
            }
            return string_index; // return the original index if there is no whitespace but the boolean is true
        }
        return word_index;
    }

}

// Lex a string into a list of Regal syntax tokens.
// Stores a list of arrays, with each array containing the token enum value and any additional necessary data.
//      line: the line of code to lex (input)
//      token_list: list of tokens from the given line (output)
void lex_string(String& line, std::list<Token>& token_list) {
    int string_index = 0;
    int matched_index;

    while (string_index < line.size()) {

        if (line[string_index] == '\n') {
            return; // end lexing at the end of a line

        } else if (is_whitespace(line[string_index])) {
            string_index = match_whitespace(line, string_index); // bypass whitespace characters

        } else if (is_integer(line[string_index])) {
            std::vector<int> position_integer; 
            match_integer(line, string_index, position_integer);
            Token token = { Int, position_integer[1] }; // include the integer value in the token vector

            token_list.push_back(token);
            string_index = position_integer[0];

        } else if ((matched_index = match_token(line, "let", string_index, true)) > string_index) {
            Token token = { Let };

            token_list.push_back(token);
            string_index = matched_index;

        } else if ((matched_index = match_token(line, "now", string_index, true)) > string_index) {
            Token token = { Now };

            token_list.push_back(token);
            string_index = matched_index;

        } else if ((matched_index = match_token(line, "nothing", string_index, true)) > string_index) {
            Token token = { Nothing };

            token_list.push_back(token);
            string_index = matched_index;

//      Assume this label starts with a letter or '_' since the same index was checked for an integer already.
        }  else if (is_label(line[string_index])) {
            Token position_label;
            match_label(line, string_index, position_label);
            Token token = { Var, position_label[1] }; // include the label text in the token vector

            token_list.push_back(token);
            string_index = std::get<int>(position_label[0]);
        
        } else if (match_token(line, "**", string_index, false) > string_index) {
            Token token = { Exp };

            token_list.push_back(token);
            string_index += 2;
        
        } else {
//          Switch statement for single character tokens.
            switch (line[string_index]) {
                case '=': {
                    Token token = { Bind };

                    token_list.push_back(token);
                    string_index++;
                    break;
                }
                case '+': {
                    Token token = { Plus };

                    token_list.push_back(token);
                    string_index++;
                    break;
                }
                case '-': {
                    if (is_integer(line[string_index + 1])) {
                        std::vector<int> position_integer;
                        match_integer(line, ++string_index, position_integer);
                        Token token = { Int, -position_integer[1] };

                        token_list.push_back(token);
                        string_index = position_integer[0];
                    } else {
                        Token token = { Minus };

                        token_list.push_back(token);
                        string_index++;
                    }

                    break;
                }
                case '/': {
                    Token token = { Div };

                    token_list.push_back(token);
                    string_index++;
                    break;
                }
                case '*': {
                    Token token = { Mult };

                    token_list.push_back(token);
                    string_index++;
                    break;
                }
                case '(': {
                    Token token = { LeftPar };

                    token_list.push_back(token);
                    string_index++;
                    break;
                }
                case ')': {
                    Token token = { RightPar };

                    token_list.push_back(token);
                    string_index++;
                    break;
                }
                default:
                    int prblm_token_index = string_index;

//                  Identify the problematic token.
                    while (!is_whitespace(line[prblm_token_index])) { prblm_token_index++; }
                    const String problem_token = line.substr(string_index, prblm_token_index - string_index);

                    const String error_msg = "token \'" + problem_token + "\' not recognized";
                    throw std::runtime_error(error_msg);
            }
        }
    }

    return;
}
