// Header file containing useful code for meta elements of compiler source code.

#include "../inc_langdef/langdef.hpp"

#ifndef META_CODE_HPP
#define META_CODE_HPP

// Debug values.
enum ReturnValue {
    FAILURE = -1,
};

// Display options for display_token function.
enum DisplayOption {
    Literal,     // display as input by the user (e.g. 'let' is 'let', '4' is '4')
    Key,         // display key name (e.g. 'let' is 'let', '4' is '<integer>')
    Subset       // display the set of tokens the input belongs to (e.g. 'let' is '<keyword>', '4' is '<number>')
};

// Store the display string of the given token.
//      token: token to display (input)
//      disp_option: controls the display string for the given token (input)
//      display: display string of the token (output)
//      assignment_operator: true if '=' is used to assign variables not compare values (input)
void display_token(const Token& token, const DisplayOption disp_option, String& display, const bool assignment_operator = true) {
    const TokenKey token_key = std::get<TokenKey>(token[0]);

    switch (token_key) {
        case TokenKey::Let:
            if (disp_option == Subset) {
                display = "<keyword>";
            } else {
                display = "let";
            }
            break;

        case TokenKey::Now:
            if (disp_option == Subset) {
                display = "<keyword>";
            } else {
                display = "now";
            }
            break;

        case TokenKey::Int:
            if (disp_option == Literal) {
                int n = std::get<int>(token[1]);
                display = std::to_string(n);
            } else if (disp_option == Key) {
                display = "<integer>";
            } else {
                display = "<number>";
            }
            break;

        case TokenKey::Bool:
            if (disp_option == Literal) {
                bool b = std::get<int>(token[1]);
                display = b ? "true" : "false";
            } else {
                display = "<boolean>";
            }
            break;

        case TokenKey::Plus:
            if (disp_option == Literal) {
                display = "+";
            } else if (disp_option == Key) {
                display = "<additive operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Minus:
            if (disp_option == Literal) {
                display = "-";
            } else if (disp_option == Key) {
                display = "<additive operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Mult:
            if (disp_option == Literal) {
                display = "*";
            } else if (disp_option == Key) {
                display = "<multiplicative operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Div:
            if (disp_option == Literal) {
                display = "/";
            } else if (disp_option == Key) {
                display = "<multiplicative operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Exp:
            if (disp_option == Subset) {
                display = "<binary operator>";
            } else {
                display = "**";
            }
            break;
        
        case TokenKey::And:
            if (disp_option == Literal) {
                display = "&";
            } else if (disp_option == Key) {
                display = "<boolean operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Or:
            if (disp_option == Literal) {
                display = "|";
            } else if (disp_option == Key) {
                display = "<boolean operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Not:
            if (disp_option == Literal) {
                display = "!";
            } else if (disp_option == Key) {
                display = "<boolean operator>";
            } else {
                display = "<unary operator>";
            }
            break;

        case TokenKey::Greater:
            if (disp_option == Literal) {
                display = ">";
            } else if (disp_option == Key) {
                display = "<comparison operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Less:
            if (disp_option == Literal) {
                display = "<";
            } else if (disp_option == Key) {
                display = "<comparison operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Var:
            if (disp_option == Literal) {
                display = std::get<String>(token[1]);
            } else {
                display = "<variable>";
            }
            break;

        case TokenKey::Equals:
            if (assignment_operator || (disp_option == Literal)) {
                display = "=";
            } else if (disp_option == Key) {
                display = "<comparison operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::LeftPar:
            display = "(";
            break;

        case TokenKey::RightPar:
            display = ")";
            break;

        case TokenKey::Nothing:
            display = "nothing";
            break;

        default:
            display = "<UNKNWON TOKEN>";
    }

    return;
}

// Return value output overload of the above function.
String display_token(const Token& token, const DisplayOption disp_option, const bool assignment_operator = true) {
    String result;

    display_token(token, disp_option, result, assignment_operator);

    return result;
}

// Store the display string of the given action.
//      action: action to display (input)
//      disp_option: controls the display string for the given action (input)
//      display: display string of the action (output)
//      assignment_operator: true if '=' is used to assign variables not compare values (input)
void display_action(const Action& action, const DisplayOption disp_option, String& display) {
    if (std::holds_alternative<std::shared_ptr<Integer>>(action)) {
        if (disp_option == Literal) {
            std::shared_ptr<Integer> integer = std::get<std::shared_ptr<Integer>>(action);
            display = std::to_string(integer->number);
        } else if (disp_option == Key) {
            display = "<integer>";
        } else {
            display = "<number>";
        }
    } else if (std::holds_alternative<std::shared_ptr<Boolean>>(action)) {
        if (disp_option == Literal) {
            std::shared_ptr<Boolean> bool_action = std::get<std::shared_ptr<Boolean>>(action);
            display = bool_action->boolean ? "true" : "false";
        } else {
            display = "<boolean>";
        }
    } else if (std::holds_alternative<std::shared_ptr<Variable>>(action)) {
        if (disp_option == Literal) {
            std::shared_ptr<Variable> variable = std::get<std::shared_ptr<Variable>>(action);
            display = variable->variable;
        } else {
            display = "<variable>";
        }
    } else {
        display = "<NON PRIMITIVE>";
    }

    return;
}

// Return value output overload of the above function.
String display_action(const Action& action, const DisplayOption disp_option) {
    String result;

    display_action(action, disp_option, result);

    return result;
}

#endif