#include "../inc_langdef/langdef.hpp"

// Debug enum.
enum ReturnValue {
    FAILURE = -1,
};

// Display options for display_token function.
enum DisplayTokenOption {
    Literal,     // display the token as input by the user (e.g. 'let' is 'let', '4' is '4')
    Key,         // display the token's key name (e.g. 'let' is 'let', '4' is '<integer>')
    Subset       // display the set of tokens the token belongs to (e.g. 'let' is '<keyword>', '4' is '<number>')
};

// Store the display string of the given token.
//      token: token to display (input)
//      display: display string of the token (output)
//      disp_option: controls the display string for the given token (input)
void display_token(Token& token, String& display, const DisplayTokenOption disp_option, const bool assignment_operator = true) {
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
                bool b = std::get<bool>(token[1]);
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
            display = "<variable>";
            break;

        case TokenKey::Equals:
            if (assignment_operator || (disp_option == Literal)) {
                display = "=";
            } else if (disp_option == Key) {
                display = "<comparison operator>";
            } else {
                display = "<binary operator>";
            }

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

