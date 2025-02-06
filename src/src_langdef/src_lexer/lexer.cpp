// File containing the Regal lexer function alongside any additional helper code.

#include "../../../include/inc_langdef/langdef.hpp"
#include "../../../include/inc_debug/error_handling.hpp"


constexpr int TAB_WIDTH = 4;

// Anonymous namespace containing helper functions/macros for the Regal lexer function.
namespace {

    #define is_whitespace(c) (c == ' ') || (c == '\t') 
    #define is_integer(c) (c >= '0') && (c <= '9')
    #define is_label(c) ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || is_integer(c)

//  Match sequential whitespace characters in the given string starting at the given index. 
//  Return the index succeeding the final whitespace character.
//      line: the line of code (input)
//      string_index: the first index to match with whitespace (input)
    int _match_whitespace(const String& line, const int string_index) {
        int whitespace_index;

        for (whitespace_index = string_index; is_whitespace(line[whitespace_index]); whitespace_index++);
        return whitespace_index;
    }

//  Match and count sequential whitespace characters, including newline, in the given string starting at the given index. 
//  Return a pair with the index succeeding the final whitespace character and the amount of matched whitespace on the final line.
//      line: the line of code (input)
//      string_index: the first index to match with whitespace (input)
    std::pair<int, int> _query_whitespace(const String& line, const int string_index) {
        int whitespace_index;
        int whitespace_count = 0;

        for (whitespace_index = string_index; is_whitespace(line[whitespace_index]) || (line[whitespace_index] == '\n'); whitespace_index++) {
            if (line[whitespace_index] == '\t') {
                whitespace_count += TAB_WIDTH;
            } else if (line[whitespace_index] == ' ') {
                whitespace_count += 1;
//          Whitespace is only counted on the final line of input.
            } else if (line[whitespace_index] == '\n') {
                whitespace_count = 0;
            }
        }

        return std::make_pair(whitespace_index, whitespace_count);
    }

//  Match sequential integer characters in the given string starting at the given index.
//  Store both the index succeeding the final integer character and the described integer.
//  This function assumes that the first character at the given index is a number.
//      line: the line of code (input)
//      string_index: the first index to match with integers (input)
//      integer_token: the index succeeding the matched integer and the integer value (output)
    void _match_integer(String& line, const int string_index, std::vector<int>& integer_token) {
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
    void _match_label(String& line, const int string_index, Token& label_token) {
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
//      end_without_label: true if the matched token can be succeeded with anything that is not a label character, 
//                         false if the token must be succeeded by whitespace (input)
    int _match_token(const String& line, const String& target, const int string_index, const bool end_without_label) {
        int word_index;

        for (word_index = string_index; line[word_index] && (line[word_index] == target[word_index - string_index]); word_index++);\

        if ((target[word_index - string_index]) || (end_without_label && (is_label(line[word_index])))) {
//          Return the original index if the entire target was not matched or if the target was succeeded with an illegal character.
            return string_index;
        }

        return word_index;
    }

}

// Lex a string into a list of Regal syntax tokens.
// Stores a list of arrays, with each array containing the token enum value and any additional necessary data.
//      line: the line of code to lex (input)
//      token_list: list of tokens from the given line (output)
void lex_string(String& line, std::list<Token>& token_list) {
    int matched_index, line_length;

//  Match any whitespace preceding the first line of code.
    std::pair<int, int> position_count = _query_whitespace(line, 0);
    int string_index = position_count.first;
    token_list.push_back({Newline, position_count.second});

    line_length = line.size();
    while (string_index < line_length) {
        if (is_integer(line[string_index])) {
            std::vector<int> position_integer; 
            _match_integer(line, string_index, position_integer);
//          Include the integer value in the token.
            Token token = { Int, position_integer[1] };

            token_list.push_back(token);
            string_index = position_integer[0];

//      Match keywords before checking for variable labels.
        } else if ((matched_index = _match_token(line, "let", string_index, true)) > string_index) {
            token_list.push_back({ Let });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "now", string_index, true)) > string_index) {
            token_list.push_back({ Now });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "if", string_index, true)) > string_index) {
            token_list.push_back({ If });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "else", string_index, true)) > string_index) {
            token_list.push_back({ Else });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "and", string_index, true)) > string_index) {
            token_list.push_back({ AndW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "or", string_index, true)) > string_index) {
            token_list.push_back({ OrW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "xor", string_index, true)) > string_index) {
            token_list.push_back({ XorW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "is", string_index, true)) > string_index) {
            token_list.push_back({ Is });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "not", string_index, true)) > string_index) {
            token_list.push_back({ NotW });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "true", string_index, true)) > string_index) {
            token_list.push_back({ Bool, 1 });
            string_index = matched_index;

        } else if ((matched_index = _match_token(line, "false", string_index, true)) > string_index) {
            token_list.push_back({ Bool, 0 });
            string_index = matched_index;

//      Assume this label starts with a letter or '_' since the same index was checked for an integer already.
        }  else if (is_label(line[string_index])) {
            Token position_label, token;
            _match_label(line, string_index, position_label);
//          Include the variable label in the token.
            token = { Var, position_label[1] };

            token_list.push_back(token);
            string_index = std::get<int>(position_label[0]);
        
        } else if (_match_token(line, "==", string_index, false) > string_index) {
            token_list.push_back({ Equals });
            string_index += 2;

        } else if (_match_token(line, "**", string_index, false) > string_index) {
            token_list.push_back({ Exp });
            string_index += 2;

        } else if (_match_token(line, "||", string_index, false) > string_index) {
            token_list.push_back({ Xor });
            string_index += 2;
        
        } else {
//          Switch statement for single character tokens.
            switch (line[string_index]) {
                case '\n': {
//                  Match and count whitespace preceding a new line of code.
                    position_count = _query_whitespace(line, string_index);
                    string_index = position_count.first;
                    token_list.push_back({Newline, position_count.second});

                    break;
                }              
                case '=':
                    token_list.push_back({ Bind });
                    string_index++;
                    break;

                case '+':
                    token_list.push_back({ Plus });
                    string_index++;
                    break;
                
                case '-':
                    token_list.push_back({ Minus });
                    string_index++;
                    break;
                
                case '/':
                    token_list.push_back({ Div });
                    string_index++;
                    break;
                
                case '*':
                    token_list.push_back({ Mult });
                    string_index++;
                    break;
                
                case '&':
                    token_list.push_back({ And });
                    string_index++;
                    break;
            
                case '|':
                    token_list.push_back({ Or });
                    string_index++;
                    break;
                
                case '!':
                    token_list.push_back({ Not });
                    string_index++;
                    break;
                
                case '>':
                    token_list.push_back({ Greater });
                    string_index++;
                    break;
                
                case '<':
                    token_list.push_back({ Less });
                    string_index++;
                    break;
            
                case '(':
                    token_list.push_back({ LeftPar });
                    string_index++;
                    break;
                
                case ')':
                    token_list.push_back({ RightPar });
                    string_index++;
                    break;
                
                default:
//                  Handle unrecognized token.
                    throw UnrecognizedInputError(line, string_index);
            }
        }

//      Bypass whitespace (if any) after a matched token.
        string_index = _match_whitespace(line, string_index);
    }

    return;
}
