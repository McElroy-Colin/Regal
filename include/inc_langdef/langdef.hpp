// Header file containing aliases and values for language definition.

#include <string>
#include <vector>
#include <variant>
#include <list>
#include <memory>
#include <typeindex>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <utility>

#ifndef LANGDEF_HPP
#define LANGDEF_HPP


// Namespace for utilities in debugging and displaying Regal code.
namespace DebugUtils {
    
    //  Display options for pieces of data.
        enum tokenDispOption {
            Literal,     // display as input by the user (e.g. 'let' is 'let', '4' is '4')
            Key,         // display key name (e.g. 'let' is 'let', '4' is '<integer>')
            Subset       // display the set of tokens the data belongs to (e.g. 'let' is '<keyword>', '4' is '<number>')
        };
    
    //  Store the display std::string of the given token.
    //       disp_token: disp_token to display (input)
    //       disp_option: controls the display std::string for the given token (input)
    //       display: display std::string of the token (output)
    //       assignment_operator: true if '=' is used to assign variables not compare values (input)
        void display_token(const TokenDef::token& disp_token, const tokenDispOption disp_option, std::string& display) {
            const TokenDef::tokenKey token_key = std::get<TokenDef::tokenKey>(disp_token[0]);
    
            switch (token_key) {
                case TokenDef::tokenKey::Let:
                    if (disp_option == Subset) {
                        display = "<keyword>";
                    } else {
                        display = "let";
                    }
                    break;
    
                case TokenDef::tokenKey::Now:
                    if (disp_option == Subset) {
                        display = "<keyword>";
                    } else {
                        display = "now";
                    }
                    break;
    
                case TokenDef::tokenKey::Int:
                    if (disp_option == Literal) {
                        int n = std::get<int>(disp_token[1]);
                        display = std::to_string(n);
                    } else if (disp_option == Key) {
                        display = "<integer>";
                    } else {
                        display = "<number>";
                    }
                    break;
    
                case TokenDef::tokenKey::Bool:
                    if (disp_option == Literal) {
                        bool b = std::get<int>(disp_token[1]);
                        display = b ? "true" : "false";
                    } else {
                        display = "<boolean>";
                    }
                    break;
    
                case TokenDef::tokenKey::Plus:
                    if (disp_option == Literal) {
                        display = "+";
                    } else if (disp_option == Key) {
                        display = "<additive operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Minus:
                    if (disp_option == Literal) {
                        display = "-";
                    } else if (disp_option == Key) {
                        display = "<additive operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Mult:
                    if (disp_option == Literal) {
                        display = "*";
                    } else if (disp_option == Key) {
                        display = "<multiplicative operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Div:
                    if (disp_option == Literal) {
                        display = "/";
                    } else if (disp_option == Key) {
                        display = "<multiplicative operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Exp:
                    if (disp_option == Subset) {
                        display = "<binary operator>";
                    } else {
                        display = "**";
                    }
                    break;
                
                case TokenDef::tokenKey::And:
                    if (disp_option == Literal) {
                        display = "&";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::AndW:
                    if (disp_option == Literal) {
                        display = "and";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Or:
                    if (disp_option == Literal) {
                        display = "|";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::OrW:
                    if (disp_option == Literal) {
                        display = "or";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Xor:
                    if (disp_option == Literal) {
                        display = "||";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::XorW:
                    if (disp_option == Literal) {
                        display = "xor";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Not:
                    if (disp_option == Literal) {
                        display = "!";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<unary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::NotW:
                    if (disp_option == Literal) {
                        display = "not";
                    } else if (disp_option == Key) {
                        display = "<boolean operator>";
                    } else {
                        display = "<unary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Greater:
                    if (disp_option == Literal) {
                        display = ">";
                    } else if (disp_option == Key) {
                        display = "<comparison operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Less:
                    if (disp_option == Literal) {
                        display = "<";
                    } else if (disp_option == Key) {
                        display = "<comparison operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Var:
                    if (disp_option == Literal) {
                        display = std::get<std::string>(disp_token[1]);
                    } else {
                        display = "<variable>";
                    }
                    break;
    
                case TokenDef::tokenKey::Bind:
                    display = "=";
                    break;
    
                case TokenDef::tokenKey::Equals:
                    if (disp_option == Literal) {
                        display = "==";
                    } else if (disp_option == Key) {
                        display = "<comparison operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::Is:
                    if (disp_option == Literal) {
                        display = "is";
                    } else if (disp_option == Key) {
                        display = "<comparison operator>";
                    } else {
                        display = "<binary operator>";
                    }
                    break;
    
                case TokenDef::tokenKey::LeftPar:
                    display = "(";
                    break;
    
                case TokenDef::tokenKey::RightPar:
                    display = ")";
                    break;
    
                case TokenDef::tokenKey::Nothing:
                    display = "nothing";
                    break;
                
                case TokenDef::tokenKey::Newline:
                    display = "<newline>";
                    break;
    
                default:
                    display = "<UNKNWON TOKEN>";
            }
    
            return;
        }
    
        // Return value output overload of the above function.
        std::string display_token(const TokenDef::token& disp_token, const tokenDispOption disp_option) {
            std::string result;
    
            display_token(disp_token, disp_option, result);
    
            return result;
        }
    }


// Namespace for definition of individual token data for lexing and parsing Regal code.
namespace TokenDef {
//  Enum for all tokens in Regal.
    enum tokenKey {
//          Keywords
            Let, Now, 
        
//          Irreducible data types
            Int, Bool,
        
//          Unary Operators
            Not, NotW,
        
//          Binary Operators
            Plus, Minus, Mult, Div, Exp, And, AndW, Or, OrW, Xor, XorW,  Greater, Less, Equals, Is,
        
//          Ternary Operators
            If, Else,
        
//          Variables
            Var,
            Bind,
        
//          Miscellaneous
            LeftPar, RightPar, Newline, 
        
//          Nothing token for debug purposes.
            Nothing
        };

//  Token aliases.
    using token = std::vector<std::variant<tokenKey, int, std::string>>;

//  Useful subsets of tokens.
    std::vector<tokenKey> number_tokens = {Int};
}

// Namespace for syntax tree data and definitoions for parsing and executing Regal code.
namespace CodeTree {
//  AST syntaxNode definitions.
    struct codeBlock;
    struct ifBlock;

    struct intContainer;
    struct boolContainer;
    struct varContainer;
    struct unaryOp;
    struct binaryOp;
    struct ternaryOp;
    struct assignOp;
    struct reassignOp;

//  Variant using shared pointers to allow the AST to have recursive, type-safe nodes.
    using syntaxNode = std::variant<
        std::shared_ptr<codeBlock>,
        std::shared_ptr<ifBlock>,

        std::shared_ptr<intContainer>, 
        std::shared_ptr<boolContainer>, 
        std::shared_ptr<varContainer>, 
        std::shared_ptr<unaryOp>, 
        std::shared_ptr<binaryOp>, 
        std::shared_ptr<ternaryOp>, 
        std::shared_ptr<assignOp>, 
        std::shared_ptr<reassignOp>
    >;

//  Parent struct for all data.
    struct Data {
        protected:
            virtual std::string disp(const DebugUtils::tokenDispOption option) const = 0;
            virtual ~Data() = default;
    };


//  Parent struct for irreducible data values.
    struct IrreducibleData : Data {
        protected:
            virtual ~IrreducibleData() = default;
    };

//  Struct for a block of code
    struct codeBlock : Data {
        private:
            syntaxNode curr_operation, remainder;

        public:
            codeBlock() : curr_operation(), remainder() {}
            codeBlock(syntaxNode& curr_op, syntaxNode& rem) 
                : curr_operation(curr_op), remainder(rem) {}
            codeBlock(const codeBlock&& other)
                : curr_operation(std::move(other.curr_operation)),
                remainder(std::move(other.remainder)) {}

            syntaxNode& get_operation() {
                return curr_operation;
            }

            syntaxNode& get_remainder() {
                return remainder;
            }

            std::string disp(const DebugUtils::tokenDispOption option) const override {
                return std::visit([](const auto& d) { return d->disp(DebugUtils::Literal); }, curr_operation);
            }

    };

//  Struct for an if-else block of code.
    struct ifBlock : Data {
        private:
            syntaxNode bool_condition, if_block, else_block;
            bool contains_else;

        public:
            ifBlock() : bool_condition(), if_block(), else_block(), contains_else(false) {}
            ifBlock(syntaxNode& b, syntaxNode& cb) : bool_condition(b), if_block(cb), else_block(), contains_else(false) {}
            ifBlock(syntaxNode& b, syntaxNode& cb, syntaxNode& eb) : bool_condition(b), if_block(cb), else_block(eb), contains_else(true) {}
            ifBlock(const ifBlock&& other) 
                : bool_condition(std::move(other.bool_condition)), 
                if_block(std::move(other.if_block)), 
                else_block(std::move(other.else_block)), contains_else(other.contains_else) {}

            syntaxNode& get_condition() {
                return bool_condition;
            }

            syntaxNode& get_if_block() {
                return if_block;
            }

            syntaxNode& get_else_block() {
                return else_block;
            }

            bool has_else() {
                return contains_else;
            }
            
            // TODO: Fix this function or redo displays.
            std::string disp(const DebugUtils::tokenDispOption option) const override {
                //return std::visit([](const auto& d) { return d->disp(Literal); }, bool_condition);
            }
    };

//  Struct for an intContainer syntaxNode.
    struct intContainer : IrreducibleData {
        private:
            int number;

        public:
            intContainer() : number(0) {}
            intContainer(int n) : number(n) {}
            intContainer(const intContainer& other) : number(other.number) {}
            
            int get_number() {
                return number;
            }

            std::string disp(const DebugUtils::tokenDispOption option) const override {
                switch (option) {
                    case DebugUtils::Literal:
                        return std::to_string(number);
                    case DebugUtils::Key:
                        return "<integer>";
                    default:
                        return "<number>";
                }
            }
    };

//  Struct for a boolContainer syntaxNode.
    struct boolContainer : IrreducibleData {
        private:
            bool boolean;

        public:
            boolContainer() : boolean(false) {}
            boolContainer(bool b) : boolean(b) {}
            boolContainer(const boolContainer& other) : boolean(other.boolean) {}

            bool get_boolean() {
                return boolean;
            }

            std::string disp(const DebugUtils::tokenDispOption option) const override {
                switch (option) {
                    case DebugUtils::Literal:
                        return boolean ? "true" : "false";
                    default:
                        return "<boolean>";
                } 
            }
    };

//  Struct for a varContainer syntaxNode.
    struct varContainer : Data {
        private:
            std::string variable;

        public:
            varContainer() : variable("") {}
            varContainer(std::string var) : variable(var) {}
            varContainer(varContainer&& other) noexcept : variable(std::move(other.variable)) {}

            std::string& get_variable() {
                return variable;
            }

            std::string disp(const DebugUtils::tokenDispOption option) const override {
                switch (option) {
                    case DebugUtils::Literal:
                        return variable;
                    default:
                        return "<variable>";
                }
            }
    };

//  Struct for a unary operator syntaxNode.
    struct unaryOp : Data {
        private:
            TokenDef::tokenKey op;
            syntaxNode expression;

        public:
            unaryOp() : op(TokenDef::Nothing), expression() {}
            unaryOp(TokenDef::tokenKey op, syntaxNode e) : op(op), expression(e) {}
            unaryOp(unaryOp&& other) noexcept : op(other.op),
                expression(std::move(other.expression)) {}
        
            syntaxNode& get_expression() {
                return expression;
            }

            TokenDef::tokenKey get_op() {
                return op;
            }

            std::string disp(const DebugUtils::tokenDispOption option) const override {
                switch (option) {
                    case DebugUtils::Literal:
                        return display_token({op}, DebugUtils::Literal);
                    default:
                        return "<unary operator>";
                }
            }
    };

//  Struct for a binary operator syntaxNode.
    struct binaryOp : Data {
        private:
            TokenDef::tokenKey op;
            syntaxNode expression1, expression2;

        public:
            binaryOp() : op(TokenDef::Nothing), expression1(), expression2() {}
            binaryOp(TokenDef::tokenKey op, syntaxNode e1, syntaxNode e2) : op(op), expression1(e1), expression2(e2) {}
            binaryOp(binaryOp&& other) noexcept : op(other.op),
                expression1(std::move(other.expression1)),
                expression2(std::move(other.expression2)) {}

            syntaxNode& get_expression1() {
                return expression1;
            }

            syntaxNode& get_expression2() {
                return expression2;
            }

            TokenDef::tokenKey get_op() {
                return op;
            }

            std::string disp(const DebugUtils::tokenDispOption option) const override {
                switch (option) {
                    case DebugUtils::Literal:
                        return display_token({op}, DebugUtils::Literal);
                    default:
                        return "<binary operator>";
                }
            }
    };

//  Struct for a ternary operator syntaxNode.
    struct ternaryOp : Data {
        private:
        TokenDef::tokenKey op;
            syntaxNode expression1, expression2, expression3;

        public:
            ternaryOp() : op(TokenDef::Nothing), expression1(), expression2(), expression3() {}
            ternaryOp(TokenDef::tokenKey op, syntaxNode e1, syntaxNode e2, syntaxNode e3) 
                : op(op), expression1(e1), expression2(e2), expression3(e3) {}
            ternaryOp(ternaryOp&& other) noexcept : op(other.op),
                expression1(std::move(other.expression1)),
                expression2(std::move(other.expression2)),
                expression3(std::move(other.expression3)) {}

            syntaxNode& get_expression1() {
                return expression1;
            }

            syntaxNode& get_expression2() {
                return expression2;
            }

            syntaxNode& get_expression3() {
                return expression3;
            }

            TokenDef::tokenKey get_op() {
                return op;
            }

            std::string disp(const DebugUtils::tokenDispOption option) const override {
                switch (option) {
                    case DebugUtils::Literal:
                        return display_token({op}, DebugUtils::Literal);
                    default:
                        return "<ternary operator>";
                }
            }
    };

//  Struct for an assignment syntaxNode.
    struct assignOp : Data {
        private:
            std::string variable;
            syntaxNode expression;

        public:
            assignOp() : variable(""), expression() {}
            assignOp(const std::string& v, const syntaxNode& e) : variable(v), expression(e) {}
            assignOp(assignOp&& other) : variable(std::move(other.variable)), 
                expression(std::move(other.expression)) {}
        
            std::string& get_variable() {
                return variable;
            }

            syntaxNode& get_expression() {
                return expression;
            }

            std::string disp(const DebugUtils::tokenDispOption option = DebugUtils::Literal) const override {
                return "<assignment>";
            }
    };

//  Struct for a reassignment syntaxNode.
    struct reassignOp : Data {
        private:
            std::string variable;
            syntaxNode expression;
            bool implicit;

        public:
            reassignOp() : variable(""), expression(), implicit(false) {}
            reassignOp(std::string v, syntaxNode e, bool i) : variable(v), expression(e), implicit(i) {}
            reassignOp(reassignOp&& other) : variable(std::move(other.variable)), 
                expression(std::move(other.expression)), implicit(other.implicit) {}

            std::string& get_variable() {
                return variable;
            }

            syntaxNode& get_expression() {
                return expression;
            }

            bool get_implicit() {
                return implicit;
            }

            std::string disp(const DebugUtils::tokenDispOption option = DebugUtils::Literal) const override {
                return implicit ? "<implicit reassignment>" : "<reassignment>";
            }
    };
}

#endif