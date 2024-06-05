#include <list>
#include <vector>
#include <variant>
#include "tokens.hpp"
#include "../langdef.hpp"

#define is_whitespace(c) (c == ' ') || (c == '\t') || (c == '\r') || (c == '\f')
#define is_valid_label(c) ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_')
#define is_integer(c) (c >= '0') && (c <= '9')

using String = std::string;
using IntArray = std::vector<int>;
using DataArray = std::vector<std::variant<int, String>>;
using IntMatrix = std::list<DataArray>;

int match_whitespace(String line, int start_offset) {
    int whitespace_index;

    for (whitespace_index = start_offset; is_whitespace(line[whitespace_index]); whitespace_index++);
    return whitespace_index;
}

IntArray match_integer(String line, int string_index) {
    int integer_index, integer_value;

    for (integer_index = string_index; is_integer(line[integer_index]); integer_index++);
    integer_value = stoi(line.substr(string_index, (integer_index - string_index)));
    return { integer_index, integer_value };
}

DataArray match_label(String line, int string_index) {
    String label;
    int label_index;

    for (label_index = string_index; is_valid_label(line[label_index]) || 
        is_integer(line[label_index]); label_index++);
    label = line.substr(string_index, (label_index - string_index));
    return { label_index, label };
}

int match_token(String word_segment, String target, int string_index, bool end_in_whitespace) {
    int word_index;

    for (word_index = string_index; word_segment[word_index] 
        && (word_segment[word_index] == target[word_index - string_index]); word_index++);
    if (target[word_index - string_index]) {
        return string_index;
    } else if (end_in_whitespace) {
        if ((is_whitespace(word_segment[word_index])) || (!word_segment[word_index])) {
            return match_whitespace(word_segment, word_index);
        }
        return string_index;
    }
    return word_index;
}

IntMatrix lex_string(String line) {
    IntMatrix token_list;
    int string_index = 0;
    int matched_index;

    while (string_index < line.size()) {
        if (is_whitespace(line[string_index])) {
            string_index = match_whitespace(line, string_index);
        } else if (is_integer(line[string_index])) {
            IntArray position_integer = match_integer(line, string_index);
            DataArray token = { Int, position_integer[1] };

            token_list.push_back(token);
            string_index = position_integer[0];
        } else if ((matched_index = match_token(line, "let", string_index, true)) > string_index) {
            DataArray token = { Let };

            token_list.push_back(token);
            string_index = matched_index;
        } else if ((matched_index = match_token(line, "now", string_index, true)) > string_index) {
            DataArray token = { Now };

            token_list.push_back(token);
            string_index = matched_index;
        }  else if (is_valid_label(line[string_index])) {
            DataArray position_label = match_label(line, string_index);
            DataArray token = { Var, position_label[1] };

            token_list.push_back(token);
            string_index = std::get<int>(position_label[0]);
        } else if (match_token(line, "**", string_index, false) > string_index) {
            DataArray token = { Exp };

            token_list.push_back(token);
            string_index += 2;
        } else if (line[string_index] == '=') {
            DataArray token = { Bind };

            token_list.push_back(token);
            string_index++;
        } else if (line[string_index] == '+') {
            DataArray token = { Plus };

            token_list.push_back(token);
            string_index++;
        } else if (line[string_index] == '-') {
            DataArray token = { Minus };

            token_list.push_back(token);
            string_index++;
        } else if (line[string_index] == '/') {
            DataArray token = { Div };

            token_list.push_back(token);
            string_index++;
        } else if (line[string_index] == '*') {
            DataArray token = { Mult };

            token_list.push_back(token);
            string_index++;
        } else if (line[string_index] == '(') {
            DataArray token = { LeftPar };

            token_list.push_back(token);
            string_index++;
        } else if (line[string_index] == ')') {
            DataArray token = { RightPar };

            token_list.push_back(token);
            string_index++;
        } else {
            perror("Failed lexing");
            exit(EXIT_FAILURE);
        }
    }
    return token_list;
}