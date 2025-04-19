/*

Declarations for primary interpreter utility constants, functions, and structures.
Utilities are separated into namespaces, categorizing their use in the interpreter.
The first namespace is general usage.

*/

#ifndef LANGDEF_HPP
#define LANGDEF_HPP

#include <string>
#include <array>
#include <tuple>
#include <variant>
#include <memory>
#include <utility>
#include <limits>
#include <cstdint>
#include <cmath>


// Utility functions and constants used throughout the interpreter.
namespace InterpreterUtils {

//  Integer overflow limits.
    constexpr std::int64_t MAX_INT32 = static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max());
    constexpr std::int64_t MIN_INT32 = -MAX_INT32;
    constexpr std::int64_t MAX_INT64 = std::numeric_limits<std::int64_t>::max();
    constexpr std::int64_t MIN_INT64 = -MAX_INT64;

//  Float overflow limits.
    constexpr float MAX_ACCURATE_FLOAT32 = static_cast<float>(1ULL << 24);
    constexpr float MIN_ACCURATE_FLOAT32 = -MAX_ACCURATE_FLOAT32;
    constexpr double MAX_ACCURATE_FLOAT64 = static_cast<double>(1ULL << 53);
    constexpr double MIN_ACCURATE_FLOAT64 = -MAX_ACCURATE_FLOAT64;
    const double LOG_MAX_FLOAT64 = std::log2(MAX_ACCURATE_FLOAT64);

//  Minimum relative error between a 32-bit and 64-bit float that warrants a 32-bit float be promoted to a 64-bit.
    constexpr double FLOAT_PROMOTION_THRESHOLD = 0.0000001;

/*  
    Update a string representing a number to a normalized string representing the same number.
    Remove leading zeroes. Also remove trailing zeroes if the number is floating point.
    For floating point values declared with no number before or after the '.', 
    add a 0 respectively (e.g. "12." becomes "12.0", ".3" becomes "0.3")

    This function assumes that the given number string contains at most one '.' and has only digit characters otherwise.
    It also asumes that the given string is less than 65,535 characters long (the size of a uint16_t).

    Parameters:
        number_str: number string to update (input/output)
        is_float: true if the number string represents a floating-point number (input)
*/
    void normalize_number_str(std::string& number_str, const bool is_float) noexcept;

/*
    Check if the given estimated number is inaccurately represented with 32-bits, 
    and should be promoted to 64-bit storage.
    Determine whether the number should be promoted according to FLOAT_PROMOTION_THRESHOLD in InterpreterUtils.

    Parameters: 
        true_val: the 64-bit version of the number (input)
        estimated_val: the 32-bit version of the number (input)

    Return true if the floating-point number should be promoted to 64-bits.
*/
    inline constexpr bool promote_float(const double true_val, const float estimated_val) noexcept {
//      Avoid division by 0.
        if (true_val == 0.0) {
            return false;
        }

//      Compute the relative error between 32-bit precision and 64-bit precision.
        const double relative_error = (
            true_val > estimated_val
                ? true_val - estimated_val 
                : estimated_val - true_val
            ) / true_val;

//      Return whether the relative error is large enough to warrant greater storage.
        return relative_error >= FLOAT_PROMOTION_THRESHOLD;
    }

/*
    Create a normalized display string for a given number.
    This function depends on a typename template.

    This function assumes that the typename used by the caller is a number type.

    Parameters:
        number: template type value to create a string for (input)
        is_float: true if the given number is a floating-point number (input)

    Return the display string of the number.
*/
    template <typename T>
    const std::string num_to_string(const T number, const bool is_float) noexcept {
//      Retrieve the basic number string and normalize it.
        std::string number_str = std::to_string(number);
        normalize_number_str(number_str, is_float);

        return number_str;
    }

}


// Structures for type-related interpretation tasks.
namespace TypingUtils {

//  Data types.
    enum class dataType {
        Int32T, Int64T, Float32T, Float64T, BoolT
    };

//  Number types are unique in that they implicitly combine.
//  Maintain a collection of these special types.
    constexpr std::uint8_t number_type_count = 4;
    constexpr std::array<dataType, number_type_count> number_types = 
        {dataType::Int32T, dataType::Int64T, dataType::Float32T, dataType::Float64T};

}


// Structures for tokenized data.
namespace TokenDef {

//  All possible tokens.
    enum class tokenKey {
//          Keywords
            Assign, If, Else,
        
//          Irreducible data types
            Int32, Int64, Float32, Float64, Bool,
        
//          Unary Operators
            Not, NotW,
        
//          Binary Operators
            Plus, Minus, Mult, Div, Exp, And, AndW, Or, OrW, Xor, XorW, 
            Greater, Less, Equals, Is, GrEqual, LessEqual,

//          If-Else is also a ternary operator.
        
//          Variables
            Var,
            Bind,
        
//          Miscellaneous
            LeftPar, RightPar, Newline,
        
//          Internal token for default initializations
            Nothing
        };


        /*      REGAL SYNTAX        */

//  Keywords
    constexpr const char* ASSIGN_TOKEN = "let";
    constexpr const char* IF_TOKEN = "if";
    constexpr const char* ELSE_TOKEN = "else";

//  Boolean values
    constexpr const char* BOOL_TRUE_TOKEN = "true";
    constexpr const char* BOOL_FALSE_TOKEN = "false";

//  Unary operators
    constexpr char NOT_TOKEN = '!';
    constexpr const char* NOTW_TOKEN = "not";

//  Mathematic binary operators
    constexpr char PLUS_TOKEN = '+';
    constexpr char MINUS_TOKEN = '-';
    constexpr char MULT_TOKEN = '*';
    constexpr char DIV_TOKEN = '/';
    constexpr const char* EXP_TOKEN = "**";

//  Boolean binary operators
    constexpr char AND_TOKEN = '&';
    constexpr const char* ANDW_TOKEN = "and";
    constexpr char OR_TOKEN = '|';
    constexpr const char* ORW_TOKEN = "or";
    constexpr const char* XOR_TOKEN = "||";
    constexpr const char* XORW_TOKEN = "xor";

//  Comparative binary operators
    constexpr char GREATER_TOKEN = '>';
    constexpr char LESS_TOKEN = '<';
    constexpr const char* EQUALS_TOKEN = "==";
    constexpr const char* IS_TOKEN = "is";
    constexpr const char* GREQUAL_TOKEN = ">=";
    constexpr const char* LESSEQUAL_TOKEN = "<=";

//  Variables
    constexpr char BIND_TOKEN = '=';

//  Miscellaneous
    constexpr char INLINE_COMMENT_TOKEN = '#';
    constexpr const char* COMMENT_BLOCK_TOKEN = "##";
    constexpr char LEFTPAR_TOKEN = '(';
    constexpr char RIGHTPAR_TOKEN = ')';
    constexpr char FLOAT_DELIMETER_TOKEN = '.';
    constexpr char NEWLINE_TOKEN = '\n';


//  Different data that tokens can hold.
    using tokenData = std::variant<
        bool, 
        std::int32_t, 
        std::uint32_t, 
        std::uint64_t, 
        float, 
        double, 
        std::string
    >;
//  Each token is a 3-tuple containing
//      <1>: the token key (e.g. "12" has key Int32)
//      <2>: any necessary token data, if no data is required the default value is false
//      <3>: the line number of that token
    using token = std::tuple<tokenKey, tokenData, std::uint32_t>;

/*
    When storing tokens, some require data. For example, storing the integer 12 would use the tuple (tokenKey::Int32, 12, line_num).
    Tokens that require data and their stored types are
        Int32: uint32_t,
        Int64: uint64_t,
        Float32: float,
        Float64: double,
        Bool: bool,
        Var: string,
        Newline: int32_t

    This data is stored at index 1 in the token, after the tokenKey.
    Note, all numbers are lexed as positive, so unsigned values can be used to store them.
        e.g. "-12" is lexed as [(tokenKey::Minus, false, line_num), (tokenKey::Int32, 12, line_num)].
*/


//  Maintain a collection of numerical comparative operators.
    constexpr const char* COMP_GENERIC_NAME = "comparative operator";
    constexpr std::uint8_t comparative_op_count = 4;
    constexpr std::array<tokenKey, comparative_op_count> comparative_ops = 
        {tokenKey::Greater, tokenKey::Less, tokenKey::GrEqual, tokenKey::LessEqual};

//  Collection of number tokens, one to one with number types.
    constexpr const char* NUMBER_GENERIC_NAME = "number";
    constexpr std::array<tokenKey, TypingUtils::number_type_count> number_tokens = 
        {tokenKey::Int32, tokenKey::Int64, tokenKey::Float32, tokenKey::Float64};

//  Global exclusive minimum indentation required for a file of code.
//  Any value less than 0 means there is no minimum indentation.
    constexpr std::int32_t GLOBAL_INDENT = -1;

}


// Syntax tree structures
namespace CodeTree {
//  Types of data nodes for fast child class retrieval.
    enum class nodeType {
//      Parent classes
        DataNode,
        ScopeInitializer,
        ValueData,
        IrreducibleData,

//      Non-expressional data
        CodeScope,
        IfBlock,
        AssignOp,
        ReassignOp,

//      Expressional data
        UnaryOp,
        BinaryOp,
        TernaryOp,
        VarContainer,

//      Ireducible (primitive) data
        Int32Container, Int64Container,
        Float32Container, Float64Container,
        BoolContainer,
    };

    // Note, all dataNode and child class objects initialize their type variable automatically.

                    /*              PARENT CLASSES              */

//  Parent for all data.
    class dataNode {
        public:
//          Represent the type of the object.
            nodeType type;
//          Line number that the data was input on, for error message output.
            std::uint32_t line_number;

//          Default constructor, initialize the line number to 0.
            inline constexpr dataNode() noexcept
                : type(nodeType::DataNode),
                  line_number(0) {}
            
//          Initialize the object type and the line number to 0.
            inline constexpr explicit dataNode(const nodeType obj_type) noexcept
                : type(obj_type),
                  line_number(0) {}

//          Initialize the object type and line number.
            inline constexpr explicit dataNode(const nodeType obj_type, const std::uint32_t line_num) noexcept
                : type(obj_type),
                  line_number(line_num) {}

//          Copy constructor.
            inline constexpr dataNode(const dataNode& other) noexcept
                : type(other.type),
                  line_number(other.line_number) {}

//          Virtual default destructor
            inline virtual ~dataNode() noexcept = default;
    };

//  Parent for commands that create a new scope. (e.g. if statements, loops)
    class scopeInitializer : public dataNode {
        public:
//          Shared pointer to the new scope
            std::shared_ptr<dataNode> code_block;

//          Default constructor, initialize the line number to 0 and code scope to a nullptr.
            inline scopeInitializer() 
                : dataNode(nodeType::ScopeInitializer),
                  code_block(nullptr) {}

//          Initialize the object type and line number to 0.
            inline constexpr explicit scopeInitializer(const nodeType obj_type) noexcept
                : dataNode(obj_type) {}

//          Initialize the object type, line number, and code block.
            inline explicit scopeInitializer(const nodeType obj_type, const std::uint32_t line_number, std::shared_ptr<dataNode> code) 
                : dataNode(obj_type, line_number),
                  code_block(std::move(code)) {}

//          Move constructor.
            inline scopeInitializer(const scopeInitializer& other) noexcept
                : dataNode(other),
                  code_block(std::move(other.code_block)) {}

//          Virtual default destructor
            inline virtual ~scopeInitializer() noexcept = default;
    };

//  Parent for expressional data that can be evaluated.
    class valueData : public dataNode {
        public:
//          Default constructor, initialize the line number to 0.
            inline constexpr valueData() noexcept
                : dataNode(nodeType::ValueData) {}

//          Initialize the object type and line number to 0.
            inline constexpr explicit valueData(const nodeType obj_type) noexcept
                : dataNode(obj_type) {}

//          Initialize the object type and line number.
            inline constexpr explicit valueData(const nodeType obj_type, const std::uint32_t line_number) noexcept
                : dataNode(obj_type, line_number) {}

//          Copy constructor.
            inline constexpr valueData(const valueData& other) noexcept
                : dataNode(other) {}

//          Virtual default destructor
            inline virtual ~valueData() noexcept = default;
    };

//  Irreducible data values, structures for primitive data. Purely virtual.
    class irreducibleData : public valueData {
        public:
//          Default constructor, initialize the line number to 0.
            inline constexpr irreducibleData() noexcept
                : valueData(nodeType::IrreducibleData) {}

//          Initialize the object type and line number to 0.
            inline constexpr explicit irreducibleData(const nodeType obj_type) noexcept
                : valueData(obj_type) {}

//          Initialize the line number.
            inline constexpr explicit irreducibleData(const nodeType obj_type, const std::uint32_t line_number) noexcept
                : valueData(obj_type, line_number) {}

//          Copy constructor.
            inline constexpr irreducibleData(const irreducibleData& other) noexcept
                : valueData(other) {}

/*
            Create a display string for a piece of irreducible data.
            Return the created display string.
*/
            inline virtual std::string disp() const noexcept = 0;

//          Virtual default destructor
            inline virtual ~irreducibleData() noexcept = default;
    };


                    /*              NON-EXPRESSIONAL DATA               */

//  A multi-line block of code representing one scope
    class codeScope : public dataNode {
        public:
//          Shared pointers to the first operation and the rest of the operations
            std::shared_ptr<dataNode> curr_operation, remainder;

//          Default constructor, initialize the line number to 0 and class variables to nullptr.
            codeScope();

//          Initialize the line number and each class variable respectively.
            explicit codeScope(const std::uint32_t line_number, std::shared_ptr<dataNode> curr_op, std::shared_ptr<dataNode> rem);

//          Move constructor
            codeScope(codeScope&& other) noexcept;
    };

//  If-Else scope of code
    class ifBlock : public scopeInitializer {
        public:
//          Shared pointer to the boolean condition expressional data
            std::shared_ptr<valueData> bool_condition;
//          Shared pointer to a scope for the 'else' code, nullptr if there is no 'else'
            std::shared_ptr<dataNode> else_block;
//          True if the If-Else scope contains an 'else' scope.
            bool contains_else;

//          Default constructor, initialize line number to 0, pointers to nullptr, and the else boolean to false.
            ifBlock();

//          Constructor for If-Else with no 'else'.
//          Initialize line number, main scope, and boolean condition respectively. Initialize 'else' boolean to false.
            explicit ifBlock(const std::uint32_t line_number, std::shared_ptr<valueData> bool_cond, std::shared_ptr<dataNode> block);

//          Constructor for If-Else with an 'else' scope.
//          Initialize line number, main scope, 'else' scope, and boolean condition respectively. Initialize 'else' boolean to true.
            explicit ifBlock(const std::uint32_t line_number, std::shared_ptr<valueData> bool_cond, 
                             std::shared_ptr<dataNode> block, std::shared_ptr<dataNode> else_blck);
            
//          Move constructor
            ifBlock(ifBlock&& other) noexcept;
    };
    
//  Variable assignment
    class assignOp : public dataNode {
        public:
//          Variable name string
            std::string variable;
//          Shared pointer to expressional data to assign to the variable
            std::shared_ptr<valueData> expression;

//          Default constructor, initialize the line number to 0, variable name to the empty string, and the expression pointer to nullptr. 
            assignOp();

//          Initialize the line number, variable name, expression pointer respectively.
            explicit assignOp(const std::uint32_t line_number, const std::string& var, std::shared_ptr<valueData> expr);

//          Move constructor
            assignOp(assignOp&& other) noexcept;
    };

//  Variable reassignment
    class reassignOp : public dataNode {
        public:
//          Variable name string
            std::string variable;
//          Shared pointer to expressional data to assign to the variable
            std::shared_ptr<valueData> expression;

//          Default constructor, initialize the line number to 0, variable name to the empty string, and the expression pointer to nullptr.
            reassignOp();

//          Initialize the line number, variable name, and expression pointer respectively.
            explicit reassignOp(const std::uint32_t line_number, const std::string& var, std::shared_ptr<valueData> expr);

//          Move constructor
            reassignOp(reassignOp&& other) noexcept;
    };
    

                    /*              EXPRESSIONAL DATA               */

//  Operators that take one argument
    class unaryOp : public valueData {
        public:
//          Operator token key
            TokenDef::tokenKey op;
//          Shared pointer to argument expression
            std::shared_ptr<valueData> expression;

//          Default constructor, initialize the line number to 0, operator to Nothing, and the argument pointer to nullptr.
            unaryOp();

//          Initialize the line number, operator, and argument expression respectively.
            explicit unaryOp(const std::uint32_t line_number, const TokenDef::tokenKey op, std::shared_ptr<valueData> expr);

//          Move constructor
            unaryOp(unaryOp&& other) noexcept;
    };

//  Operators that take two arguments
    class binaryOp : public valueData {
        public:
//          Operator token key
            TokenDef::tokenKey op;
//          Shared pointers to argument expressions.
            std::shared_ptr<valueData> expression1, expression2;

//          Default constructor, initialize the line number to 0, operator to Nothing, and argument expressions to nullptr.
            binaryOp();

//          Initialize the line number, operator, and argument expressions respectively.
            explicit binaryOp(const std::uint32_t line_number, const TokenDef::tokenKey op, 
                              std::shared_ptr<valueData> expr1, std::shared_ptr<valueData> expr2);
            
//          Move constructor
            binaryOp(binaryOp&& other) noexcept;
    };

//  Operators that take three arguments
    class ternaryOp : public valueData {
        public:
//          Operator token key
            TokenDef::tokenKey op;
//          Shared pointers to argument expressions.
            std::shared_ptr<valueData> expression1, expression2, expression3;

//          Default constructor, initialize the line number to 0, operator to Nothing, and argument expressions to nullptr.
            ternaryOp();

//          Initialize the line number, operator, and argument expressions respectively.
            explicit ternaryOp(const std::uint32_t line_number, const TokenDef::tokenKey op, 
                               std::shared_ptr<valueData> expr1, std::shared_ptr<valueData> expr2, std::shared_ptr<valueData> expr3);
            
//          Move constructor
            ternaryOp(ternaryOp&& other) noexcept;
    };

//  Variable
    class varContainer : public valueData {
        public:
//          Variable name
            std::string variable;

//          Default constructor, initialize the line number to 0 and variable to the empty string.
            varContainer();

//          Initialize the line number and variable.
            explicit varContainer(const std::uint32_t line_number, const std::string& var);

//          Move constructor
            varContainer(varContainer&& other) noexcept;
    };


                    /*              IRREDUCIBLE (PRIMITIVE) DATA                */

//  32-bit whole number
    class int32Container : public irreducibleData {
        public:
//          Whole number value
            std::int32_t number;

//          Default constructor, initialize the line number and number value to 0.
            constexpr int32Container() noexcept;

//          Initialize the line number and number value.
            inline constexpr explicit int32Container(const std::uint32_t line_number, const std::int32_t num) noexcept
                : irreducibleData(nodeType::Int32Container, line_number),
                  number(num) {}
    
//          Copy constructor
            constexpr int32Container(const int32Container& other) noexcept;

/*
            Create a display string for a 32-bit whole number.
            Return the created display string.
*/
            std::string disp() const noexcept override;
    };

//  64-bit whole number
    class int64Container : public irreducibleData {
        public:
//          Whole number value
            std::int64_t number;

//          Default constructor, initialize the line number and number value to 0.
            constexpr int64Container() noexcept;

//          Initialize the line number and number value.
            inline constexpr explicit int64Container(const std::uint32_t line_number, const std::int64_t num) noexcept
                : irreducibleData(nodeType::Int64Container, line_number),
                  number(num) {}

//          Copy constructor
            constexpr int64Container(const int64Container& other) noexcept;

/*
            Create a display string for a 64-bit whole number.
            Return the created display string.
*/
            std::string disp() const noexcept override;
    };

//  32-bit precision floating-point number
    class float32Container : public irreducibleData {
        public:
//          Floating-point number value
            float number;

//          Default constructor, initialize the line number to 0 and number value to 0.0.
            constexpr float32Container() noexcept;

//          Initialize the line number and number value.
            inline constexpr explicit float32Container(const std::uint32_t line_number, const float num) noexcept
                : irreducibleData(nodeType::Float32Container, line_number),
                  number(num) {}

//          Copy constructor
            constexpr float32Container(const float32Container& other) noexcept;

/*
            Create a display string for a 32-bit floating-point number.
            Return the created display string.
*/
            std::string disp() const noexcept override;
    };

//  64-bit precision floating-point number
    class float64Container : public irreducibleData {
        public:
//          Floating-point number value
            double number;

//          Default constructor, initialize the line number to 0 and number value to 0.0.
            constexpr float64Container() noexcept;

//          Initialize the line number and number value.
            inline constexpr explicit float64Container(const std::uint32_t line_number, const double num) noexcept
                : irreducibleData(nodeType::Float64Container, line_number),
                  number(num) {}

//          Copy constructor
            constexpr float64Container(const float64Container& other) noexcept;

/*
            Create a display string for a 64-bit floating-point number.
            Return the created display string.
*/
            std::string disp() const noexcept override;
    };

//  Boolean
    class boolContainer : public irreducibleData {
        public:
//          Boolean value
            bool boolean;

//          Default constructor, initialize the line number to 0 and boolean value to false.
            constexpr boolContainer() noexcept;

//          Initialize the line number and boolean value.
            inline constexpr explicit boolContainer(const std::uint32_t line_number, bool bl) noexcept
                : irreducibleData(nodeType::BoolContainer, line_number),
                  boolean(bl) {}

//          Copy constructor
            constexpr boolContainer(const boolContainer& other) noexcept;

/*
            Create a display string for a boolean.
            Return the created display string.
*/
            std::string disp() const noexcept override;
    };

}


#endif
