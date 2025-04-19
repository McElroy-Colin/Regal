/*

The lex_string function and any helper functions or structures.

*/

#include "inc_interpreter/lexer.hpp"

// Standard library aliases
using std::string, std::pair, std::tuple, std::list, std::array, std::out_of_range, std::int32_t, std::uint32_t,
      std::make_pair, std::make_tuple, std::get, std::to_string, std::tie, std::stoul, std::stoull, std::stof, std::stod, std::ignore;

// interp_utils namespaces
using namespace InterpreterUtils;
using namespace TypingUtils;
using namespace TokenDef;

// Strictly in-file helper functions and structures for lex_string.
namespace {
/*
    Determine if the gien character is considered trivia.
    '#' is the comment character, so is considered trivia.
    '\n' is critical to Regal syntax and so is not considered trivia.

    Parameters:
        c: character to check (input)

    Return true if the character is trivia.
*/
    inline constexpr bool _is_trivia(char c) noexcept { return (c == ' ') || (c == '\t') || (c == INLINE_COMMENT_TOKEN); }

/*
    Determine if the given character is an integer.

    Parameters:
        c: character to check (input)

    Return true if the character is an integer.
*/
    inline constexpr bool _is_integer(char c) noexcept { return (c >= '0') && (c <= '9'); }

/*
    Determine if the given character is valid in a label.
    Labels contain any alphanumeric characters and '_'.

    Parameters:
        c: character to check (input)

    Return true if the character could be in a label.
*/
    inline constexpr bool _is_label(char c) noexcept { return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_') || _is_integer(c); }

/*
    Compute the first inline index of the given string at or after the given start index that is not considered trivia.
    Trivia is defined in the is_trivia macro as most whitespace characters as well as comments.
    Update the current line number counter when a newline is passed.

    This function assumes that, in the given string, the index of the next non-trivia character or next line 
    will not overflow an unsigned 32-bit integer. i.e. the index will not reach 4,294,967,296.

    Parameters:
        input: string to bypass inline trivia in (input)
        start_index: first index to check for inline non-trivia (input)
        initial_indent: the indent in the given string at the given start index (input)
        line_number: line number counter to increment when necessary (output)

    Return a pair containing:
        first: the first non-trivia index found. If the rest of the given string is trivia or the given 
               start index is out of range, return exactly the string's size.
        second: the indent after the matched inline trivia. If an inline comment is matched, the indent is 0.
*/
    inline const pair<uint32_t, int32_t> _match_inline_trivia(const string& input, const uint32_t start_index, const int32_t initial_indent, uint32_t& line_number) noexcept {
        uint32_t trivia_index;
        const uint32_t input_size = input.size();
        int32_t curr_indent = initial_indent;

        for (trivia_index = start_index; (trivia_index < input_size) && _is_trivia(input[trivia_index]); trivia_index++, curr_indent++) {
//          A comment character means that the rest of the current line is trivia unless it is a comment block.
            if (input[trivia_index] == '#') {
//              Do not match a comment block denoted with '##'.
                if ((trivia_index + 1 < input_size) && (input[trivia_index + 1] == '#')) {
//                  Return the index at the first '#' given a comment block.
                    return make_pair(trivia_index, curr_indent);
                }

//              Match a single line comment
                while ((trivia_index < input_size) && input[trivia_index] != NEWLINE_TOKEN) { trivia_index++; }
                line_number++;
                return make_pair(trivia_index, 0);
            }
        }
        return make_pair(trivia_index, curr_indent);
    }

/*
    Determine whether the given target string is properly in the given input string at the given start index.
    If the target is present, compute the next index after the target substring in the given input string.
    The given boolean denotes whether the target must be succeeded by a nonlabel character, determined in the is_label macro.
    If the boolean is true, the target will not be present if the substring is found but is succeeded with a label character.
        e.g. If the boolean is true, the input string is "targ5", the target string is "targ", and the start index 0, 
             the target would not be found since '5' succeeds the target string and '5' is a label character.
             If the input string were "targ+", the target would be found since PLUS_TOKEN is not a label character.

    This function assumes that the size of the given target plus the given start index will 
    not overflow an unsigned 32-bit integer. i.e. the index will not reach 4,294,967,296.

    Parameters:
        input: string to check for target substring (input)
        target: string to be identified in input string (input)
        start_index: first index in input string to check for target string (input)
        end_in_nonlabel: true if the target string must be succeeded with a nonlabel character in the input string
                         false if the target string can be succeeded with any character in the input string (input)

    Return the index after the target substring in the input string if the target was properly found, otherwise return the start index.
*/
    inline constexpr uint32_t _match_target(const string& input, const string& target, const uint32_t start_index, const bool end_in_nonlabel) noexcept {
        const uint32_t input_size = input.size();
        const uint32_t target_size = target.size();
        uint32_t word_index = start_index;

//      Increment as long as the two strings match. 
        while ((word_index < input_size) &&
                (word_index - start_index < target_size) &&
//              Check the input from the start index and the target from index 0.
                (input[word_index] == target[word_index - start_index]))
            { word_index++; }

//      Return the original index if the entire target was not matched or if the target was succeeded with an illegal character.
        if ((word_index - start_index < target_size) || (end_in_nonlabel && _is_label(input[word_index]))) {
            return start_index;
        }

        return word_index;
    }

/*
    Compute the index after a comment block starting at the given start index in the given input string.
    Comment blocks are denoted with "##", e.g. "## this is a comment block ##". 
    The given boolean denotes whether the comment began in a line that contained text before the opening "##".
    Update the current line number counter when a newline is passed.

    This function assumes that the size of the comment block plus the given start index will not overflow
    an unsigned 32-bit integer, i.e. the computed index will not reach 4,294,967,296.

    Throw an exception if the given comment block is not closed in the given string, i.e. the computed index reaches the end of the string.

    Parameters:
        input: string to match a comment block in (input)
        start_index: index in the input string just after the opening "##" of the comment block (input)
        initial_indent: the indent of the current line up to the given start index (input)
        line_number: line number counter to increment when necessary (output)

    Return a 3-tuple containing
        <0>: the index after the closing "##" in the input string
        <1>: the amount of indent up to the closing "##" in the input string
        <2>: true if the comment block stayed in its original line
*/
    const tuple<uint32_t, int32_t, bool> _match_comment_block(const string& input, const uint32_t start_index, const int32_t initial_indent, uint32_t& line_number) {
        uint32_t comment_index;
        const uint32_t input_size = input.size();
        bool inline_comm = true;
        uint32_t indent_count = initial_indent;
        uint32_t comment_line_num = line_number;

//      A comment block ends with two characters, so stop if the penultimate character is not part of the comment block token.
        for (comment_index = start_index; comment_index < input_size - 1; comment_index++) {
//          On a newline, reset the indent count to 0.
            if (input[comment_index] == NEWLINE_TOKEN) {
                indent_count = 0;
                inline_comm = false;

                comment_line_num++;

//          On a tab character, increase indent by the tab character.
            } else if (input[comment_index] == '\t') {
                indent_count += TAB_WIDTH;

//          On any character other than '#', continue incrementing.
            } else if (input[comment_index] != INLINE_COMMENT_TOKEN) {
                indent_count++;

//          On this condition, the current character is guaranteed to be '#' by the previous if-statement.
//          So, check if the next character is also '#', making "##".
//          Note, the for-loop condition ensures that the next index is also in range.
            } else if (input[comment_index + 1] == INLINE_COMMENT_TOKEN) {
                line_number = comment_line_num;
                return make_tuple(comment_index + 2, indent_count + 2, inline_comm);

            } else {
                indent_count++;
            }
        }

//      Throw an error if the string was exhausted of characters before "##" was found.
        throw UnexpectedInputError("unclosed comment at end of file", comment_line_num);
    }

/*
    Compute the index of the first non-trivia character after the given start index in the given input string.
    Trivia is defined in the is_trivia macro as most whitespace characters as well as comments.
    Update the current line number counter when a newline is passed.

    This function assumes that the given start index represents the beginning of a new line, i.e. the initial indent is 0.
    It also assumes that the amount of trivia plus the given start index will not overflow an unsigned 32-bit integer, 
    i.e. the computed index will not reach 4,294,967,296.

    Throw an exception if an unclosed comment block is found, i.e. the number of "##" sequences is odd after the given start index.

    Parameters:
        input: string to match trivia in (input)
        start_index: first index in the input string to check for trivia characters (input)
        line_number: line number counter to increment when necessary (output)

    Return a pair containing:
        first: the index after the final sequential non-trivia character in the input string from the start index
        second: the amount of indent in the final line of trivia characters
*/
    const pair<uint32_t, int32_t> _match_multiline_trivia(const string& input, const uint32_t start_index, uint32_t& line_number) {
        uint32_t trivia_index;
        const uint32_t input_size = input.size();
        int32_t indent_count = 0;

        for (trivia_index = start_index; 
                (trivia_index < input_size) && 
//                  Check for trivia characters or the newline charater.
                    (_is_trivia(input[trivia_index]) || 
                    (input[trivia_index] == NEWLINE_TOKEN)); 
            trivia_index++) {

//          On a tab character, increase indent by the tab character.
            if (input[trivia_index] == '\t') {
                indent_count += TAB_WIDTH;

            } else if (input[trivia_index] == ' ') {
                indent_count++;

            } else if (input[trivia_index] == INLINE_COMMENT_TOKEN) {
//              Check the next index for '#', which would indicate a comment block.
                if ((trivia_index + 1 < input_size) && input[trivia_index + 1] == INLINE_COMMENT_TOKEN) {
//                  Retrieve the index after the comment block and the indent in the final line of the comment block.
                    tie(trivia_index, indent_count, ignore) = _match_comment_block(input, trivia_index + 2, indent_count + 2, line_number);
//                  The trivia index will increment at the top of the for-loop, so decrement once to offset the increment.
                    trivia_index -= 1;

//              Inline comment case
                } else {
//                  Ignore all text until the next line.
                    while (input[trivia_index] != NEWLINE_TOKEN) { trivia_index++; }
                    indent_count = 0;
                }

            } else if (input[trivia_index] == NEWLINE_TOKEN) {
                indent_count = 0;
                line_number++;
            }
        }

//      Return the index on the next non-trivia character and the indent up to that character.
        return make_pair(trivia_index, indent_count);
    }

/*
    Check whether the given number string represents a valid 32-bit integer.

    This function assumes that the given number string contains only number characters.

    Throw an exception if the given number string represents an integer too large to be stored with 64-bits.

    Parameters:
        number_str: string to check for 32-bit integer storage compatibility (input)
        line_number: line number of the given number string (input)

    Return true if the given number string represents a valid 32-bit integer or
           false if the given number string represents a 64-bit integer.
*/
    inline const bool _is_int32(const string& number_str, const uint32_t line_number) {
//      Throw an exception if the number cannot be stored with 64-bits.
        try {
            stoll(number_str);

        } catch (out_of_range) {
            throw OverflowError("int magnitude too large: " + number_str, line_number);
        }

//      Otherwise, try to store it in 32-bits.
        try {
            stoi(number_str);

        } catch (out_of_range) {
            return false;
        }

        return true;
    }

/*
    Check whether the given number string represents a valid 32-bit floating-point number.

    This function assumes that the given number string contains at most one '.' and only number characters otherwise.
    It also assumes that the given number string does not start with the '.' character.

    Throw an exception if the given number string represents a floating-point number too large to be stored with 64-bits.

    Parameters:
        number_str: string to check for 32-bit floating-point number storage compatibility (input)
        line_number: line number of the number string (input)

    Return true if the given number string represents a valid 32-bit floating-point number or
           false if the given number string represents a 64-bit floating-point number.
*/
    inline const bool _is_float32(const string& number_str, const uint32_t line_number) {
//      Throw an exception if the number cannot be stored with 64-bits.
        try {
            stod(number_str);

        } catch (out_of_range) {
            if (number_str[0] == '0') {
                throw OverflowError("float magnitude too small: " + number_str, line_number);
            }

            throw OverflowError("float magnitude too big: " + number_str, line_number);
        }

//      Otherwise, try to store it in 32-bits.
        try {
            stof(number_str);

        } catch (out_of_range) {
            return false;
        }

        return true;
    }

/*
    Match and store a number substring in the given input string starting at the given start index.
    A number substring is at most one '.' adjacent to any number of digit characters.
    Determine whether the matched number substring represents a 32-bit or 64-bit number, and whether 
    it represents a floatng-point number or an integer. Also, compute the index of the given input string 
    just after the number substring.

    This function assumes that the character of the input string at the start index is '.' or a digit character.
    It also assumes that the length of the number substring plus the given start index will not overflow 
    an unsigned 32-bit integer, i.e. the computed index will not reach 4,294,967,296.

    Throw an exception if the number substring contains more than one '.' character, or if it represents a number 
    that is too large to store with 64-bits.

    Parameters:
        input: string to match a number substring in (input)
        start_index: first index in the input string of the number substring (input)
        line_number: line number of the number substring (input)

    Return a 3-tuple containing 
        <0>: the index in the input string after the matched number substring
        <1>: the type that the matched number substring represents,
                e.g. "12.5" represents a 32-bit float while "4134215212321525" represents a 64-bit integer.
        <2>: the matched number substring as a string
*/
    const tuple<uint32_t, dataType, string> _match_number(const string& input, const uint32_t start_index, const uint32_t line_number) {
        uint32_t number_index;
        dataType number_type;
        string number_str;
        const uint32_t input_size = input.size();
        bool floating_point = false;

//      Increment as long as the character is '.' or a digit.
        for (number_index = start_index; 
                (number_index < input_size) && 
                    (_is_integer(input[number_index]) || 
                    (input[number_index] == FLOAT_DELIMETER_TOKEN)); 
            number_index++) {

//          Ensure that there is at most one '.' character.
            if (input[number_index] == FLOAT_DELIMETER_TOKEN) {
                if (floating_point) {
                    throw UnrecognizedInputError(input, start_index, line_number);
                } else {
                    floating_point = true;
                }
            }
        }

//      Extract the number string, throw an exception if it is invalid.
        number_str = input.substr(start_index, (number_index - start_index));
        const uint32_t number_size = number_str.size();
        if (number_str == string(1, FLOAT_DELIMETER_TOKEN)) {
            throw UnrecognizedInputError(input, start_index, line_number);
        }

//      Remove unnecessary 0's.
        normalize_number_str(number_str, floating_point);

//      Retrieve the type of the number, throw an exception if it is out of bounds.
        if (floating_point) {
            number_type = _is_float32(number_str, line_number) ? dataType::Float32T : dataType::Float64T;
        } else {
            number_type = _is_int32(number_str, line_number) ? dataType::Int32T : dataType::Int64T;
        }

//      Return the index after the number substring, the number type, and the number substring itself.
        return { number_index, number_type, number_str };
    }

//  Match a custom variable label in the given string at the given index.
//  Store both the index succeeding the label and the label text.
//  This function assumes that the first character at the given index is a label but not a number.
//      input: string of code (input)
//      string_index: the first index to match with a label (input)
//      position_label: the index succeeding the matched label and the label string (output)

/*
    Match a variable/function label substring in the given input string at the given start index.
    A label is defined by the is_label macro and can contian any number of alphanumeric characters as well as '_'.
    Compute the index in the given input string just after the label substring.

    This function assumes that the matched label substring length plus the start index will not overflow an
    unsigned 32-bit integer, i.e. the computed index will not reach 4,294,967,296.

    Parameters:
        input: the string to match a label substring in (input)
        start_index: the first index in the input string to match a label substring (input)

    Return a pair containing 
        first: the index in the input string after the label substring
        second: the label substring as a string
*/
    inline const pair<uint32_t, string> _match_label(const string& input, const uint32_t start_index) noexcept {
        const uint32_t input_size = input.size();
        uint32_t label_index = start_index;

//      Increment as long as the character is a label character.
        while ((label_index < input_size) && _is_label(input[label_index])) { label_index++; }

//      Return the index after the label substring and the substring itself.
        return { label_index, input.substr(start_index, (label_index - start_index)) };
    }

/*
    Append the given token to the end of the given token list.
    Also, compute the resulting indent increase and index increase.

    Parameters:
        new_token: token to append to the token list (input)
        new_index: index after the given token in the string it was extracted from (input)
        start_index: first index of the given token in the string it was extracted from (input) 
        initial_indent: indent at the start of the given token in the string it was extracted from (input)
        token_list: list to append the given token to (input/output)

    Return a pair containing
        first: the index after the given token in the string it was extracted from
        second: the increased indent that resulted from the new token
*/
    inline const pair<uint32_t, int32_t> _add_token(const token& new_token, const uint32_t new_index, const uint32_t start_index, 
                                                    const int32_t initial_indent, list<token>& token_list) noexcept {

            token_list.push_back(new_token);
            return make_pair(new_index, initial_indent + (new_index - start_index));
    }
    
}


const list<token> lex_string(string& input) {
    list<token> token_list;
    uint32_t curr_index, matched_index, trivia_index;
    int32_t curr_indent;
    const uint32_t input_size = input.size();
//  First line is line 1.
    uint32_t line_number = 1;

//  Match any trivia preceding the first input of code.
    tie(curr_index, curr_indent) = _match_multiline_trivia(input, 0, line_number);

//  The global indent is -1, so ensure that the newline token contains a signed value.
//  Since the multiline trivia function returns a signed int, we can safely cast down.
    token_list.push_back(make_tuple(tokenKey::Newline, static_cast<int32_t>(curr_indent), line_number));
    curr_indent = 0;

//  Lex the string until there are no more characters.
    while (curr_index < input_size) {
//      Check for the start of a number substring.
        if (_is_integer(input[curr_index]) || (input[curr_index] == FLOAT_DELIMETER_TOKEN)) {
            token num_token;
            uint32_t num_index;
            dataType number_type;
            string number_str;

//          Retrieve the next index, number size, and number string.
            tie(num_index, number_type, number_str) = _match_number(input, curr_index, line_number);

//          Convert the number string based on type.
            switch (number_type) {
                case dataType::Float32T: {
//                  The string is ensured to be in range of a 32-bit floating-point number. 
//                  It was checked when the number was matched.
                    const float estimated_val = stof(number_str);
                    const double true_val = stod(number_str);

//                  If there is a significant loss in precision, promote a 32-bit float to a 64-bit float.
//                  "significant" is determined by FLOAT_PROMOTION_THRESHOLD in interp_utils.hpp.
                    if (promote_float(true_val, estimated_val)) {
                        num_token = make_tuple(tokenKey::Float64, true_val, line_number);
                    } else {
                        num_token = make_tuple(tokenKey::Float32, estimated_val, line_number);
                    }

                    break;
                }
                
                case dataType::Float64T:
                    num_token = make_tuple(tokenKey::Float64, stod(number_str), line_number);
                    break;
                
                case dataType::Int32T:
//                  The string is ensured to be in range of a 32-bit integer. It was checked when the number was matched.
                    num_token = make_tuple(tokenKey::Int32, static_cast<uint32_t>(stoul(number_str)), line_number);
                    break;
                
                default:
                    num_token = make_tuple(tokenKey::Int64, stoull(number_str), line_number);
                    break;
            }

//          Append the token to the back of the list, calculate the indent increase from the number substring, and update the current index.
            tie(curr_index, curr_indent) = _add_token(num_token, num_index, curr_index, curr_indent, token_list);


//      Match multicharacter tokens using the match target function. In each if-statement,
//      retrieve the index after the given target and compare it to the current index. If the 
//      new index is larger, that target was present. Append the corresponding token, compute 
//      the indent increase and update the current index.
//      Note that these keyword tokens require the third parameter of the match target function to be true, 
//      meaning they must be succeeded with a nonlabel character.
//      Tokens that do not store data have a default value of false at index 1.

        } else if ((matched_index = _match_target(input, ASSIGN_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Assign, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, IF_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::If, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, ELSE_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Else, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, ANDW_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::AndW, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, ORW_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::OrW, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, XORW_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::XorW, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, IS_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Is, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, NOTW_TOKEN, curr_index, true)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::NotW, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, BOOL_TRUE_TOKEN, curr_index, true)) > curr_index) {
//          Include the relevant boolean value in the token.
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Bool, true, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, BOOL_FALSE_TOKEN, curr_index, true)) > curr_index) {
//          Include the relevant boolean value in the token.
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Bool, false, line_number), matched_index, curr_index, curr_indent, token_list);

//      Once keyword matches have been exhausted, attempt to match a variable/function name.
//      Assume this label starts with a letter or '_' since the same index was checked for a digit already.
        }  else if (_is_label(input[curr_index])) {
            token token;
            string label;

//          Retrieve the label and following index.
            tie(matched_index, label) = _match_label(input, curr_index);

//          Include the label in the token.
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Var, label, line_number), matched_index, curr_index, curr_indent, token_list);


//      Continue to attempt matches on multicharacter tokens like before.
//      Note that these non-keyword tokens can be succeeded with any character, 
//      so the third parameter of the match target function is false.

        } else if ((matched_index = _match_target(input, EQUALS_TOKEN, curr_index, false)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Equals, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, EXP_TOKEN, curr_index, false)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Exp, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, XOR_TOKEN, curr_index, false)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Xor, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, LESSEQUAL_TOKEN, curr_index, false)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::LessEqual, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, GREQUAL_TOKEN, curr_index, false)) > curr_index) {
            tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::GrEqual, false, line_number), matched_index, curr_index, curr_indent, token_list);

        } else if ((matched_index = _match_target(input, COMMENT_BLOCK_TOKEN, curr_index, false)) > curr_index) {
            bool inline_comment;

//          Retrieve the succeeding index, indent, and whether the comment was inline.
//          Pass the index and indent values increased by 2 to account for the matched "##" sequence.
            tie(curr_index, curr_indent, inline_comment) = _match_comment_block(input, curr_index + 2, curr_indent + 2, line_number);

//          If the comment went to a new line, insert a newline token with the indentation level of the final line of the comment.
            if (!inline_comment) {
                token_list.push_back(make_tuple(tokenKey::Newline, curr_indent, line_number));
            }
        
        } else {

//          For single-character tokens, switch over the current character of the input string.

            switch (input[curr_index]) {
                case NEWLINE_TOKEN: {
//                  Retrieve the next non-trivia index and the indent of the beginning of that non-trivia line.
//                  Pass the current indent increased by 1 to account for the matched '\n' character. Increment the line number for the same reason.
                    tie(curr_index, curr_indent) = _match_multiline_trivia(input, curr_index + 1, ++line_number);

//                  The global indent is -1, so ensure that the newline token contains a signed value.
//                  Since the multiline trivia function returns a signed int, we can safely cast down.
                    token_list.push_back(make_tuple(tokenKey::Newline, curr_indent, line_number));
                    break;
                }


//              Handle single-character tokens similarly to the multicharacter tokens. 
//              Pass 1 plus the current index as the new index to the add function since the length of a single-character token is always 1.
//              Note, the default data value for tokens that don't store data is false.

                case BIND_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Bind, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;

                case PLUS_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Plus, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case MINUS_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Minus, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case DIV_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Div, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case MULT_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Mult, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case AND_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::And, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
            
                case OR_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Or, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case NOT_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Not, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case GREATER_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Greater, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case LESS_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::Less, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
            
                case LEFTPAR_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::LeftPar, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
                case RIGHTPAR_TOKEN:
                    tie(curr_index, curr_indent) = _add_token(make_tuple(tokenKey::RightPar, false, line_number), curr_index + 1, curr_index, curr_indent, token_list);
                    break;
                
//              Increment to the next line and reset the current indent for an inline comment.
//              Note this index has been checked already for a multiline comment.
                case INLINE_COMMENT_TOKEN:
                    while ((curr_index < input_size) && input[curr_index] != NEWLINE_TOKEN) { curr_index++; }
                    curr_indent = 0;
                    line_number++;
                    break;
                
                default:
//                  Throw an exception on an unrecognized token.
                    throw UnrecognizedInputError(input, curr_index, line_number);
            }
        }

//      Retrieve the index and indent after any inline trivia.
        tie(curr_index, curr_indent) = _match_inline_trivia(input, curr_index, curr_indent, line_number);
    }


//  Every token list ends with a newline containing the global indent as a way to close the global scope.

//  If the final token is already a newline, update its indent to the global indent.
//  Otherwise, add this newline token.
    if (get<0>(token_list.back()) == tokenKey::Newline) {
        token_list.pop_back();
    }

    token_list.push_back(make_tuple(tokenKey::Newline, GLOBAL_INDENT, line_number));

    return token_list;
}
