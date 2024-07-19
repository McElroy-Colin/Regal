// File containing the Regal lexer function alongside any additional helper code.

#include "../langdef.hpp"

// Anonymous namespace containing helper functions/macros for the Regal lexer function.
namespace {

    #define is_whitespace(c) (c == ' ') || (c == '\t') || (c == '\r') || (c == '\f')
    #define is_integer(c) (c >= '0') && (c <= '9')
    #define is_label(c) ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || is_integer(c)

//  Match sequential whitespace characters in the given string starting at the given index. 
//  Return the index succeeding the final whitespace character.
    int match_whitespace(String line, int string_index) {
        int whitespace_index;

        for (whitespace_index = string_index; is_whitespace(line[whitespace_index]); whitespace_index++);
        return whitespace_index;
    }

//  Match sequential integer characters in the given string starting at the given index.
//  Return both the index succeeding the final integer character and the described integer in a vector.
//      This function assumes that the first character at the given index is a number.
    IntArray match_integer(String line, int string_index) {
        int integer_index, integer_value;

        for (integer_index = string_index; is_integer(line[integer_index]); integer_index++);
        integer_value = stoi(line.substr(string_index, (integer_index - string_index)));
        return { integer_index, integer_value };
    }

//  Match a custom variable label in the given string at the given index.
//  Return both the index succeeding the label and the label text in a variant vector.
//      This function assumes that the first character at the given index is a label but not a number.
    Token match_label(String line, int string_index) {
        String label;
        int label_index;

        for (label_index = string_index; is_label(line[label_index]); label_index++);
        label = line.substr(string_index, (label_index - string_index));
        return { label_index, label };
    }

//  Match the given target string to the given line starting at the given index.
//  Return the index succeeding the target if matched or the original given index if not matched, 
//  returned index also succeeds any following whitespace if the given boolean is true.
    int match_token(String line, String target, int string_index, bool end_in_whitespace) {
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
// Return a list of arrays, with each array containing the token enum value and any additional necessary data.
TokenList lex_string(String line) {
    TokenList token_list;
    int string_index = 0;
    int matched_index;

    while (string_index < line.size()) {

        if (line[string_index] == '\n') {
            return token_list; // end lexing at the end of a line

        } else if (is_whitespace(line[string_index])) {
            string_index = match_whitespace(line, string_index); // bypass whitespace characters

        } else if (is_integer(line[string_index])) {
            IntArray position_integer = match_integer(line, string_index);
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
            Token position_label = match_label(line, string_index);
            Token token = { Var, position_label[1] }; // include the label text in the token vector

            token_list.push_back(token);
            string_index = std::get<int>(position_label[0]);
        
        } else if (match_token(line, "**", string_index, false) > string_index) {
            Token token = { Exp };

            token_list.push_back(token);
            string_index += 2;
        
        } else if (line[string_index] == '=') {
            Token token = { Bind };

            token_list.push_back(token);
            string_index++;
        
        } else if (line[string_index] == '+') {
            Token token = { Plus };

            token_list.push_back(token);
            string_index++;
        
        } else if (line[string_index] == '-') {
            if (is_integer(line[string_index + 1])) {
                IntArray position_integer = match_integer(line, ++string_index);
                Token token = { Int, -position_integer[1] };

                token_list.push_back(token);
                string_index = position_integer[0];
            } else {
                Token token = { Minus };

                token_list.push_back(token);
                string_index++;
            }

        } else if (line[string_index] == '/') {
            Token token = { Div };

            token_list.push_back(token);
            string_index++;
        
        } else if (line[string_index] == '*') {
            Token token = { Mult };

            token_list.push_back(token);
            string_index++;
        
        } else if (line[string_index] == '(') {
            Token token = { LeftPar };

            token_list.push_back(token);
            string_index++;
        
        } else if (line[string_index] == ')') {
            Token token = { RightPar };

            token_list.push_back(token);
            string_index++;
        
        } else {
            perror("Failed lexing");
            exit(EXIT_FAILURE);
        }
    }
    return token_list;
}