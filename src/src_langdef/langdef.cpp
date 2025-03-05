// File containing aliases and structure implementations for interpreter language definition.

#include "inc_langdef/langdef.hpp"
#include "inc_debug/compiler_debug.hpp"

using std::string, std::array, std::shared_ptr, std::size_t, std::move, std::visit, std::to_string;
using namespace TypingUtils;
using namespace DebugUtils;


// Namespace for definition of individual token data for lexing and parsing Regal code.
namespace TokenDef {
//  Useful subsets of tokens.
    array<tokenKey, number_type_count> number_tokens = {tokenKey::Int};
}

// Namespace for syntax tree data and definitions for parsing and executing Regal code.
namespace CodeTree {
    codeScope::codeScope() : curr_operation(), remainder() {}
    codeScope::codeScope(shared_ptr<dataNode> curr_op, shared_ptr<dataNode> rem) 
        : curr_operation(move(curr_op)), remainder(move(rem)) {}
    codeScope::codeScope(codeScope&& other)
        : curr_operation(move(other.curr_operation)),
        remainder(move(other.remainder)) {}


    ifBlock::ifBlock() : bool_condition(), scopeInitializer(), else_block(), contains_else(false) {}
    ifBlock::ifBlock(shared_ptr<valueData> b, shared_ptr<dataNode> code) : bool_condition(move(b)), scopeInitializer(move(code)), else_block(), contains_else(false) {}
    ifBlock::ifBlock(shared_ptr<valueData> b, shared_ptr<dataNode> code, shared_ptr<dataNode> else_b) 
        : bool_condition(move(b)), scopeInitializer(move(code)), else_block(move(else_b)), contains_else(true) {}
    ifBlock::ifBlock(ifBlock&& other) 
        : bool_condition(move(other.bool_condition)), 
        scopeInitializer(move(other.code_block)), 
        else_block(move(other.else_block)), contains_else(other.contains_else) {}


    assignOp::assignOp() : variable(""), expression() {}
    assignOp::assignOp(const string& v, shared_ptr<valueData> e) : variable(v), expression(move(e)) {}
    assignOp::assignOp(assignOp&& other) 
        : variable(move(other.variable)), expression(move(other.expression)) {}


    reassignOp::reassignOp() : variable(""), expression(), implicit(false) {}
    reassignOp::reassignOp(string& v, shared_ptr<valueData> e, bool i) : variable(v), expression(move(e)), implicit(i) {}
    reassignOp::reassignOp(reassignOp&& other) 
        : variable(move(other.variable)), expression(move(other.expression)), implicit(other.implicit) {}


    unaryOp::unaryOp() : op(TokenDef::tokenKey::Nothing), expression() {}
    unaryOp::unaryOp(TokenDef::tokenKey op, shared_ptr<valueData> e) : op(op), expression(move(e)) {}
    unaryOp::unaryOp(unaryOp&& other) noexcept : op(other.op),
        expression(move(other.expression)) {}


    binaryOp::binaryOp() : op(TokenDef::tokenKey::Nothing), expression1(), expression2() {}
    binaryOp::binaryOp(TokenDef::tokenKey op, shared_ptr<valueData> e1, shared_ptr<valueData> e2) : op(op), expression1(move(e1)), expression2(move(e2)) {}
    binaryOp::binaryOp(binaryOp&& other) noexcept : op(other.op),
        expression1(move(other.expression1)),
        expression2(move(other.expression2)) {}


    ternaryOp::ternaryOp() : op(TokenDef::tokenKey::Nothing), expression1(), expression2(), expression3() {}
    ternaryOp::ternaryOp(TokenDef::tokenKey op, shared_ptr<valueData> e1, shared_ptr<valueData> e2, shared_ptr<valueData> e3) 
        : op(op), expression1(move(e1)), expression2(move(e2)), expression3(move(e3)) {}
        ternaryOp::ternaryOp(ternaryOp&& other) noexcept : op(other.op),
        expression1(move(other.expression1)),
        expression2(move(other.expression2)),
        expression3(move(other.expression3)) {}


    varContainer::varContainer() : variable("") {}
    varContainer::varContainer(string var) : variable(var) {}
    varContainer::varContainer(varContainer&& other) noexcept : variable(move(other.variable)) {}


    intContainer::intContainer() : number(0) {}
    intContainer::intContainer(int n) : number(n) {}
    intContainer::intContainer(const intContainer& other) : number(other.number) {}

    string intContainer::disp(const tokenDispOption option) const {
        switch (option) {
            case tokenDispOption::Literal:
                return to_string(number);
            case tokenDispOption::Key:
                return "<integer>";
            default:
                return "<number>";
        }
    }


    boolContainer::boolContainer() : boolean(false) {}
    boolContainer::boolContainer(bool b) : boolean(b) {}
    boolContainer::boolContainer(const boolContainer& other) : boolean(other.boolean) {}
    
    string boolContainer::disp(const tokenDispOption option) const {
        switch (option) {
            case tokenDispOption::Literal:
                return (boolean ? "true" : "false");
            default:
                return "<boolean>";
        } 
    }
}
