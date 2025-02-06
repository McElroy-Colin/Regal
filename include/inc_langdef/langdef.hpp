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

using String = std::string;

// Enum for all tokens in Regal.
enum TokenKey {
//  Keywords
    Let, Now, 

//  Primitive data types
    Int, Bool,

//  Unary Operators
    Not, NotW,

//  Binary Operators
    Plus, Minus, Mult, Div, Exp, And, AndW, Or, OrW, Xor, XorW,  Greater, Less, Equals, Is,

//  Ternary Operators
    If, Else,

//  Variables
    Var,
    Bind,

//  Miscellaneous
    LeftPar, RightPar, Whitespace, Newline, 

//  Nothing token for debug purposes.
    Nothing
};

// Useful subsets of tokens.
std::vector<TokenKey> keyword_tokens = {Let, Now};
std::vector<TokenKey> number_tokens = {Int};


// Token aliases.
using Token = std::vector<std::variant<TokenKey, int, String>>;

// AST node definitions.
struct CodeBlock;

struct Integer;
struct Boolean;
struct Variable;
struct UnaryOperator;
struct BinaryOperator;
struct TernaryOperator;
struct Assign;
struct Reassign;

// Variant using shared pointers to allow the AST to have recursive, type-safe nodes.
using Action = std::variant<
    std::shared_ptr<CodeBlock>,

    std::shared_ptr<Integer>, 
    std::shared_ptr<Boolean>, 
    std::shared_ptr<Variable>, 
    std::shared_ptr<UnaryOperator>, 
    std::shared_ptr<BinaryOperator>, 
    std::shared_ptr<TernaryOperator>, 
    std::shared_ptr<Assign>, 
    std::shared_ptr<Reassign>
>;

// Display options for pieces of data.
enum DisplayOption {
    Literal,     // display as input by the user (e.g. 'let' is 'let', '4' is '4')
    Key,         // display key name (e.g. 'let' is 'let', '4' is '<integer>')
    Subset       // display the set of tokens the data belongs to (e.g. 'let' is '<keyword>', '4' is '<number>')
};

// Store the display string of the given token.
//      token: token to display (input)
//      disp_option: controls the display string for the given token (input)
//      display: display string of the token (output)
//      assignment_operator: true if '=' is used to assign variables not compare values (input)
void display_token(const Token& token, const DisplayOption disp_option, String& display) {
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

        case TokenKey::AndW:
            if (disp_option == Literal) {
                display = "and";
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

        case TokenKey::OrW:
            if (disp_option == Literal) {
                display = "or";
            } else if (disp_option == Key) {
                display = "<boolean operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Xor:
            if (disp_option == Literal) {
                display = "||";
            } else if (disp_option == Key) {
                display = "<boolean operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::XorW:
            if (disp_option == Literal) {
                display = "xor";
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

        case TokenKey::NotW:
            if (disp_option == Literal) {
                display = "not";
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

        case TokenKey::Bind:
            display = "=";
            break;

        case TokenKey::Equals:
            if (disp_option == Literal) {
                display = "==";
            } else if (disp_option == Key) {
                display = "<comparison operator>";
            } else {
                display = "<binary operator>";
            }
            break;

        case TokenKey::Is:
            if (disp_option == Literal) {
                display = "is";
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
        
        case TokenKey::Newline:
            display = "<newline>";
            break;

        default:
            display = "<UNKNWON TOKEN>";
    }

    return;
}

// Return value output overload of the above function.
String display_token(const Token& token, const DisplayOption disp_option) {
    String result;

    display_token(token, disp_option, result);

    return result;
}


// Parent struct for all data.
struct Data {
    protected:
        virtual String disp(const DisplayOption option) const = 0;
        virtual ~Data() = default;
};

// Struct for a block of code
struct CodeBlock : Data {
    private:
        Action curr_operation, remainder;
        int depth;

    public:
        CodeBlock() : curr_operation(), remainder(), depth(0) {}
        CodeBlock(Action& curr_op, Action& rem, int d) 
            : curr_operation(curr_op), remainder(rem), depth(d) {}
        CodeBlock(const CodeBlock&& other)
            : curr_operation(std::move(other.curr_operation)),
            remainder(std::move(other.remainder)), depth(other.depth) {}

        Action& get_operation() {
            return curr_operation;
        }

        Action& get_remainder() {
            return remainder;
        }

        int get_depth() {
            return depth;
        }

        String disp(const DisplayOption option) const override {
            return std::visit([](const auto& d) { return d->disp(Literal); }, curr_operation);
        }

};

// Struct for an Integer node.
struct Integer : Data {
    private:
        int number;

    public:
        Integer() : number(0) {}
        Integer(int n) : number(n) {}
        Integer(const Integer& other) : number(other.number) {}
        
        int get_number() {
            return number;
        }

        String disp(const DisplayOption option) const override {
            switch (option) {
                case Literal:
                    return std::to_string(number);
                case Key:
                    return "<integer>";
                default:
                    return "<number>";
            }
        }
};

// Struct for a Boolean node.
struct Boolean : Data {
    private:
        bool boolean;

    public:
        Boolean() : boolean(false) {}
        Boolean(bool b) : boolean(b) {}
        Boolean(const Boolean& other) : boolean(other.boolean) {}

        bool get_boolean() {
            return boolean;
        }

        String disp(const DisplayOption option) const override {
            switch (option) {
                case Literal:
                    return boolean ? "true" : "false";
                default:
                    return "<boolean>";
            } 
        }
};

// Struct for a Variable node.
struct Variable : Data {
    private:
        String variable;

    public:
        Variable() : variable("") {}
        Variable(String var) : variable(var) {}
        Variable(Variable&& other) noexcept : variable(std::move(other.variable)) {}

        String& get_variable() {
            return variable;
        }

        String disp(const DisplayOption option) const override {
            switch (option) {
                case Literal:
                    return variable;
                default:
                    return "<variable>";
            }
        }
};

/*
// Struct for whitespace data.
struct Whitespace : Data {
    private:
        String whitespace;
    
    public:
        Whitespace() : whitespace(""){}
        Whitespace(String w) : whitespace(w) {}
        Whitespace(Whitespace&& other) noexcept : whitespace(std::move(other.whitespace)) {}

        String& get_whitespace() {
            return whitespace;
        }

        String disp(const DisplayOption option) const override {
            return "";
        }
};
*/

// Struct for a unary operator node.
struct UnaryOperator : Data {
    private:
        TokenKey op;
        Action expression;

    public:
        UnaryOperator() : op(Nothing), expression() {}
        UnaryOperator(TokenKey op, Action e) : op(op), expression(e) {}
        UnaryOperator(UnaryOperator&& other) noexcept : op(other.op),
            expression(std::move(other.expression)) {}
    
        Action& get_expression() {
            return expression;
        }

        TokenKey get_op() {
            return op;
        }

        String disp(const DisplayOption option) const override {
            switch (option) {
                case Literal:
                    return display_token({op}, Literal);
                default:
                    return "<unary operator>";
            }
        }
};

// Struct for a binary operator node.
struct BinaryOperator : Data {
    private:
        TokenKey op;
        Action expression1, expression2;

    public:
        BinaryOperator() : op(Nothing), expression1(), expression2() {}
        BinaryOperator(TokenKey op, Action e1, Action e2) : op(op), expression1(e1), expression2(e2) {}
        BinaryOperator(BinaryOperator&& other) noexcept : op(other.op),
            expression1(std::move(other.expression1)),
            expression2(std::move(other.expression2)) {}

        Action& get_expression1() {
            return expression1;
        }

        Action& get_expression2() {
            return expression2;
        }

        TokenKey get_op() {
            return op;
        }

        String disp(const DisplayOption option) const override {
            switch (option) {
                case Literal:
                    return display_token({op}, Literal);
                default:
                    return "<binary operator>";
            }
        }
};

// Struct for a ternary operator node.
struct TernaryOperator : Data {
    private:
        TokenKey op;
        Action expression1, expression2, expression3;

    public:
        TernaryOperator() : op(Nothing), expression1(), expression2(), expression3() {}
        TernaryOperator(TokenKey op, Action e1, Action e2, Action e3) 
            : op(op), expression1(e1), expression2(e2), expression3(e3) {}
        TernaryOperator(TernaryOperator&& other) noexcept : op(other.op),
            expression1(std::move(other.expression1)),
            expression2(std::move(other.expression2)),
            expression3(std::move(other.expression3)) {}

        Action& get_expression1() {
            return expression1;
        }

        Action& get_expression2() {
            return expression2;
        }

        Action& get_expression3() {
            return expression3;
        }

        TokenKey get_op() {
            return op;
        }

        String disp(const DisplayOption option) const override {
            switch (option) {
                case Literal:
                    return display_token({op}, Literal);
                default:
                    return "<ternary operator>";
            }
        }
};

// Struct for an assignment node.
struct Assign : Data {
    private:
        String variable;
        Action expression;

    public:
        Assign() : variable(""), expression() {}
        Assign(const String& v, const Action& e) : variable(v), expression(e) {}
        Assign(Assign&& other) : variable(std::move(other.variable)), 
            expression(std::move(other.expression)) {}
    
        String& get_variable() {
            return variable;
        }

        Action& get_expression() {
            return expression;
        }

        String disp(const DisplayOption option = Literal) const override {
            return "<assignment>";
        }
};

// Struct for a reassignment node.
struct Reassign : Data {
    private:
        String variable;
        Action expression;
        bool implicit;

    public:
        Reassign() : variable(""), expression(), implicit(false) {}
        Reassign(String v, Action e, bool i) : variable(v), expression(e), implicit(i) {}
        Reassign(Reassign&& other) : variable(std::move(other.variable)), 
            expression(std::move(other.expression)), implicit(other.implicit) {}

        String& get_variable() {
            return variable;
        }

        Action& get_expression() {
            return expression;
        }

        bool get_implicit() {
            return implicit;
        }

        String disp(const DisplayOption option = Literal) const override {
            return implicit ? "<implicit reassignment>" : "<reassignment>";
        }
};

#endif