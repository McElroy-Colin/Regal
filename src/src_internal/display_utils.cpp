/*

Implementations for interpreter data display functions.

*/

#include "inc_internal/display_utils.hpp"

// Standard library aliases
using std::string, std::int32_t, std::int64_t, 
      std::to_string, std::get;

// interp_utils namespaces
using namespace InterpreterUtils;
using namespace TypingUtils;
using namespace TokenDef;


const string display_token(const token& disp_token, const bool literal) {
//  Retrieve the token key.
    const tokenKey token_key = get<0>(disp_token);

    string result = "\'";

    switch (token_key) {
        case tokenKey::Assign:
            result += ASSIGN_TOKEN;
        
        case tokenKey::Int32:
            if (literal) {
//              Retrieve the stored 32-bit integer and result +=  its normalized string.
                result += num_to_string<int32_t>(get<uint32_t>(get<1>(disp_token)), false);
            } else {
//              Otherwise, result +=  the string for the relevant type.
                result += display_type(dataType::Int32T, get<2>(disp_token));
            }

            break;

        case tokenKey::Int64:
            if (literal) {
//              Retrieve the stored 64-bit integer and result +=  its normalized string.
                result += num_to_string<int64_t>(get<uint64_t>(get<1>(disp_token)), false);
            } else {
//              Otherwise, result +=  the string for the relevant type.
                result += display_type(dataType::Int64T, get<2>(disp_token));
            }

            break;

        case tokenKey::Float32:
            if (literal) {
//              Retrieve the stored 32-bit float and result +=  its normalized string.
                result += num_to_string<float>(get<float>(get<1>(disp_token)), true);
            } else {
//              Otherwise, result +=  the string for the relevant type.
                result += display_type(dataType::Float32T, get<2>(disp_token));
            }

            break;

        case tokenKey::Float64:
            if (literal) {
//              Retrieve the stored 64-bit float and result +=  its normalized string.
                result += num_to_string<double>(get<double>(get<1>(disp_token)), true);
            } else {
//              Otherwise, result +=  the string for the relevant type.
                result += display_type(dataType::Float64T, get<2>(disp_token));
            }

            break;

        case tokenKey::Bool:
            if (literal) {
//              Retrieve the stored boolean and display its value.
                result += get<bool>(get<1>(disp_token)) ? BOOL_TRUE_TOKEN : BOOL_FALSE_TOKEN;
            } else {
//              Otherwise, result +=  the string for the relevant type.
                result += display_type(dataType::BoolT, get<2>(disp_token));
            }

            break;

        case tokenKey::Plus:
            result += string(1, PLUS_TOKEN);
            break;

        case tokenKey::Minus:
            result += string(1, MINUS_TOKEN);
            break;

        case tokenKey::Mult:
            result += string(1, MULT_TOKEN);
            break;

        case tokenKey::Div:
            result += string(1, DIV_TOKEN);
            break;

        case tokenKey::Exp:
            result += EXP_TOKEN;
            break;
        
        case tokenKey::And:
            result += string(1, AND_TOKEN);
            break;

        case tokenKey::AndW:
            result += ANDW_TOKEN;
            break;

        case tokenKey::Or:
            result += string(1, OR_TOKEN);
            break;

        case tokenKey::OrW:
            result += ORW_TOKEN;
            break;

        case tokenKey::Xor:
            result += XOR_TOKEN;
            break;

        case tokenKey::XorW:
            result += XORW_TOKEN;
            break;

        case tokenKey::Not:
            result += string(1, NOT_TOKEN);
            break;

        case tokenKey::NotW:
            result +=  NOTW_TOKEN;
            break;

        case tokenKey::Greater:
            result += string(1, GREATER_TOKEN);
            break;

        case tokenKey::Less:
            result += string(1, LESS_TOKEN);
            break;

        case tokenKey::Var:
            if (literal) {
//              Retrieve the stored string.
                result += get<string>(get<1>(disp_token));
            } else {
                return "<var>";
            }

            break;

        case tokenKey::Bind:
            result +=  string(1, BIND_TOKEN);
            break;

        case tokenKey::Equals:
            result += EQUALS_TOKEN;
            break;

        case tokenKey::Is:
            result += IS_TOKEN;
            break;

        case tokenKey::GrEqual:
            result += GREQUAL_TOKEN;
            break;

        case tokenKey::LessEqual:
            result += LESSEQUAL_TOKEN;
            break;

        case tokenKey::If:
            result += IF_TOKEN;
            break;
            
        case tokenKey::Else:
            result += ELSE_TOKEN;
            break;
            
        case tokenKey::LeftPar:
            result += string(1, LEFTPAR_TOKEN);
            break;
            
        case tokenKey::RightPar:
            result += string(1, RIGHTPAR_TOKEN);
            break;
            
        case tokenKey::Newline:
            result += "newline";
            break;
            
        case tokenKey::Nothing:
            result += "DEBUG_NOTHING";
            break;
            
        default:
            throw FatalError("token key not recognized for display string", get<2>(disp_token));
    }

    return result + "\'";
}

const string display_type(const dataType type, const uint32_t line_number) {
    switch (type) {
        case dataType::Int32T:
        case dataType::Int64T:
            return "<int>";
            
        case dataType::Float32T:
        case dataType::Float64T:
            return "<float>";
            
        case dataType::BoolT:
            return "<bool>";
            
        default:
            throw FatalError("data type not recognized for display string", line_number);
    }
}
