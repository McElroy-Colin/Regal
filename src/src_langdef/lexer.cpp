// File containing the Regal lexer function alongside any additional helper code.

#include <tuple>
#include "inc_langdef/lexer.hpp"

using std::string, std::pair, std::tuple, std::list, std::array, std::make_pair, std::make_tuple, std::get;
using namespace TokenDef;


// Anonymous namespace containing helper functions/macros for the Regal lexer function.
namespace {

//  Character macros. '#' is the comment character, so is considered whitespace.
    #define is_whitespace(c) (c == ' ') || (c == '\t') || (c == '#') 
    #define is_integer(c) (c >= '0') && (c <= '9')
    #define is_label(c) ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || is_integer(c)

//  Match sequential whitespace characters and in-line comments in the given string starting at the given index. 
//  Return the index succeeding the final whitespace or comment character.
//      input: string of code (input)
//      string_index: the first index to match with whitespace (input)
    int _match_whitespace(const string& input, const int string_index) {
        int whitespace_index;

        for (whitespace_index = string_index; is_whitespace(input[whitespace_index]); whitespace_index++) {
            if (input[whitespace_index] == '#') {
//              Do not match a multi-line comment.
                if ((whitespace_index + 1 < input.size()) && (input[whitespace_index + 1] == '#')) {
                    return whitespace_index;
                }

//              Match a single line comment
                while (input[whitespace_index] != '\n') { whitespace_index++; }
                return whitespace_index;
            }
        }
        return whitespace_index;
    }

//  Match the given target string to the given input starting at the given index.
//  Return the index succeeding the target if matched or the original given index if not matched, 
//  returned index also succeeds any following whitespace if the given boolean is true.
//      input: string of code (input)
//      string_index: the first index to match with a token (input)
//      end_in_nonlabel: true if the token cannott be followed by a label character
//                       false if the token can be followed by any character (input)
    int _match_token(const string& input, const string& target, const int string_index, const bool end_in_nonlabel) {
        int word_index;

        for (word_index = string_index; input[word_index] && (input[word_index] == target[word_index - string_index]); word_index++);

//      Return the original index if the entire target was not matched or if the target was succeeded with an illegal character.
        if ((target[word_index - string_index]) || (end_in_nonlabel && is_label(input[word_index]))) {
            return string_index;
        }

        return word_index;
    }

//  Forward declaration to handle cyclic function dependency in the following two functions.
//  Match sequential whitespace characters and comments, count the indentation of the final line of whitespace or comments. 
//  Return a pair with the index succeeding the final whitespace/comment character and the amount of indent on the final line.
//      input: string of code (input)
//      string_index: the first index to match with whitespace (input)
    pair<int, int> _query_whitespace(const string& input, const int string_index);

//  Match a multi-line comment and return a tuple containing the index succeeding the final comment character, 
//  the amount of indent in the final line of the comment, and whether the comment was in a single line.
//      input: string of code (input)
//      string_index: the first index to match with a comment (after '##') (input)
//      inline_comm: true if the comment is already in a line containing regular code (input)
    tuple<int, int, bool> _match_multiline_comment(const string& input, const int string_index, const bool inline_comm) {
        int comment_index = string_index;
        int indent_count = 0;

        while (comment_index < input.size() - 1) {
            if (input[comment_index] == '\n') {
//              If the comment is in a line containing code and it goes to another line, use _query_whitespace to count its indent.
                if (inline_comm) {
                    pair<int, int> position_count = _query_whitespace(input, string_index - 2);
//                  Return the third element as true to indicate that a newline token should be inserted.
                    return make_tuple(position_count.first, position_count.second, true);
                }

                comment_index++;
                indent_count = 0;
            } else if (input[comment_index] == '\t') {
                comment_index++;
                indent_count += TAB_WIDTH;
            } else if (input[comment_index] != '#') {
                comment_index++;
                indent_count++;
            } else if (input[comment_index + 1] == '#') {
//              Return the third element as false to indicate that the comment stayed in one line.
                return make_tuple(comment_index + 2, indent_count + 2, false);
            } else {
                comment_index += 2;
                indent_count += 2;
            }
        }

        throw UnexpectedInputError("unclosed comment at end of file");
    }

//  Match sequential whitespace characters and comments, count the indentation of the final line of whitespace or comments. 
//  Return a pair with the index succeeding the final whitespace/comment character and the amount of indent on the final line.
//      input: string of code (input)
//      string_index: the first index to match with whitespace (input)
    pair<int, int> _query_whitespace(const string& input, const int string_index) {
        int whitespace_index;
        int indent_count = 0;

        for (whitespace_index = string_index; is_whitespace(input[whitespace_index]) || (input[whitespace_index] == '\n'); whitespace_index++) {
            if (input[whitespace_index] == '\t') {
                indent_count += TAB_WIDTH;
            } else if (input[whitespace_index] == ' ') {
                indent_count++;
            } else if (input[whitespace_index] == '#') {
//              Check for a multi-line comment.
                if ((whitespace_index + 1 < input.size()) && input[whitespace_index + 1] == '#') {
                    tuple<int, int, bool> position_indent = _match_multiline_comment(input, whitespace_index + 2, false);
                    whitespace_index = get<0>(position_indent) - 1;
                    indent_count = get<1>(position_indent);
//              Handle a single line comment.
                } else {
                    while (input[whitespace_index] != '\n') { whitespace_index++; }
                    indent_count = 0;
                }
//          Whitespace is only counted on the final input of input.
            } else if (input[whitespace_index] == '\n') {
                indent_count = 0;
            }
        }

        return make_pair(whitespace_index, indent_count);
    }

//  Match sequential integer characters in the given string starting at the given index.
//  Store both the index succeeding the final integer character and the described integer.
//  This function assumes that the first character at the given index is a number.
//      input: string of code (input)
//      string_index: the first index to match with integers (input)
//      index_value: the index succeeding the matched integer and the integer value (output)
    void _match_integer(string& input, const int string_index, pair<int, int>& index_value) {
        int integer_index, integer_value;

        for (integer_index = string_index; is_integer(input[integer_index]); integer_index++);

        integer_value = stoi(input.substr(string_index, (integer_index - string_index)));
        index_value = { integer_index, integer_value };
        return;
    }

//  Match a custom variable label in the given string at the given index.
//  Store both the index succeeding the label and the label text.
//  This function assumes that the first character at the given index is a label but not a number.
//      input: string of code (input)
//      string_index: the first index to match with a label (input)
//      position_label: the index succeeding the matched label and the label string (output)
    void _match_label(string& input, const int string_index, pair<int, string>& position_label) {
        string label;
        int label_index;

        for (label_index = string_index; is_label(input[label_index]); label_index++);

        label = input.substr(string_index, (label_index - string_index));
        position_label = { label_index, label };
        return;
    }
}

// Lex a string into a list of Regal syntax tokens.
// Stores a list of arrays, with each array containing the token enum value and any additional necessary data.
//      input: the code to lex (input)
//      token_list: list of tokens from the given input (output)
void lex_string(string& input, list<token>& token_list) {
    int matched_index, line_length;

//  Match any whitespace or comments preceding the first input of code.
    pair<int, int> position_count = _query_whitespace(input, 0);
    int string_index = position_count.first;
    token_list.push_back({tokenKey::Newline, position_count.second});

    line_length = input.size();
    while (string_index < line_length) {
        if (is_integer(input[string_index])) {
            pair<int, int> position_integer; 
            _match_integer(input, string_index, position_integer);
//          Include the integer value in the token.
            token token = { tokenKey::Int, position_integer.second };

            token_list.push_back(token);
            string_index = position_integer.first;

//      Match keywords before checking for variable labels.
        } else if ((matched_index = _match_token(input, "let", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::Let });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "now", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::Now });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "if", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::If });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "else", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::Else });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "and", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::AndW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "or", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::OrW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "xor", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::XorW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "is", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::Is });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "not", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::NotW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "true", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::Bool, 1 });
            string_index = matched_index;

        } else if ((matched_index = _match_token(input, "false", string_index, true)) > string_index) {
            token_list.push_back({ tokenKey::Bool, 0 });
            string_index = matched_index;

//      Assume this label starts with a letter or '_' since the same index was checked for an integer already.
        }  else if (is_label(input[string_index])) {
            token token;
            pair<int, string> position_label;
            _match_label(input, string_index, position_label);
//          Include the variable label in the token.
            token = { tokenKey::Var, position_label.second };

            token_list.push_back(token);
            string_index = position_label.first;
        
        } else if (_match_token(input, "==", string_index, false) > string_index) {
            token_list.push_back({ tokenKey::Equals });
            string_index += 2;

        } else if (_match_token(input, "**", string_index, false) > string_index) {
            token_list.push_back({ tokenKey::Exp });
            string_index += 2;

        } else if (_match_token(input, "||", string_index, false) > string_index) {
            token_list.push_back({ tokenKey::Xor });
            string_index += 2;

        } else if (_match_token(input, "<=", string_index, false) > string_index) {
            token_list.push_back({ tokenKey::LessEqual });
            string_index += 2;

        } else if (_match_token(input, ">=", string_index, false) > string_index) {
            token_list.push_back({ tokenKey::GrEqual });
            string_index += 2;

//      Handle a mult-line comment.
        } else if (_match_token(input, "##", string_index, false) > string_index) {
            tuple<int, int, bool> position_count_multi = _match_multiline_comment(input, string_index + 2, true);
            string_index = get<0>(position_count_multi);

//          If the comment went to a new line, insert a Newline token with the indentation level of the final line of comments.
            if (get<2>(position_count_multi)) {
                token_list.push_back({tokenKey::Newline, get<1>(position_count_multi)});
            }
        
        } else {
//          Switch statement for single character tokens.
            switch (input[string_index]) {
                case '\n': {
//                  Count the indent of a new line of code.
                    position_count = _query_whitespace(input, string_index + 1);
                    token_list.push_back({tokenKey::Newline, position_count.second});
                    string_index = position_count.first; 

                    break;
                }              
                case '=':
                    token_list.push_back({ tokenKey::Bind });
                    string_index++;
                    break;

                case '+':
                    token_list.push_back({ tokenKey::Plus });
                    string_index++;
                    break;
                
                case '-':
                    token_list.push_back({ tokenKey::Minus });
                    string_index++;
                    break;
                
                case '/':
                    token_list.push_back({ tokenKey::Div });
                    string_index++;
                    break;
                
                case '*':
                    token_list.push_back({ tokenKey::Mult });
                    string_index++;
                    break;
                
                case '&':
                    token_list.push_back({ tokenKey::And });
                    string_index++;
                    break;
            
                case '|':
                    token_list.push_back({ tokenKey::Or });
                    string_index++;
                    break;
                
                case '!':
                    token_list.push_back({ tokenKey::Not });
                    string_index++;
                    break;
                
                case '>':
                    token_list.push_back({ tokenKey::Greater });
                    string_index++;
                    break;
                
                case '<':
                    token_list.push_back({ tokenKey::Less });
                    string_index++;
                    break;
            
                case '(':
                    token_list.push_back({ tokenKey::LeftPar });
                    string_index++;
                    break;
                
                case ')':
                    token_list.push_back({ tokenKey::RightPar });
                    string_index++;
                    break;
                
                case '#':
                    while (input[string_index] != '\n') { string_index++; }
                    break;
                
                default:
//                  Handle unrecognized token.
                    throw UnrecognizedInputError(input, string_index);
            }
        }

//      Bypass whitespace or in-line comments (if any) after a matched token.
        string_index = _match_whitespace(input, string_index);
    }

//  Ensure trailing newline closes the file scope.
    if (get<tokenKey>(token_list.back()[0]) == tokenKey::Newline) {
        token_list.back()[1] = -1;
    } else {
        token_list.push_back({ tokenKey::Newline, -1 });
    }

    return;
}
