// File containing internal interpreter debugging utilities.

#include "inc_debug/compiler_debug.hpp"

using std::string, std::to_string, std::get;
using namespace TokenDef;
using namespace TypingUtils;


// Namespace for utilities in debugging and displaying Regal code.
namespace DebugUtils {
//  Store the display string of the given token.
//       disp_token: token to display (input)
//       disp_option: controls the display string for the given token (input)
//       display_str: display string of the token (output)
    void display_token(const token& disp_token, const tokenDispOption disp_option, string& display_str) {
        const tokenKey token_key = get<tokenKey>(disp_token[0]);

        switch (token_key) {
            case tokenKey::Let:
                if (disp_option == tokenDispOption::Subset) {
                    display_str = "<keyword>";
                } else {
                    display_str = "let";
                }
                break;

            case tokenKey::Now:
                if (disp_option == tokenDispOption::Subset) {
                    display_str = "<keyword>";
                } else {
                    display_str = "now";
                }
                break;

            case tokenKey::Int:
                if (disp_option == tokenDispOption::Literal) {
                    int n = get<int>(disp_token[1]);
                    display_str = to_string(n);
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<integer>";
                } else {
                    display_str = "<number>";
                }
                break;

            case tokenKey::Bool:
                if (disp_option == tokenDispOption::Literal) {
                    bool b = get<int>(disp_token[1]);
                    display_str = b ? "true" : "false";
                } else {
                    display_str = "<boolean>";
                }
                break;

            case tokenKey::Plus:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "+";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<additive operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Minus:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "-";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<additive operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Mult:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "*";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<multiplicative operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Div:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "/";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<multiplicative operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Exp:
                if (disp_option == tokenDispOption::Subset) {
                    display_str = "<binary operator>";
                } else {
                    display_str = "**";
                }
                break;
            
            case tokenKey::And:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "&";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::AndW:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "and";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Or:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "|";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::OrW:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "or";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Xor:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "||";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::XorW:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "xor";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Not:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "!";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<unary operator>";
                }
                break;

            case tokenKey::NotW:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "not";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<boolean operator>";
                } else {
                    display_str = "<unary operator>";
                }
                break;

            case tokenKey::Greater:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = ">";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<comparison operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Less:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "<";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<comparison operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Var:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = get<string>(disp_token[1]);
                } else {
                    display_str = "<variable>";
                }
                break;

            case tokenKey::Bind:
                display_str = "=";
                break;

            case tokenKey::Equals:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "==";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<comparison operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::Is:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "is";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<comparison operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;
            
            case tokenKey::GrEqual:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = ">=";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<comparison operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::LessEqual:
                if (disp_option == tokenDispOption::Literal) {
                    display_str = "<=";
                } else if (disp_option == tokenDispOption::Key) {
                    display_str = "<comparison operator>";
                } else {
                    display_str = "<binary operator>";
                }
                break;

            case tokenKey::If:
                if (disp_option == tokenDispOption::Subset) {
                    display_str = "<ternary if>";
                } else {
                    display_str = "if";
                }
                break;

            case tokenKey::Else:
                display_str = "else";
                break;

            case tokenKey::LeftPar:
                display_str = "(";
                break;

            case tokenKey::RightPar:
                display_str = ")";
                break;
            
            case tokenKey::Newline:
                display_str = "<newline>";
                break;

            case tokenKey::Nothing:
                display_str = "<DEBUG NOTHING>";
                break;

            default:
                display_str = "<UNKNWON TOKEN>";
        }

        return;
    }

//  Return the display string of the given token.
//       disp_token: token to display (input)
//       disp_option: controls the display string for the given token (input)
    string display_token(const token& disp_token, const tokenDispOption disp_option) {
        string result;

        display_token(disp_token, disp_option, result);

        return result;
    }

//  Store the display string for the given type.
//      type: type to display (input)
//      display_str: string to store the type's display string (output)
    void display_type(const TypingUtils::dataType type, std::string& display_str) {
        switch (type) {
            case dataType::IntT:
                display_str = "<int>";
                break;
            case dataType::BoolT:
                display_str = "<bool>";
                break;
            default:
                throw FatalError("data type not recognized for display string");
        }

        return;
    }

//  Return the display string for the given type.
//      type: type to display (input)
    string display_type(const TypingUtils::dataType type) {
        string display_str;

        display_type(type, display_str);

        return display_str;
    }
}
