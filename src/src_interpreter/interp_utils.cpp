/*

Implementations for primary interpreter utility constants, functions, and structures.

*/

#include "inc_interpreter/interp_utils.hpp"
#include "inc_internal/display_utils.hpp"

// Standard library aliases
using std::string, std::array, std::shared_ptr, std::uint16_t, std::uint32_t, std::int32_t, std::int64_t, 
      std::move, std::visit, std::to_string;

// interp_utils namespaces
using namespace InterpreterUtils;
using namespace TypingUtils;
using namespace TokenDef;


namespace InterpreterUtils {

    void normalize_number_str(string& number_str, const bool is_float) noexcept {
        uint16_t number_length = number_str.size();
        uint16_t i;

//      Count the number of leading zeroes.
        for (i = 0; (i < number_length) && (number_str[i] == '0'); i++);

//      Handle a string of just 0's.
        if (i == number_length) {
            number_str = "0";
            number_length = 1;

//      Handle the case for at least one leading 0.
        } else if (i != 0) {
//          If the leading 0's lead into a decimal, include one 0 in front of '.', otherwise remove all leading 0's.
            number_str = (number_str[i] == FLOAT_DELIMETER_TOKEN ? "0" : "") + number_str.substr(i);
            number_length = number_str.size();
        }

//      Only remove trailing zeroes in a floating-point number.
        if (is_float) {
//          Count trailing zeroes on a float.
            for (i = number_length - 1; (i >= 0) && (number_str[i] == '0'); i--);

//          If the trailing 0's lead to a decimal, include one 0 behind '.', otherwise remove all trailing 0's.
            number_str = number_str.substr(0, i + 1) + (number_str[i] == FLOAT_DELIMETER_TOKEN ? "0" : "");
        }

        return;
    }

}

namespace CodeTree {

    // The line number and object type of each class is initialized with the parent class's constructor. In move/copy constructors, 
    // the copy constructor of a parent class is called to transfer the line number.

            /*              NON-EXPRESSIONAL DATA               */

        /*      codeScope implementation       */

    codeScope::codeScope() 
        : dataNode(nodeType::CodeScope),
          curr_operation(nullptr), 
          remainder(nullptr) {}

    codeScope::codeScope(const uint32_t line_number, shared_ptr<dataNode> curr_op, shared_ptr<dataNode> rem) 
        : dataNode(nodeType::CodeScope, line_number),
          curr_operation(move(curr_op)), 
          remainder(move(rem)) {}

    inline codeScope::codeScope(codeScope&& other) noexcept
        : dataNode(other),
          curr_operation(move(other.curr_operation)),
          remainder(move(other.remainder)) {}


        /*      ifBlock implementation       */

//  The main 'if' block of code is initialized with the scopeInitializer constructor.

    ifBlock::ifBlock() 
        : bool_condition(nullptr), 
          scopeInitializer(nodeType::IfBlock), 
          else_block(nullptr), 
          contains_else(false) {}

    ifBlock::ifBlock(const uint32_t line_number, shared_ptr<valueData> bool_cond, shared_ptr<dataNode> block) 
        : bool_condition(move(bool_cond)), 
          scopeInitializer(nodeType::IfBlock, line_number, block),
          else_block(nullptr), 
          contains_else(false) {}

    ifBlock::ifBlock(const uint32_t line_number, shared_ptr<valueData> bool_cond, shared_ptr<dataNode> block, shared_ptr<dataNode> else_blck) 
        : bool_condition(move(bool_cond)), 
          scopeInitializer(nodeType::IfBlock, line_number, move(block)), 
          else_block(move(else_blck)), 
          contains_else(true) {}

    inline ifBlock::ifBlock(ifBlock&& other) noexcept
        : bool_condition(move(other.bool_condition)), 
          scopeInitializer(other), 
          else_block(move(other.else_block)), 
          contains_else(other.contains_else) {}


        /*      assignOp implementation     */

    assignOp::assignOp() 
        : dataNode(nodeType::AssignOp),
          variable(""), 
          expression(nullptr) {}

    assignOp::assignOp(const uint32_t line_number, const string& var, shared_ptr<valueData> expr) 
        : dataNode(nodeType::AssignOp, line_number),
          variable(var), 
          expression(move(expr)) {}

    inline assignOp::assignOp(assignOp&& other) noexcept
        : dataNode(other),
          variable(move(other.variable)), 
          expression(move(other.expression)) {}


        /*      reassignOp implementation       */

    reassignOp::reassignOp() 
        : dataNode(nodeType::ReassignOp),
          variable(""), 
          expression(nullptr) {}

    reassignOp::reassignOp(const uint32_t line_number, const string& var, shared_ptr<valueData> expr) 
        : dataNode(nodeType::ReassignOp, line_number),
          variable(var), 
          expression(move(expr)) {}

    inline reassignOp::reassignOp(reassignOp&& other) noexcept
        : dataNode(other),
          variable(move(other.variable)), 
          expression(move(other.expression)) {}


            /*              EXPRESSIONAL DATA               */

        /*      unaryOp implementation      */

    unaryOp::unaryOp() 
        : valueData(nodeType::UnaryOp),
          op(tokenKey::Nothing), 
          expression(nullptr) {}

    unaryOp::unaryOp(const uint32_t line_number, const tokenKey op, shared_ptr<valueData> expr) 
        : valueData(nodeType::UnaryOp, line_number),
          op(op), 
          expression(move(expr)) {}

    inline unaryOp::unaryOp(unaryOp&& other) noexcept 
        : valueData(other),
          op(other.op),
          expression(move(other.expression)) {}


        /*      binaryOp implementation     */

    binaryOp::binaryOp() 
        : valueData(nodeType::BinaryOp),
          op(tokenKey::Nothing), 
          expression1(nullptr), 
          expression2(nullptr) {}

    binaryOp::binaryOp(const uint32_t line_number, const tokenKey op, shared_ptr<valueData> expr1, shared_ptr<valueData> expr2) 
        : valueData(nodeType::BinaryOp, line_number),
          op(op), 
          expression1(move(expr1)), 
          expression2(move(expr2)) {}

    inline binaryOp::binaryOp(binaryOp&& other) noexcept 
        : valueData(other),
          op(other.op),
          expression1(move(other.expression1)),
          expression2(move(other.expression2)) {}


        /*      ternaryOp implementation        */

    ternaryOp::ternaryOp() 
        : valueData(nodeType::TernaryOp),
          op(tokenKey::Nothing), 
          expression1(nullptr), 
          expression2(nullptr), 
          expression3(nullptr) {}

    ternaryOp::ternaryOp(const uint32_t line_number, const tokenKey op, shared_ptr<valueData> expr1, shared_ptr<valueData> expr2, shared_ptr<valueData> expr3) 
        : valueData(nodeType::TernaryOp, line_number),
          op(op), 
          expression1(move(expr1)), 
          expression2(move(expr2)), 
          expression3(move(expr3)) {}

    inline ternaryOp::ternaryOp(ternaryOp&& other) noexcept 
        : valueData(other),
          op(other.op),
          expression1(move(other.expression1)),
          expression2(move(other.expression2)),
          expression3(move(other.expression3)) {}


        /*      varContainer implementation     */

    varContainer::varContainer() 
        : valueData(nodeType::VarContainer),
          variable("") {}

    varContainer::varContainer(const uint32_t line_number, const string& var) 
        : valueData(nodeType::VarContainer, line_number),
          variable(var) {}

    inline varContainer::varContainer(varContainer&& other) noexcept 
        : valueData(other),
          variable(move(other.variable)) {}


            /*              IRREDUCIBLE (PRIMITIVE) DATA                */

        /*      int32Container implementation       */

    inline constexpr int32Container::int32Container() noexcept 
        : irreducibleData(nodeType::Int32Container),
          number(0) {}

    inline constexpr int32Container::int32Container(const int32Container& other) noexcept 
        : irreducibleData(other),
          number(other.number) {}

    inline string int32Container::disp() const noexcept {
//      Pass the second parameter as false since the number is not floating-point.
        return num_to_string<int32_t>(number, false);
    }


        /*      int64Container implementation       */

    inline constexpr int64Container::int64Container() noexcept 
        : irreducibleData(nodeType::Int64Container),
          number(0) {}

    inline constexpr int64Container::int64Container(const int64Container& other) noexcept 
        : irreducibleData(other),
          number(other.number) {}

    inline string int64Container::disp() const noexcept {
//      Pass the second parameter as false since the number is not floating-point.
        return num_to_string<int64_t>(number, false);
    }

    
        /*      float32Container implementation     */

    inline constexpr float32Container::float32Container() noexcept 
        : irreducibleData(nodeType::Float32Container),
          number(0.0) {}

    inline constexpr float32Container::float32Container(const float32Container& other) noexcept 
        : irreducibleData(other),
          number(other.number) {}

    inline string float32Container::disp() const noexcept {
//      Pass the second parameter as true since the number is floating-point.
        return num_to_string<float>(number, true);
    }


        /*      float64Container implementation        */

    inline constexpr float64Container::float64Container() noexcept 
        : irreducibleData(nodeType::Float64Container),
          number(0.0) {}

    inline constexpr float64Container::float64Container(const float64Container& other) noexcept 
        : irreducibleData(other),
          number(other.number) {}

    inline string float64Container::disp() const noexcept {
//      Pass the second parameter as true since the number is floating-point.
        return num_to_string<double>(number, true);
    }


        /*      boolContainer implementation       */

    inline constexpr boolContainer::boolContainer() noexcept 
        : irreducibleData(nodeType::BoolContainer),
          boolean(false) {}

    inline constexpr boolContainer::boolContainer(const boolContainer& other) noexcept 
        : irreducibleData(other),
          boolean(other.boolean) {}
    
    inline string boolContainer::disp() const noexcept {
        return (boolean ? BOOL_TRUE_TOKEN : BOOL_FALSE_TOKEN);
    }

}
