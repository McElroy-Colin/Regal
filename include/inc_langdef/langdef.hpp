// Header file containing aliases and structures for general interpreter language definition.

#ifndef LANGDEF_HPP
#define LANGDEF_HPP

#include <string>
#include <array>
#include <vector>
#include <variant>
#include <memory>
#include <utility>

// Forward declaration of debugging namespace.
// Namespace for utilities in debugging and displaying Regal code.
namespace DebugUtils {
    enum class tokenDispOption;
}

namespace TypingUtils {
//  Data types in Regal.
    enum class dataType {
        IntT, BoolT
    };

    constexpr std::size_t number_type_count = 1;
}

// Namespace for definition of individual token data for lexing and parsing Regal code.
namespace TokenDef {
//  Enum for all tokens in Regal.
    enum class tokenKey {
//          Keywords
            Let, Now, 
        
//          Irreducible data types
            Int, Bool,
        
//          Unary Operators
            Not, NotW,
        
//          Binary Operators
            Plus, Minus, Mult, Div, Exp, And, AndW, Or, OrW, Xor, XorW, 
            Greater, Less, Equals, Is, GrEqual, LessEqual,
        
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
    using tokenData = std::variant<tokenKey, int, std::string>;
    using token = std::vector<tokenData>;

//  Useful subsets of tokens.
    constexpr std::size_t numeric_comp_op_count = 4;
    extern std::array<tokenKey, TypingUtils::number_type_count> number_tokens;
}

// Namespace for syntax tree data and definitions for parsing and executing Regal code.
namespace CodeTree {
//  Parent class for all data.
    class dataNode {
        public:
            virtual ~dataNode() = default;
    };

    class scopeInitializer : public dataNode {
        public:
            std::shared_ptr<dataNode> code_block;

            scopeInitializer() : code_block(nullptr) {}
            scopeInitializer(std::shared_ptr<dataNode> code) : code_block(code) {}
            virtual ~scopeInitializer() = default;
    };

//  Class for a block of code representing one scope.
    class codeScope : public dataNode {
        public:
            std::shared_ptr<dataNode> curr_operation, remainder;

            codeScope();
            codeScope(std::shared_ptr<dataNode> curr_op, std::shared_ptr<dataNode> rem);
            codeScope(codeScope&& other);
    };

//  Class for expressional data that evaluates to a value.
    class valueData : public dataNode {
        public:
            virtual ~valueData() = default;
    };

//  Class for an if-else block of code.
    class ifBlock : public scopeInitializer {
        public:
            std::shared_ptr<dataNode> else_block;
            std::shared_ptr<valueData> bool_condition;
            bool contains_else;

            ifBlock();
            ifBlock(std::shared_ptr<valueData> b, std::shared_ptr<dataNode> code);
            ifBlock(std::shared_ptr<valueData> b, std::shared_ptr<dataNode> code, std::shared_ptr<dataNode> else_b);
            ifBlock(ifBlock&& other);
    };
    
//  Class for a variable assignment.
    class assignOp : public dataNode {
        public:
            std::string variable;
            std::shared_ptr<valueData> expression;

            assignOp();
            assignOp(const std::string& v, std::shared_ptr<valueData> e);
            assignOp(assignOp&& other);
    };

//  Class for a variable reassignment.
    class reassignOp : public dataNode {
        public:
            std::string variable;
            std::shared_ptr<valueData> expression;
            bool implicit;

            reassignOp();
            reassignOp(std::string& v, std::shared_ptr<valueData> e, bool i);
            reassignOp(reassignOp&& other);
    };

//  Class for a unary operator std::shared_ptr<dataNode>.
    class unaryOp : public valueData {
        public:
            TokenDef::tokenKey op;
            std::shared_ptr<valueData> expression;

            unaryOp();
            unaryOp(TokenDef::tokenKey op, std::shared_ptr<valueData> e);
            unaryOp(unaryOp&& other) noexcept;
    };

//  Class for a binary operator std::shared_ptr<dataNode>.
    class binaryOp : public valueData {
        public:
            TokenDef::tokenKey op;
            std::shared_ptr<valueData> expression1, expression2;

            binaryOp();
            binaryOp(TokenDef::tokenKey op, std::shared_ptr<valueData> e1, std::shared_ptr<valueData> e2);
            binaryOp(binaryOp&& other) noexcept;
    };

//  Class for a ternary operator std::shared_ptr<dataNode>.
    class ternaryOp : public valueData {
        public:
            TokenDef::tokenKey op;
            std::shared_ptr<valueData> expression1, expression2, expression3;

            ternaryOp();
            ternaryOp(TokenDef::tokenKey op, std::shared_ptr<valueData> e1, std::shared_ptr<valueData> e2, std::shared_ptr<valueData> e3);
            ternaryOp(ternaryOp&& other) noexcept;
    };

    //  Class for a varContainer std::shared_ptr<dataNode>.
    class varContainer : public valueData {
        public:
            std::string variable;

            varContainer();
            varContainer(std::string var);
            varContainer(varContainer&& other) noexcept;
    };

//  Parent class for irreducible data values.
    class irreducibleData : public valueData {
        public:
            virtual std::string disp(const DebugUtils::tokenDispOption option) const = 0;
            virtual ~irreducibleData() = default;
    };

//  Class for an intContainer std::shared_ptr<dataNode>.
    class intContainer : public irreducibleData {
        public:
            int number;

            intContainer();
            intContainer(int n);
            intContainer(const intContainer& other);

            std::string disp(const DebugUtils::tokenDispOption option) const override;
    };

//  Class for a boolContainer std::shared_ptr<dataNode>.
    class boolContainer : public irreducibleData {
        public:
            bool boolean;

            boolContainer();
            boolContainer(bool b);
            boolContainer(const boolContainer& other);

            std::string disp(const DebugUtils::tokenDispOption option) const override;
    };
}

#endif
