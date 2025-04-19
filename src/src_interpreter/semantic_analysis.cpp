/*

Function implementations for optimization and typechecking.

*/

#include "inc_interpreter/semantic_analysis.hpp"

// Standard library aliases
using std::list, std::map, std::shared_ptr, std::string, std::pair, std::tuple, std::array, std::size_t, std::variant, std::is_same, 
      std::less, std::greater, std::equal_to, std::greater_equal, std::less_equal, std::logical_and, std::logical_or, std::not_equal_to, std::plus,
      std::uint8_t, std::uint32_t, std::int8_t, std::int32_t, std::int64_t, std::to_string, std::make_pair, std::make_tuple, std::move, std::get, 
      std::dynamic_pointer_cast, std::make_shared, std::tie, std::log2, std::abs, std::pow, std::find, std::visit;

// interp_utils namespaces
using namespace InterpreterUtils;
using namespace TypingUtils;
using namespace TokenDef;
using namespace CodeTree;

// semantic_analysis namespace
using namespace DataStorage;


// Optimization/typechecking helper functions.
namespace {

//  Function pointer type for identity checks with boolean binary operators.
    using boolIdFunc = const bool(*)(const std::pair<bool, bool>&,
                                     const bool, const bool,
                                     const uint32_t, shared_ptr<valueData>&);

//  Function pointer type for floating-point operations.
    using floatOpFunc = const double(*)(const double, const double);

//  Function pointer type for floating-point identity operation checks.
    using floatIdFunc = const bool(*)(const double, const double,
                                      const bool, const bool, const dataType, 
                                      const binaryOp*, shared_ptr<valueData>&, pair<bool, dataType>&);

//  Function pointer type for floating-point operation overflow checks
    using floatOverflowFunc = void(*)(const double, const double, const uint32_t);

    
/*
    Mimic a boolean identity function but always return false. Used for operators that have no identities.
*/
    inline const bool _no_bool_id(const pair<bool, bool>& two_bools, const bool b1, const bool b2, const uint32_t num, shared_ptr<valueData>& data) noexcept {
        return false;
    }

    
/*
    Replace the given value data with a number container that holds the given number, but uses less memory when possible.
        e.g. int64Container(12) would be wrapped into int32Container(12), since 12 takes less than 32-bits to store.
    This function depends on a typename template that represents the type of the given value.

    This function assumes that the given floating point boolean accurately represents the given typename and value type,
        e.g. the floating point boolean is only true when the value parameter is a float of some kind.
    Furthermore, this function assumes that the given typename and value type is a 64 or 32-bit integer or float.

    Parameters:
        value: the number to wrap in a value data node (input)
        floating_point: true if the given value is a float of some kind (input)
        line_number: line number that the wrapped data should have (input)
        value_data: value data object to store the node wrapping the given data (output)
    
    Return the data type that was used to wrap the given value.
*/
    template <typename T>
    const dataType _wrap_number_data(const T value, const bool floating_point, const uint32_t line_number, shared_ptr<valueData>& value_data) noexcept {
//      Retrieve the line number.
        const uint32_t line_num = value_data->line_number;

//      Retrieve the maximum and minimum values for wrapping depending on the given type.
        T max32 = floating_point ? MAX_ACCURATE_FLOAT32 : MAX_INT32;
        T min32 = floating_point ? MIN_ACCURATE_FLOAT32 : MIN_INT32;

//      Check if the value cannot be contained with 32-bits.
        if ((value > max32) || (value < min32)) {
//          Store the value in the appropriate 64-bit container.
            if (floating_point) {
                value_data = make_shared<float64Container>(line_num, value);
                return dataType::Float64T;
            }

            value_data = make_shared<int64Container>(line_num, value);
            return dataType::Int64T;

        } else if (floating_point) {
//          Check if the float needs to be promoted to a 64-bit float for greater precision.
            const float estimated_val = static_cast<float>(value);

            if (promote_float(value, estimated_val)) {
                value_data = make_shared<float64Container>(line_num, value);
                return dataType::Float64T;
            }

            value_data = make_shared<float32Container>(line_num, estimated_val);
            return dataType::Float32T;
        }

        value_data = make_shared<int32Container>(line_num, static_cast<int32_t>(value));
        return dataType::Int32T;
    }

/*
    Wrap the given value data in a float container if it is not already.
    
    This function assumes that the given value data is a number container.
    Furthermore, it assumes that the given type corresponds to the type of 
    number container that value data is.
    
    Parameters:
        type: the type of the value data (input)
        value_data: data object to wrap (input/output)

    Return the new type of the wrapped data, or the original type if the data was a float.
*/
    inline const dataType _wrap_to_float(const dataType type, shared_ptr<valueData>& value_data) noexcept {
//      Default to return the original type.
        dataType new_type = type;

//      Wrap the data if the type is an integer.
        if (type == dataType::Int32T) {
            shared_ptr<int32Container> int32 = dynamic_pointer_cast<int32Container>(value_data);
            new_type = _wrap_number_data<double>(int32->number, true, value_data->line_number, value_data);
        } else if (type == dataType::Int64T) {
            shared_ptr<int64Container> int64 = dynamic_pointer_cast<int64Container>(value_data);
            new_type = _wrap_number_data<double>(int64->number, true, value_data->line_number, value_data);
        }

        return new_type;
    }


        /*      ADDITION      */

//  Operator object to add two values.
    class numAdd {
        public:

/*
            Add the given values. This operator depends on a typename template.

            This operator assumes that the given type is addable.

            Parameters:
                n1: first number (input)
                n2: second number (input)

            Return the sum.
*/
            template <typename T>
            constexpr T operator()(const T n1, const T n2) const noexcept {
                return n1 + n2;
            }
    };

//  Operator object to check the identities of addition.
    class addId {
        public:

/*
            Check additive idenities of the given pair of numbers from a binary operator. 
            This operator depends on a typename template for the given numbers.
            Update the given value data with the identity result and update the id_output with the a 
            pair containing information about optimization and type.

            This operator assumes that the given optimization status booleans accurately represent whether
            each expression of the given binary operator are optimized. Furthermore, it assumes that the given 
            types accurately represent the type of each expression of the given binary operator.

            Parameters:
                nums: pair of numbers being added (input)
                opt_expr1/2: true if the respective binary operator expression was optimized (input)
                type1/2: type of each binary operator expression (input)
                binary_op: pointer to a binary operator object (input)
                value_data: reference to the current optimizable object (input/output)
                id_output: output parameter pair containing
                               first: true if the value data object is optimized
                               second: the type of the value data object (output)

            Return true if an identity was satisfied.
*/
            template <typename T>
            const bool operator()(const pair<T, T>& nums, const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, 
                                  const binaryOp* binary_op, shared_ptr<valueData>& value_data, pair<bool, dataType>& id_output) const noexcept {
//              Check addition identities depending on which expression was optimized.
                if (opt_expr1 && (nums.first == 0)) { // 0 + x
                    value_data = binary_op->expression2;
                    id_output = make_pair(opt_expr2, type2);
                    return true;
                } else if (opt_expr2 && (nums.second == 0)) { // x + 0
                    value_data = binary_op->expression1;
                    id_output = make_pair(opt_expr1, type1);
                    return true;
                }

                return false;
            }
    };

//  Operator object to check for overflow during addition.
    class addOverflow {
        public:

/*
            Check if the given pair of numbers would cause overflow when added. This operator
            depends on a typename template for the given numbers.

            This function assumes that the given boolean accurately represents whether the given numbers are floats.

            Throw an exception if the result would cause overflow.

            Parameters:
                nums: pair of numbers being added (input)
                floats: true if the given numbers are floating-point input)
                line_number: line number of the addition operation (input)
*/
            template <typename T>
            void operator()(const pair<T, T>& nums, const bool floats, const uint32_t line_number) const {
//              Retrieve the relevant overflow threshold values.
                T max64 = floats ? MAX_ACCURATE_FLOAT64 : MAX_INT64;
                T min64 = floats ? MIN_ACCURATE_FLOAT64 : MIN_INT64;

//              Check for overflow.
//                  The first condition checks that adding a positive number will not overflow upwards,
//                  the second condition checks that adding a negative number will not overflow downwards.
                if (((nums.second > 0) && (nums.first > max64 - nums.second)) || ((nums.second < 0) && (nums.first < min64 - nums.second))) {
                    throw OverflowError("overflow when adding " + num_to_string<T>(nums.first, floats) + " to " 
                                        + num_to_string<T>(nums.second, floats), line_number);
                }

                return;
            }
    };


            /*      SUBTRACTION      */

//  Operator object to subtract two values.
    class numSubtract {
        public:

/*
            Subtract the given values. This operator depends on a typename template.

            This operator assumes that the given type is subtractable.

            Parameters:
                n1: first number (input)
                n2: second number (input)

            Return the subtraction.
*/
            template <typename T>
            inline constexpr T operator()(const T n1, const T n2) const noexcept {
                return n1 - n2;
            }
    };

//  Operator object to check the identities of subraction.
    class subtractId {
        public:

/*
            Check subtraction idenities of the given pair of numbers from a binary operator. 
            This operator depends on a typename template for the given numbers.
            Update the given value data with the identity result and update the id_output with the a 
            pair containing information about optimization and type.

            This operator assumes that the given optimization status booleans accurately represent whether
            each expression of the given binary operator are optimized. Furthermore, it assumes that the given 
            types accurately represent the type of each expression of the given binary operator.

            Parameters:
                nums: pair of numbers being subtracted (input)
                opt_expr1/2: true if the respective binary operator expression was optimized (input)
                type1/2: type of each binary operator expression (input)
                binary_op: pointer to a binary operator object (input)
                value_data: reference to the current optimizable object (input/output)
                id_output: output parameter pair containing
                            first: true if the value data object is optimized
                            second: the type of the value data object (output)

            Return true if an identity was satisfied.
*/
            template <typename T>
            inline const bool operator()(const pair<T, T>& nums, const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, 
                                         const binaryOp* binary_op, shared_ptr<valueData>& value_data, pair<bool, dataType>& id_output) const noexcept {
//              Check the subtraction identity if the second expression was optimized.
                if (opt_expr2 && (nums.second == 0)) { // x - 0
                    value_data = binary_op->expression1;
                    id_output = make_pair(opt_expr1, type1);
                    return true;
                }

                return false;
            }
    };

//  Operator object to check for overflow during subtraction.
    class subtractOverflow {
        public:

/*
            Check if the given pair of numbers would cause overflow when subtracted. This operator
            depends on a typename template for the given numbers.

            This function assumes that the given boolean accurately represents whether the given numbers are floats.

            Throw an exception if the result would cause overflow.

            Parameters:
                nums: pair of numbers being subtracted (input)
                floats: true if the given numbers are floating-point input)
                line_number: line number of the subtraction operation (input)
*/
            template <typename T>
            void operator()(const pair<T, T>& nums, const bool floats, const uint32_t line_number) const {
//              Retrieve the relevant overflow threshold values.
                T max64 = floats ? MAX_ACCURATE_FLOAT64 : MAX_INT64;
                T min64 = floats ? MIN_ACCURATE_FLOAT64 : MIN_INT64;

//              Check for overflow. 
//                  The first condition checks that subtracting a positive number from the first expression will not overflow downwards,
//                  the second condition checks that subtracting a negative number from the first expession will not overflow upwards.
                if (((nums.second > 0) && (nums.first < min64 + nums.second)) || ((nums.second < 0) && (nums.first > max64 + nums.second))) {
                    throw OverflowError("overflow when subtracting " + num_to_string<T>(nums.first, floats) + " from " 
                                        + num_to_string<T>(nums.second, floats), line_number);
                }

                return;
            }
    };

/*
    Determine if the sign of the product of the given values is positive.
    This function depends on a typename template for the given values.

    Parameters:
        num1: the first number in the product (input)
        num2: the second number in the product (input)

    Return true if the product would be positive, false if it would be negative or 0.
*/
    template <typename T>
    inline constexpr bool _positive_product(const T num1, const T num2) noexcept {
        return !((num1 > 0) ^ (num2 > 0));
    }

            /*      MULTIPLICATION      */

//  Operator object to multiply two values.
    class numMult {
        public:

/*
            Multiply the given values. This operator depends on a typename template.

            This operator assumes that the given type is multipicative.

            Parameters:
                n1: first number (input)
                n2: second number (input)

            Return the product.
*/
            template <typename T>
            inline constexpr T operator()(const T n1, const T n2) const noexcept {
                return n1 * n2;
            }
    };

//  Operator object to check the identities of multiplication.
    class multId {
        public:

/*
            Check multiplication idenities of the given pair of numbers from a binary operator. 
            This operator depends on a typename template for the given numbers.
            Update the given value data with the identity result and update the id_output with the a 
            pair containing information about optimization and type.

            This operator assumes that the given optimization status booleans accurately represent whether
            each expression of the given binary operator are optimized. Furthermore, it assumes that the given 
            types accurately represent the type of each expression of the given binary operator.

            Parameters:
                nums: pair of numbers being multiplied (input)
                opt_expr1/2: true if the respective binary operator expression was optimized (input)
                type1/2: type of each binary operator expression (input)
                binary_op: pointer to a binary operator object (input)
                value_data: reference to the current optimizable object (input/output)
                id_output: output parameter pair containing
                            first: true if the value data object is optimized
                            second: the type of the value data object (output)

            Return true if an identity was satisfied.
*/
            template <typename T>
            const bool operator()(const pair<T, T>& nums, const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, 
                                  const binaryOp* binary_op, shared_ptr<valueData>& value_data, pair<bool, dataType>& id_output) const noexcept {

//              Check multiplication identities depending on which expression was optimized.
                if (opt_expr1) {
                    if (nums.first == 0) { // 0 * x
                        value_data = make_shared<int32Container>(binary_op->expression1->line_number, 0);
                        id_output = make_pair(true, dataType::Int32T);
                        return true;
                    } else if (nums.first == 1) { // 1 * x
                        value_data = binary_op->expression2;
                        id_output = make_pair(opt_expr2, type2);
                        return true;
                    }
                }
                if (opt_expr2) {
                    if (nums.second == 0) { // x * 0
                        value_data = make_shared<int32Container>(binary_op->expression1->line_number, 0);
                        id_output = make_pair(true, dataType::Int32T);
                        return true;
                    } else if (nums.second == 1) { // x * 1
                        value_data = binary_op->expression1;
                        id_output = make_pair(opt_expr1, type1);
                        return true;
                    }
                }

                return false;
            }
    };

//  Operator object to check for overflow during multiplication.
    class multOverflow {
        public:

/*
            Check if the given pair of numbers would cause overflow when multiplied. This operator
            depends on a typename template for the given numbers.

            This function assumes that the given boolean accurately represents whether the given numbers are floats.

            Throw an exception if the result would cause overflow.

            Parameters:
                nums: pair of numbers being multiplied (input)
                floats: true if the given numbers are floating-point input)
                line_number: line number of the multiplication operation (input)
*/
            template <typename T>
            void operator()(const pair<T, T>& nums, const bool floats, const uint32_t line_number) {
//              Retrieve the relevant overflow threshold values.
                T max64 = floats ? MAX_ACCURATE_FLOAT64 : MAX_INT64;
                T min64 = floats ? MIN_ACCURATE_FLOAT64 : MIN_INT64;

//              Retrieve the sign of the product.
                const bool pos = _positive_product<T>(nums.first, nums.second);

//              Check for overflow.
//                  The first condition checks that multiplying with a positive result will not overflow upwards,
//                  the second condition checks that multiplying with a negative result will not overflow downwards.
                if ((pos && (nums.first > max64 / nums.second)) || (!pos && (nums.first < min64 / nums.second))) {
                    throw OverflowError("overflow when multiplying " + num_to_string<T>(nums.first, floats) + " with " 
                                        + num_to_string<T>(nums.second, floats), line_number);
                }

                return;
            }
    };


//  Note that division and exponentiation always operate on type double. So, they do not
//  need wrapper classes for their implementations.

                /*      DIVISION      */

/*
    Divide the given values.

    This function assumes that n2 is not 0.

    Parameters:
        n1: first number (input)
        n2: second number (input)

    Return the quotient.
*/
    inline const double _float_div(const double n1, const double n2) noexcept {
        return n1 / n2;
    }

/*
    Check divsion idenities of the given pair of numbers from a binary operator. 
    Update the given value data with the identity result and update the id_output with the a 
    pair containing information about optimization and type.

    This function assumes that the given optimization status booleans accurately represent whether
    each expression of the given binary operator are optimized.

    Throw an exeption if the second expression was optimized and the second number is 0.

    Parameters:
        num1/2: numbers being divided (input)
        opt_expr1/2: true if the respective binary operator expression was optimized (input)
        type1: type of the first binary operator expression (input)
        binary_op: pointer to a binary operator object (input)
        value_data: reference to the current optimizable object (input/output)
        id_output: output parameter pair containing
                       first: true if the value data object is optimized
                       second: the type of the value data object (output)

    Return true if an identity was satisfied.
*/
    const bool _div_id(const double num1, const double num2, const bool opt_expr1, const bool opt_expr2, const dataType type1, 
                       const binaryOp* binary_op, shared_ptr<valueData>& value_data, pair<bool, dataType>& id_output) {

//      Check division identities depending on which expression was optimized.
//      Note, expression 2 (the divisor) is checked first so that 0 / 0 throws an exception.
        if (opt_expr2) {
//          Check for division by zero.
            if (num2 == 0.0) { // x / 0
//              Include the dividend in the error mesage if it was optimized. The line number of the 0 is also included.
                throw ExecutionError((opt_expr1 
                                         ? "dividing " + num_to_string<double>(num1, true) 
                                         : "division") 
                                     + " by 0" , binary_op->expression2->line_number);
            } else if (num2 == 1.0) { // x / 1
                value_data = binary_op->expression1;
//              Default to a 64-bit float.
                dataType new_type = dataType::Float64T;

//              Ensure that identity computations also result in a floating-point value for type consistency.
                if (opt_expr1) {
//                  Wrap the value data in order to maintain a consistent floating-point type for division.
                    new_type = _wrap_to_float(type1, value_data);
                }

                id_output =  make_pair(opt_expr1, new_type);
                return true;
            }
        }    
        if ((opt_expr1) && (num1 == 0.0)) { // 0 / x
            value_data = make_shared<float32Container>(binary_op->expression1->line_number, 0.0);
            id_output = make_pair(true, dataType::Float32T);
            return true;
        }
        
        return false;
    }

/*
    Check if the given pair of numbers would cause overflow when divided.

    This function assumes that the given boolean accurately represents whether the given numbers are floats.

    Throw an exception if the result would cause overflow.

    Parameters:
        num1/2: numbers being divided (input)
        floats: true if the given numbers are floating-point input)
        line_number: line number of the division operation (input)
*/
    void _div_overflow(const double num1, const double num2, const uint32_t line_number) {
//      Retrieve the sign of the quotient.
        const bool pos = _positive_product(num1, num2);

//      Check for floating-point overflow.
//          The first condition checks that dividing with a positive result will not overflow upwards,
//          the second condition checks that dividing with a negative result will not overflow downwards.
        if ((pos && (num1 > MAX_ACCURATE_FLOAT64 * num2)) || (!pos && (num1 < MIN_ACCURATE_FLOAT64 * num2))) {
            throw OverflowError("overflow when dividing " + num_to_string<double>(num1, true) + " by " 
                                + num_to_string<double>(num2, true), line_number);
        }
    }


                /*      EXPONENTIATION      */

/*
    Calculate the exponential of two floats.

    This function assumes that the result of exponentiation will not overflow a 64-bit float.
    It also assumes that if the given base is negative, the given exponent is a whole number.

    Parameters:
        base: base value (input)
        exp: exponent value (input)

    Return the result of exponentiation in a 64-bit float.
*/
    inline const double _exp(const double base, const double exp) noexcept {
        return pow(base, exp);
    }

/*
    Check exponential idenities of the given pair of numbers from a binary operator. 
    Update the given value data with the identity result and update the id_output with the a 
    pair containing information about optimization and type.

    This function assumes that the given optimization status booleans accurately represent whether
    each expression of the given binary operator are optimized.

    Parameters:
        num1/2: numbers being divided (input)
        opt_expr1/2: true if the respective binary operator expression was optimized (input)
        type: type of the first binary operator expression (input)
        binary_op: pointer to a binary operator object (input)
        value_data: reference to the current optimizable object (input/output)
        id_output: output parameter pair containing
                       first: true if the value data object is optimized
                       second: the type of the value data object (output)

    Return true if an identity was satisfied.
*/
    const bool _exp_id(const double num1, const double num2, const bool opt_expr1, const bool opt_expr2, const dataType type1, 
                       const binaryOp* binary_op, shared_ptr<valueData>& value_data, pair<bool, dataType>& id_output) noexcept {
        
//      Check exponential identities depending on which expression was optimized.
//      Note, expression 2 (the exponent) is checked first so that 0 ** 0 = 1.
        if (opt_expr2) {
            if (num2 == 0) { // x ** 0
                value_data = make_shared<float32Container>(binary_op->expression1->line_number, 1.0);
                id_output = make_pair(true, dataType::Float32T);
                return true;
            } else if (num2 == 1) { // x ** 1
                value_data = binary_op->expression1;
//              Default to a 64-bit float.
                dataType new_type = dataType::Float64T;

//              Ensure that identity computations also result in a floating-point value for type consistency.
                if (opt_expr1) {
//                  Wrap the result in order to maintain a consistent floating-point type for exponentiation.
                    new_type = _wrap_to_float(type1, value_data);
                }

                id_output = make_pair(opt_expr1, new_type);
                return true;
            }
        }
        if (opt_expr1) {
            if (num1 == 0) { // 0 ** x
                value_data = make_shared<float32Container>(binary_op->expression1->line_number, 0.0);
                id_output = make_pair(true, dataType::Float32T);
                return true;
            } else if (num1 == 1) { // 1 ** x
                value_data = make_shared<float32Container>(binary_op->expression1->line_number, 1.0);
                id_output = make_pair(true, dataType::Float32T);
                return true;
            }
        }

        return false;
    }

/*
    Check if the given pair of numbers would cause overflow when exponentiated.

    This function assumes that the given boolean accurately represents whether the given numbers are floats.

    Throw an exception if the result would cause overflow or if num1 is negative and num2 is not a whole number.

    Parameters:
        num1/2: numbers being divided (input)
        floats: true if the given numbers are floating-point input)
        line_number: line number of the exponential operation (input)
*/
    void _exp_overflow(const double num1, const double num2, const uint32_t line_number) {
//      First, check for an invalid number combination like -1 ** 0.5.
        if ((num1 < 0.0) && (static_cast<int64_t>(num2) != num2)) {
//          Throw an exception if imaginary numbers would result.
            throw ExecutionError("invalid negative base with non-integer exponent: " + num_to_string<double>(num1, true) 
                                 + "^" + num_to_string<double>(num2, true), line_number);
        }

//      Check for overflow using log2 of the base and comparing it to LOG_MAX_FLOAT64.
        const double log_base = log2(num1);
        const bool pos = _positive_product<double>(num1, num2);

//      Check for integer overflow.
//          The first if-statement ensures that there is no overflow when multiplying the exponent by log2(base).
//              This comes from:  b^e > MAX_FLOAT64 ==> e log2(b) > log2(MAX_FLOAT64)  when checking for overflow.
//          Check for this overflow with the same checks as in multiplication.
        if ((pos && (num2 > MAX_ACCURATE_FLOAT64 / log_base)) || (!pos && (num2 < MIN_ACCURATE_FLOAT64 / log_base))) {
            throw OverflowError("overflow from " + num_to_string<double>(num1, true) + "^" 
                                + num_to_string<double>(num2, true), line_number);
//      The second if-statement ensures that there is no overflow in the exponential by checking:
//          b^e > MAX_FLOAT64 ==> e log2(b) > log2(MAX_FLOAT64)
        } else if (num2 * log_base > LOG_MAX_FLOAT64) {
            throw OverflowError("overflow from " + num_to_string<double>(num1, true) + "^" 
                                + num_to_string<double>(num2, true), line_number);
        }

        return;
    }


        /*      BOOLEANS        */

/*
    Determine if the given boolean pair from a binary operator satisfies any identity values for logical AND.
    If an expression was optimized, check if that boolean allows for logical AND to execute without knowing the other expression.
    If an identity is found, update the given value data with the result of logical AND.

    This function assumes that the given optimization status booleans accurately represent whether
    each expression of the binary operator are optimized.

    Parameters:
        bools: pair of boolean values from a binary operator, default value is false if a boolean expression was not optimized (input)
        opt_expr1/2: true if the respective binary operator expression was optimized (input)
        line_number: the line number of the AND operation (input)
        value_data: pointer to the object to be updated if an identity is found (input)

    Return true if an identity was found and value data was updated.
*/
    inline const bool _and_identity(const pair<bool, bool>& bools, const bool opt_expr1, const bool opt_expr2, const uint32_t line_number, 
                                    shared_ptr<valueData>& value_data) noexcept {
//      Check the logical AND identity depending on which expression was optimized.
        if ((opt_expr1 && (!bools.first)) || (opt_expr2 && (!bools.second))) {
            value_data = make_shared<boolContainer>(line_number, false);
            return true;
        }

        return false;
    }

/*
    Determine if the given boolean pair from a binary operator satisfies any identity values for logical OR.
    If an expression was optimized, check if that boolean allows for logical OR to execute without knowing the other expression.
    If an identity is found, update the given value data with the result of logical OR.

    This function assumes that the given optimization status booleans accurately represent whether
    each expression of the binary operator are optimized.

    Parameters:
        bools: pair of boolean values from a binary operator, default value is false if a boolean expression was not optimized (input)
        opt_expr1/2: true if the respective binary operator expression was optimized (input)
        line_number: the line number of the OR operation (input)
        value_data: pointer to the object to be updated if an identity is found (input)

    Return true if an identity was found and value data was updated.
*/
    inline const bool _or_identity(const pair<bool, bool>& bools, const bool opt_expr1, const bool opt_expr2, const uint32_t line_number, 
                                   shared_ptr<valueData>& value_data) noexcept {
//      Check the logical OR identity depending on which expression was optimized.
        if ((opt_expr1 && (bools.first)) || (opt_expr2 && (bools.second))) {
            value_data = make_shared<boolContainer>(line_number, true);
            return true;
        }

        return false;
    }

/*
    Initialize a new scope below the given parent environment and optimize/typecheck the given code block in it.
    Update the new environment while opimizing if the given boolean is true. If the second boolean 
    is true and the code block could be fully optimized, pop the newly optimized/typechecked scope after 
    analysis is complete.

    This function assumes that the given code block is an instsance of a child class and not a data node instance.

    Throw an exception if, in the given code block,
        any code is not correct in type (e.g. an 'if' condition is not of type boolean),
        a variable is referenced without explicit assignment or is assigned explicitly twice, or
        an operator takes invalid operands (e.g. -1 ** 0.5).
    Throw a fatal error if an operator, data type, or data node class is not recognized (not implemented) in the given code block.

    Parameters:
        parent_env: pointer to environment object to create a child scope below (input/output)
        code_block: code to optimize in the newly created environment (input/output)
        update_env: true if the code block should update its new environment,
                    false if the code block should only be typechecked (input)
        pop_scope: true if the new scope should be popped after complete optimization/typechecking (input)
*/
    const bool _create_analyze_scope(const shared_ptr<environment>& parent_env, shared_ptr<dataNode>& code_block, const bool update_env, const bool pop_scope) {
//      Initialize a new environment and optimize the code block.
//      Note that the parent environment is passed to build a new child environment.
        parent_env->inner_scopes.push_back(make_shared<environment>(parent_env));
        const bool optimized_block = analyze_data_node(code_block, parent_env->inner_scopes.back(), update_env);

//      If the new scope was completely optimized, there is no need for the corresponding environment.
        if (pop_scope && optimized_block) {
            parent_env->inner_scopes.pop_back();
        }

        return optimized_block;
    }

/*
    Determine if the given types are not implicitly combinable in a binary operator.

    Parameters:
        type1: first type (input)
        type2: second type (input)

    Return true if the gvien types cannot be implicitly combined.
*/
    inline const bool _uncombinable_types(const dataType type1, const dataType type2) noexcept {
        if (type1 != type2) {
            const dataType* const numbers_beginning = number_types.begin();
            const dataType* const numbers_end = number_types.end();

//          Ensure that if the types don't match, neither is a number type (otherwise they would be combinable).
            return (find(numbers_beginning, numbers_end, type1) == numbers_end) || 
                   (find(numbers_beginning, numbers_end, type2) == numbers_end);
        }

        return false;
    }

/*
    Ensure that the given types from the given biunary operator are both number types.

    This function assumes that the given types accurately represent the types of the expressions 
    of the given binary operator.

    Throw an exception if either of the given types is not a number type.

    Parameters:
        type1/2: the types of the respective expressions of the binary operator (input)
        binary_op: pointer to the binary operator object (input)
*/
    void _binaryop_number_types(const dataType type1, const dataType type2, const binaryOp* const binary_op) {
        const dataType* const numbers_beginning = number_types.begin();
        const dataType* const numbers_end = number_types.end();
        const tokenKey oper = binary_op->op;

        if (find(numbers_beginning, numbers_end, type1) == numbers_end) {
            const uint32_t type1_line_number = binary_op->expression1->line_number;
            throw TypeMismatchError(display_token(make_tuple(oper, false, type1_line_number), !(oper == tokenKey::If)) + " operator is invalid " 
                                    + "with expression of type " + display_type(type1, type1_line_number), type1_line_number);
        } else if (find(numbers_beginning, numbers_end, type2) == numbers_end) {
            const uint32_t type2_line_number = binary_op->expression2->line_number;
            throw TypeMismatchError(display_token(make_tuple(oper, false, type2_line_number), !(oper == tokenKey::If)) + " operator is invalid " 
                                    + "with expression of type " + display_type(type2, type2_line_number), type2_line_number);
        }

        return;
    }

/*
    Extract boolean values from the expressions of a given binary operatore when possible.
    If either expression is not known before runtime, that boolean value defaults to false.

    This function assumes that the given binary operator contains a boolean container in each expression that was optimized.
        e.g. if the given boolean representing expression 1 is true, then the first expression of the binary operator
             must be a boolean container.

    Parameters:
        optimized1: true if the first expression of the binary operator has been optimized fully (input)
        optimized2: true if the second expression of the binary operator has been optimized fully (input)
        binary_op: a pointer to the binary operator object (input)

    Return a pair containing 
        first: the boolean value of the first expression, default false if it was not optimized
        second: the boolean value of the second expression, default false if it was not optimized
*/
    inline const pair<const bool, const bool> _binaryop_booleans(const bool optimized1, const bool optimized2, const binaryOp* binary_op) noexcept {
//      Default values are false.
        bool val1 = false;
        bool val2 = false;

        if (optimized1) {
            const boolContainer* const bool1 = dynamic_cast<boolContainer*>(binary_op->expression1.get());
            val1 = bool1->boolean;
        }
        if (optimized2) {
            const boolContainer* const bool2 = dynamic_cast<boolContainer*>(binary_op->expression2.get());
            val2 = bool2->boolean;
        }

        return make_pair(val1, val2);
    }

/*
    Extract numbers of expresssions from a given binary operator and store their values when possible.
    The given booleans determine whether the binary operator has expressions from which number values can be extracted from,
    if a boolean is false, that number value defaults to 0.0. Store the numbers in the numbers pair as either 64-bit integers 
    or 64-bit floats (wrapped in a variant). Use the given types to determine what type of number to extract.
    
    This function assumes that the given types and optimization booleans accurately represent the expressions of the given binary operator.
        e.g. if type1 is Int32T and optimized1 is true, then expression1 of the binary operator cannot be a float container.
    
    Parameters:
        type1: the type of the first expression of the binary operator (input)
        type2: the type of the second expression of the binary operator (input)
        optimized1: true if the first expression of the binary operator was optrimized down to a single node (input)
        optimized2: true if the second expression of the binary operator was optrimized down to a single node (input)
        binary_op: pointer to the binary operator object (input)
        numbers: variant containing a pair of 64-bit integers or 64-bit floats to store the extracted numbers (output)

    Return true if the extracted values have been stored in a pair of floats, false if they are in a pair of integers.
*/
    const bool _binaryop_numbers(const dataType type1, const dataType type2, const bool optimized1, const bool optimized2, const binaryOp* binary_op, 
                                 variant<pair<int64_t, int64_t>, pair<double, double>>& numbers) noexcept {
//      Each number can be either a float of integer, so use a variant to temporarily store them.
        variant<double, int64_t> num1 = 0.0;
        variant<double, int64_t> num2 = 0.0;
        bool doubles = true;

//      Retrieve the value of each expression depending on their types and optimization status.
//      Each number value is stored as the largest possible type temporarily, and will be cast down later if possible.
//          e.g. a Float32T and Int32T will result in both being cast to a 64-bit float to be combined, then later cast down.

//      Two 32-bit integers.
        if ((type1 == dataType::Int32T) && (type2 == dataType::Int32T)) {
            num1 = 0LL;
            num2 = 0LL;

            if (optimized1) {
                const int32Container* const int1 = dynamic_cast<int32Container*>(binary_op->expression1.get());
                num1 = static_cast<int64_t>(int1->number);
            }
            if (optimized2) {
                const int32Container* const int2 = dynamic_cast<int32Container*>(binary_op->expression2.get());
                num2 = static_cast<int64_t>(int2->number);
            }

            doubles = false;

//      One 32-bit integer and one 64-bit integer.
        } else if (((type1 == dataType::Int32T) && (type2 == dataType::Int64T)) || 
                   ((type1 == dataType::Int64T) && (type2 == dataType::Int32T))) {
            
            num1 = 0LL;
            num2 = 0LL;

//          The first value is 32-bit and the second is 64-bit.
            if (type1 == dataType::Int32T) {
                if (optimized1) {
                    const int32Container* const int1 = dynamic_cast<int32Container*>(binary_op->expression1.get());
                    num1 = static_cast<int64_t>(int1->number);
                }
                if (optimized2) {
                    const int64Container* const int2 = dynamic_cast<int64Container*>(binary_op->expression2.get());
                    num2 = int2->number;
                }

//          The first value is 64-bit and the second is 32-bit.
            } else {
                if (optimized1) {
                    const int64Container* const int1 = dynamic_cast<int64Container*>(binary_op->expression1.get());
                    num1 = int1->number;
                }
                if (optimized2) {
                    const int32Container* const int2 = dynamic_cast<int32Container*>(binary_op->expression2.get());
                    num2 = static_cast<int64_t>(int2->number);
                }
            }

            doubles = false;

//      One 32-bit integer and one 32-bit float.
        } else if (((type1 == dataType::Int32T) && (type2 == dataType::Float32T)) || 
                   ((type1 == dataType::Float32T) && (type2 == dataType::Int32T))) {

//          The first value is an integer and the second is a float.
            if (type1 == dataType::Int32T) {
                if (optimized1) {
                    const int32Container* const int1 = dynamic_cast<int32Container*>(binary_op->expression1.get());
                    num1 = static_cast<double>(int1->number);
                }
                if (optimized2) {
                    const float32Container* const float1 = dynamic_cast<float32Container*>(binary_op->expression2.get());
                    num2 = static_cast<double>(float1->number);
                }

//          The first value is a float and the second is an integer.
            } else {
                if (optimized1) {
                    const float32Container* const float1 = dynamic_cast<float32Container*>(binary_op->expression1.get());
                    num1 = static_cast<double>(float1->number);
                }
                if (optimized2) {
                    const int32Container* const int1 = dynamic_cast<int32Container*>(binary_op->expression2.get());
                    num2 = static_cast<double>(int1->number);
                }
            }

//      One 32-bit integer and one 64-bit float.
        } else if (((type1 == dataType::Int32T) && (type2 == dataType::Float64T)) || 
                   ((type1 == dataType::Float64T) && (type2 == dataType::Int32T))) {

//          The first value is an integer and the second is a float.
            if (type1 == dataType::Int32T) {
                if (optimized1) {
                    const int32Container* const int1 = dynamic_cast<int32Container*>(binary_op->expression1.get());
                    num1 = static_cast<double>(int1->number);
                }
                if (optimized2) {
                    const float64Container* const float1 = dynamic_cast<float64Container*>(binary_op->expression2.get());
                    num2 = float1->number;
                }

//          The first value is a float and the second is an integer.
            } else {
                if (optimized1) {
                    const float64Container* const float1 = dynamic_cast<float64Container*>(binary_op->expression1.get());
                    num1 = float1->number;
                }
                if (optimized2) {
                    const int32Container* const int1 = dynamic_cast<int32Container*>(binary_op->expression2.get());
                    num2 = static_cast<double>(int1->number);
                }
            }
            
//      Two 32-bit floats.
        } else if ((type1 == dataType::Float32T) && (type2 == dataType::Float32T)) {
            if (optimized1) {
                const float32Container* const float1 = dynamic_cast<float32Container*>(binary_op->expression1.get());
                num1 = static_cast<double>(float1->number);
            }
            if (optimized2) {
                const float32Container* const float2 = dynamic_cast<float32Container*>(binary_op->expression2.get());
                num2 = static_cast<double>(float2->number);
            }

//      One 32-bit float and one 64-bit integer.
        } else if (((type1 == dataType::Float32T) && (type2 == dataType::Int64T)) || 
                ((type1 == dataType::Int64T) && (type2 == dataType::Float32T))) {

//          The first value is a float and the second is an integer.
            if (type1 == dataType::Float32T) {
                if (optimized1) {
                    const float32Container* const float1 = dynamic_cast<float32Container*>(binary_op->expression1.get());
                    num1 = static_cast<double>(float1->number);
                }
                if (optimized2) {
                    const int64Container* const int1 = dynamic_cast<int64Container*>(binary_op->expression2.get());
                    num2 = static_cast<double>(int1->number);
                }

//          The first value is an integer and the second is a float.
            } else {
                if (optimized1) {
                    const int64Container* const int1 = dynamic_cast<int64Container*>(binary_op->expression1.get());
                    num1 = static_cast<double>(int1->number);
                }
                if (optimized2) {
                    const float32Container* const float1 = dynamic_cast<float32Container*>(binary_op->expression2.get());
                    num2 = static_cast<double>(float1->number);
                }
            }

//      One 32-bit float and one 64-bit float.
        } else if (((type1 == dataType::Float32T) && (type2 == dataType::Float64T)) || 
                ((type1 == dataType::Float64T) && (type2 == dataType::Float32T))) {

//          The first value is 32-bit and the second is 64-bit.
            if (type1 == dataType::Float32T) {
                if (optimized1) {
                    const float32Container* const float1 = dynamic_cast<float32Container*>(binary_op->expression1.get());
                    num1 = static_cast<double>(float1->number);
                }
                if (optimized2) {
                    const float64Container* const float2 = dynamic_cast<float64Container*>(binary_op->expression2.get());
                    num2 = float2->number;
                }

//          The first value is 64-bit and the second is 32-bit.
            } else {
                if (optimized1) {
                    const float64Container* const float1 = dynamic_cast<float64Container*>(binary_op->expression1.get());
                    num1 = float1->number;
                }
                if (optimized2) {
                    const float32Container* const float2 = dynamic_cast<float32Container*>(binary_op->expression2.get());
                    num2 = static_cast<double>(float2->number);
                }
            }

//      One 64-bit float and one 64-bit integer.
        } else if (((type1 == dataType::Float64T) && (type2 == dataType::Int64T)) || 
                ((type1 == dataType::Int64T) && (type2 == dataType::Float64T))) {

//          The first value is a float and the second is an integer.
            if (type1 == dataType::Float64T) {
                if (optimized1) {
                    const float64Container* const float1 = dynamic_cast<float64Container*>(binary_op->expression1.get());
                    num1 = float1->number;
                }
                if (optimized2) {
                    const int64Container* const int1 = dynamic_cast<int64Container*>(binary_op->expression2.get());
                    num2 = static_cast<double>(int1->number);
                }

//          The first value is an integer and the second is a float.
            } else {
                if (optimized1) {
                    const int64Container* const int1 = dynamic_cast<int64Container*>(binary_op->expression1.get());
                    num1 = static_cast<double>(int1->number);
                }
                if (optimized2) {
                    const float64Container* const float1 = dynamic_cast<float64Container*>(binary_op->expression2.get());
                    num2 = float1->number;
                }
            }
    
//      Two 64-bit floats.
        } else if ((type1 == dataType::Float64T) && (type2 == dataType::Float64T)) {
            if (optimized1) {
                const float64Container* const float1 = dynamic_cast<float64Container*>(binary_op->expression1.get());
                num1 = float1->number;
            }
            if (optimized2) {
                const float64Container* const float2 = dynamic_cast<float64Container*>(binary_op->expression2.get());
                num2 = float2->number;
            }

//      Two 64-bit integers.
        } else if ((type1 == dataType::Int64T) && (type2 == dataType::Int64T)) {

            num1 = 0LL;
            num2 = 0LL;

            if (optimized1) {
                const int64Container* const int1 = dynamic_cast<int64Container*>(binary_op->expression1.get());
                num1 = int1->number;

            }
            if (optimized2) {
                const int64Container* const int2 = dynamic_cast<int64Container*>(binary_op->expression2.get());
                num2 = int2->number;
            }

            doubles = false;
        }

//      Store the values in the output parameter.
        if (doubles) {
            numbers = make_pair(get<double>(num1), get<double>(num2));
        } else {
            numbers = make_pair(get<int64_t>(num1), get<int64_t>(num2));
        }

        return doubles;
    }
    
/*
    Perform semantic analysis (optimization and typechecking) on the expressions of a given binary operator and 
    compute relevant data from that analysis.

    This function assumes that the given binary operator's expressions are child class members and not an actual 
    instances of the value data class.

    Throw an exception if, for either of the binary operator's expressions,
        any code is not correct in type (e.g. adding '4' to 'false'),
        a variable is referenced without explicit assignment, or
        an operator takes invalid operands (e.g. -1 ** 0.5).
    Throw a fatal error if an operator, data type, or value data class is not recognized (not implemented).

    Parameters:
        binary_op: pointer to the binary operator object (input/output)
        scope_env: environment to optimize the binary operator's expressions in (input)
    
    Return a 4-tuple containing
        <0>: true if the first expression was fully optimized
        <1>: true if the second expression was fully optimized
        <2>: the data type of the first expression
        <3>: the data type of the second expression
*/
    const tuple<bool, bool, dataType, dataType> _binaryop_analyze(binaryOp* binary_op, shared_ptr<environment>& scope_env) {
        bool opt1, opt2;
        dataType type1, type2;

//      Optimize both expressions and store their data.
        tie(opt1, type1) = analyze_value_data(binary_op->expression1, scope_env);
        tie(opt2, type2)  = analyze_value_data(binary_op->expression2, scope_env);

        return make_tuple(opt1, opt2, type1, type2);
    }

/*
    Analyze a comparison operation given all necessary data.
    This function depends on a typename and comparator template, it assumes that the given type is either int64_t or double
    and that the given comparator evaluates for two of the given type.
    Update the current optimizable object if the binary operator's expressions have been optimized.

    This function also assumes that the given optimization status booleans accurately represent whether
    each expression of the binary operator are optimized.

    Parameters:
        vals: pair of values to compare (input)
        opt_expr1/2: true if the respective binary operator's expressions were optimized (input)
        line_number: line number of the comparison operation (input)
        comp: comparator functional to use on the given values (input)
        value_data: optimizable object to be updated (input/output)

    Return a pair containing
        <0>: true if the comparison was optimized
        <1>: the type of the result (boolean)
*/
    template <typename Comparator>
    const pair<bool, dataType> _analyze_comp_operation(const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, const tokenKey comp_token, 
                                                       const Comparator comp, const binaryOp* binary_op, shared_ptr<valueData>& value_data) noexcept {
        variant<pair<long long, long long>, pair<double, double>> vals;

//      Ensure that expressions are each a number type.
        _binaryop_number_types(type1, type2, binary_op);

//      Stop if either expression was not optimized.
        if (!(opt_expr1 && opt_expr2)) {
            return make_pair(false, dataType::BoolT);
        }

//      Retrieve 64-bit implementations of numbers.
        _binaryop_numbers(type1, type2, opt_expr1, opt_expr2, binary_op, vals);

//      Return that the binary operator was optimized and compute the result.
//      Visit the pair of values to compute the result regardless of type.
        value_data = make_shared<boolContainer>(binary_op->expression1->line_number, 
                                                visit([&comp](const auto& value_pair) {
                                                    return comp(value_pair.first, value_pair.second);
                                                }, vals));
        return make_pair(true, dataType::BoolT);
    }

/*
    Analyze a floating-point operation given all necessary data.
    Typecheck the operation, execute it when possible, and update the given value data parameter.

    This function assumes that the given optimization status booleans accurately represent whether
    each expression of the binary operator are optimized. Furthermore, it assumes that the given data types
    accurately represent the types of the binary operator expressions.
    This function also assumes that the given function pointers and operator token all align with 
    the binary operator's operation.

    Throw an exception if 
        the binary opreator's expressions are not valid types with their operator,
        the expressions are invalid with their operator (e.g. 2 / 0), or
        the operation would cause overflow.

    Parameters:
        opt_expr1/2: true if the respective expressions of the binary operator are optimized (input)
        type1/2: the data types of the respective binary operator expressions (input)
        op_token: the token key of the binary operator's operator (input)
        float_op: a float operation function pointer, the operation to perform on the expressions (input)
        id_func: a float function pointer that checks the identities of the given operation (input)
        overf_func: a float function pointer that checks for overflow with the given operation and expressions (input)
        binary_op: pointer to the binary operator object (input)
        value_data: optimizable object to update if the operation was executed (input/output)
    
    Return a pair containing
        first: true if the given operation was executed
        second: the type of the operation result
*/
    const pair<bool, dataType> _analyze_float_operation(const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, const floatOpFunc float_op, 
                                                  const floatIdFunc id_func, const floatOverflowFunc overf_func, const binaryOp* binary_op, shared_ptr<valueData>& value_data) {
        
        variant<pair<int64_t, int64_t>, pair<double, double>> nums;
        pair<bool, dataType> id_output;
        double num1, num2;

//      Ensure that expressions are each a number type.
        _binaryop_number_types(type1, type2, binary_op);

//      Retrieve 64-bit implementations of numbers.
        const bool floats = _binaryop_numbers(type1, type2, opt_expr1, opt_expr2, binary_op, nums);

//      Retrieve floating-point values of the numbers.
        if (!floats) {
            const pair<int64_t, int64_t> int_vals = get<pair<int64_t, int64_t>>(nums);
            num1 = static_cast<double>(int_vals.first);
            num2 = static_cast<double>(int_vals.second);
        } else {
            const pair<double, double> float_vals = get<pair<double, double>>(nums);
            num1 = float_vals.first;
            num2 = float_vals.second;
        }

//      Check the identities of the given operator.
        if (id_func(num1, num2, opt_expr1, opt_expr2, type1, binary_op, value_data, id_output)) {
//          Return the status of optimization and type if an identity was evaluated.
            return id_output;
        }

//      Stop if either expression was not optimized. Default the type to a 64-bit float.
        if (!(opt_expr1 && opt_expr2)) {
            return make_pair(false, dataType::Float64T);
        }

//      Check if the operation would cause overflow/has other errors.
        overf_func(num1, num2, binary_op->expression2->line_number);

//      Safely perform the operation.
        const double result = float_op(num1, num2);

//      Return that the binary operator was optimized and wrap the result in a smaller data type if possible.
        return make_pair(true, _wrap_number_data(result, true, binary_op->expression1->line_number, value_data));
    }

/*
    Attempt to execute a math operation given all necessary data.
    Execute the operation when possible and store its result in the value data parameter.
    This function depends on typename templates
        T: the type of the numbers, this must be compatible with the operator objects
        MathOp: a templated operator object type
        OperId: a templated operator identity-checker type
        OverflowFunc: a templated operator overflow checker

    This function assumes that the given optimization status booleans accurately represent whether
    each expression of the binary operator are optimized. Furthermore, it assumes that the given data types
    accurately represent the types of the binary operator expressions.
    This function also assumes that the given operator objects align with the binary operator's operator.

    Throw an exception if the operation would cause overflow.

    Parameters:
        nums: pair of numbers to perform the operation on (input)
        opt_expr1/2: true if the respective expression of the binary operator are optimized (input)
        type1/2: the data types of the respective binary operator expressions (input)
        floats: true if the given numbers are floats (input)
        math_op: a templated operator object that performs the operation on the given numbers (input)
        id_func: a templated operator object that checks the identities of the given operation (input)
        overf_func: a templated operator object that checks for overflow with the given operation and numbers (input)
        binary_op: pointer to the binary operator object (input)
        value_data: optimizable object to update if the operation was executed (input/output)
    
    Return a pair containing
        first: true if the given operation was executed
        second: the type of the operation result
*/
    template <typename T, typename MathOp, typename OperId, typename OverflowFunc>
    const pair<bool, dataType> _compute_generic_math_operation(pair<T, T>& nums, const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, const bool floats,  
                                                         const MathOp math_op, const OperId id_func, OverflowFunc overf_func, const binaryOp* binary_op, shared_ptr<valueData>& value_data) {
        
        pair<bool, dataType> id_output;

//      Check for identities of the given operator using a templated identity checker.
        if (id_func.template operator()<T>(nums, opt_expr1, opt_expr2, type1, type2, binary_op, value_data, id_output)) {
//          Return the status of optimization and type if an identity was evaluated.
            return id_output;
        }
        
//      Stop if either expression was not optimized. Default the type to a 64-bit number.
        if (!(opt_expr1 && opt_expr2)) {
            return make_pair(false, floats ? dataType::Float64T : dataType::Int64T);
        }

//      Check for overflow with the given values and operator using a templated overflow checker.
        overf_func.template operator()<T>(nums, floats, binary_op->expression1->line_number);

//      Safely perform the operation on the given numebrs using a templated operator.
        const T result = math_op.template operator()<T>(nums.first, nums.second);

//      Return that the binary operator was optimized and wrap the result in a smaller data type if possible.
        return make_pair(true, _wrap_number_data(result, floats, binary_op->expression1->line_number, value_data));
    }

/*
    Analyze a math operation given all necessary data.
    Typecheck the operation, execute it when possible and store the result in the value data parameter.
    This function depends on typename templates
        MathOp: a templated operator object type
        OperId: a templated operator identity-checker type
        OverflowFunc: a templated operator overflow checker

    This function assumes that the given optimization status booleans accurately represent whether
    each expression of the binary operator are optimized. Furthermore, it assumes that the given data types
    accurately represent the types of the binary operator expressions.
    This function also assumes that the given operator objects align with the binary operator's operator.

    Throw an exception if 
        the types are not valid with the given operator or    
        the operation would cause overflow.

    Parameters:
        opt_expr1/2: true if the respective expression of the binary operator are optimized (input)
        type1/2: the data types of the respective binary operator expressions (input)
        math_token: token key of the given operation (input)
        math_op: a templated operator object that performs the operation on the binary operator's expressions (input)
        id_func: a templated operator object that checks the identities of the given operation (input)
        overf_func: a templated operator object that checks for overflow with the given operation and expressions (input)
        binary_op: pointer to the binary operator object (input)
        value_data: optimizable object to update if the operation was executed (input/output)
    
    Return a pair containing
        first: true if the given operation was executed
        second: the type of the operation result
*/
    template <typename MathOp, typename OperId, typename OverflowFunc>
    const pair<bool, dataType> _generic_math_operation(const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, const MathOp math_op, 
                                                       const OperId id_func, const OverflowFunc overf_func, const binaryOp* binary_op, shared_ptr<valueData>& value_data) {

        variant<pair<int64_t, int64_t>, pair<double, double>> nums;

//      Ensure that the expressions are each a number type.
        _binaryop_number_types(type1, type2, binary_op);

//      Retrieve 64-bit implementations of numbers.
        const bool floats = _binaryop_numbers(type1, type2, opt_expr1, opt_expr2, binary_op, nums);

        if (floats) {
//          Retrieve the floats from the numbers pair and perform the operation.
            return _compute_generic_math_operation<double>(get<pair<double, double>>(nums), opt_expr1, opt_expr2, type1, type2, floats, 
                                                           math_op, id_func, overf_func, binary_op, value_data);
        }

//      Retrieve the integers from the numbers pair and perform the operation.
        return _compute_generic_math_operation<int64_t>(get<pair<int64_t, int64_t>>(nums), opt_expr1, opt_expr2, type1, type2, floats, 
                                                        math_op, id_func, overf_func, binary_op, value_data);
    }

/*
    Analyze a comparison operation given all necessary data.
    This function depends on a typename and comparator template, it assumes that the given type is either int64_t or double
    and that the given comparator evaluates for two of the given type.
    Update the current optimizable object if the binary operator's expressions have been optimized.

    This function also assumes that the given optimization status booleans accurately represent whether
    each expression of the binary operator are optimized.

    Parameters:
        vals: pair of values to compare (input)
        opt_expr1/2: true if the respective binary operator's expression was optimized (input)
        line_number: line number of the comparison operation (input)
        comp: comparator functional to use on the given values (input)
        value_data: optimizable object to be updated (input/output)

    Return a pair containing
        first: true if the comparison was optimized
        second: the type of the result (boolean)
*/
    template <typename BoolOp>
    pair <bool, dataType> _analyze_bool_operation(const bool opt_expr1, const bool opt_expr2, const dataType type1, const dataType type2, 
                                                  BoolOp bool_op, boolIdFunc id_func, binaryOp* binary_op, shared_ptr<valueData>& value_data) {
//      Ensure that both expressions are boolean.
        if ((type1 != dataType::BoolT) || (type2 != dataType::BoolT)) {
            throw TypeMismatchError(binary_op->op, true, (type1 == dataType::BoolT ? type2 : type1), dataType::BoolT, true,
                                   (type1 == dataType::BoolT ? binary_op->expression2->line_number : binary_op->expression1->line_number));
        }

//      Retrieve the boolean values.
        pair<bool, bool> bools = _binaryop_booleans(opt_expr1, opt_expr2, binary_op);

//      Check identities for the given operator and return if an identity was evaluated.
        if (id_func(bools, opt_expr1, opt_expr2, binary_op->expression1->line_number, value_data)) {
            return make_pair(true, dataType::BoolT);
        }

//      Return the type of the result as a boolean if one of the expressions wasn't optimized.
        if (!(opt_expr1 && opt_expr2)) {
            return make_pair(false, dataType::BoolT);
        }

//      Update the data with the operation result and return that it was optimized.
        value_data = make_shared<boolContainer>(binary_op->expression1->line_number, bool_op(bools.first, bools.second));
        return make_pair(true, dataType::BoolT);
    }

}


const bool analyze_data_node(shared_ptr<dataNode>& data_node, shared_ptr<environment>& scope_env, const bool update_env) {
//  Deduce which instance of a data node the current object is.

    switch(data_node->type) {
        case nodeType::CodeScope: {
//          Retrieve the code scope object.
            codeScope* const code_scope = dynamic_cast<codeScope*>(data_node.get());

//          Analyze the current operation and the remainder of the scope.
            const bool optimized_operation = analyze_data_node(code_scope->curr_operation, scope_env, update_env);
            const bool optimized_remainder = analyze_data_node(code_scope->remainder, scope_env, update_env);

//          Return true only if both were fully optimized.
            return optimized_operation && optimized_remainder;
        }

        case nodeType::IfBlock: {
            dataType condition_type;
            bool condition_opt;

//          Retrieve the if block object.
            ifBlock* const if_block = dynamic_cast<ifBlock*>(data_node.get());

//          Analyze the if condition and retrieve its status/type.
            tie(condition_opt, condition_type) = analyze_value_data(if_block->bool_condition, scope_env);
            
//          Throw an exception if the condition is not a boolean.
            if (condition_type != dataType::BoolT) {
                const uint32_t condition_line_number = if_block->bool_condition->line_number;
                throw TypeMismatchError(display_token(make_tuple(tokenKey::If, false, condition_line_number), true) + " condition expected type " 
                                        + display_type(dataType::BoolT, condition_line_number) + " but received type " 
                                        + display_type(condition_type, condition_line_number), condition_line_number);
            }

//          Handle the case when the condition value is known pre-runtime.
            if (condition_opt) {
//              Retrieve the condition object.
                const boolContainer* const condition = dynamic_cast<boolContainer*>(if_block->bool_condition.get());

//              Handle the case when the if condition is true.
                if (condition->boolean) {
//                  Analyze the if block. Note the update_env parameter is passed and the final parameter denotes to pop the scope after.
                    const bool optimized_if = _create_analyze_scope(scope_env, if_block->code_block, update_env, true);
                    if (if_block->contains_else) {
//                      Analyze an else block if one exists, but only typecheck it (third parameter is false).
                        _create_analyze_scope(scope_env, if_block->else_block, false, true);
                    }
                
//                  Replace the if block object with just the code under 'if'.
                    data_node = move(if_block->code_block);
                    return optimized_if;

//              Handle the case when the if condition is false.
                } else {
//                  Analyze the if block, but only typecheck.
                    _create_analyze_scope(scope_env, if_block->code_block, false, true);

                    if (if_block->contains_else) {
//                      Fully analyze the else block if one exists.
                        const bool optimized_else = _create_analyze_scope(scope_env, if_block->else_block, update_env, true);

//                      Replace the if block object with just the code under 'else'
                        data_node = move(if_block->else_block);
                        return optimized_else;
                    }

//                  If the condition is false and there is no else block, then that has automatically been fully optimized.
                    return true;
                }

//          Handle the case where the condition is not known pre-runtime.
            } else {
//              Analyze all blocks but only typecheck them. Do not pop their environments afterwards.
                _create_analyze_scope(scope_env, if_block->code_block, false, false);
                if (if_block->contains_else) {
                    _create_analyze_scope(scope_env, if_block->else_block, false, false);
                }

                return false;
            }
        }

        case nodeType::AssignOp: {
            map<string, variableInfo>::iterator iter;
            dataType expr_type;
            bool expr_opt;

//          Retrieve the assignment operation object.
            assignOp* const assign = dynamic_cast<assignOp*>(data_node.get());

//          Iterate through each local variable, then check the parent scope and repeat.
            environment* current_scope = scope_env.get();
            while (current_scope != nullptr) {
//              Check for the variable.
                iter = current_scope->locals.find(assign->variable);
//              If the variable was found, throw an exception.
                if (iter != current_scope->locals.end()) {
                    throw VariableInitializationError(assign->variable, false, assign->line_number);
                }

//              Check the parent scope.
                current_scope = current_scope->parent_scope.get();
            }

//          Analyze the expression to assign to the variable.
            tie(expr_opt, expr_type) = analyze_value_data(assign->expression, scope_env);

//          Create the variable in the local scope. Note the expression has been analyzed already.
            scope_env->locals.emplace(assign->variable, variableInfo{expr_type, assign->expression, expr_opt});
            return expr_opt;
        }

        case nodeType::ReassignOp: {
            map<string, variableInfo>::iterator iter;
            dataType expr_type;
            bool expr_opt;
            environment* const primary_env = scope_env.get();

//          Retrieve the reassign operation object.
            reassignOp* const reassign = dynamic_cast<reassignOp*>(data_node.get());

//          Iterate through each local variable, then check the parent scope and repeat.
            environment* current_scope = primary_env;
            while ((iter = current_scope->locals.find(reassign->variable)) == current_scope->locals.end()) {
                if (current_scope->parent_scope == nullptr) {
//                  Throw an exception if the variable is not found anywhere.
                    throw VariableInitializationError(reassign->variable, true, reassign->line_number);
                }
                
//              Check the parent scope.
                current_scope = current_scope->parent_scope.get();
            }

//          Analyze the expression to reassign to the variable.
            tie(expr_opt, expr_type) = analyze_value_data(reassign->expression, scope_env);

//          Ensure that the reassignment is with a type that is combinable with the original type of the variable.
            const dataType original_type = iter->second.type;
            if (_uncombinable_types(original_type, expr_type)) {
                const uint32_t reassign_line_number = reassign->line_number;
                throw TypeMismatchError("variable \'" + reassign->variable + "\' reassignment expected type " + display_type(original_type, reassign_line_number)
                                        + " but received type " + display_type(expr_type, reassign_line_number), reassign_line_number);
//          Only update the variable if we are reassigning it in its local scope or if the update boolean is true.
            } else if (update_env || (primary_env == current_scope)) {
                iter->second = {expr_type, reassign->expression, expr_opt};
            }

            return expr_opt;
        }

//      Throw an exception when a piece of data was not recognized (not implemented).
        default:
            throw FatalError("data not recognized", data_node->line_number);
    }
}


pair<bool, dataType> analyze_value_data(shared_ptr<valueData>& value_data, shared_ptr<environment>& scope_env) {
//  Deduce which child class member the value data is.

    switch (value_data->type) {
        case nodeType::UnaryOp: {
            dataType expr_type;
            bool expr_opt;

//          Retrieve the unary operator object.
            unaryOp* const unary_op = dynamic_cast<unaryOp*>(value_data.get());
            
//          Analyze the operator's expression
            tie(expr_opt, expr_type) = analyze_value_data(unary_op->expression, scope_env);

//          For each operator, check the expression type and perform the operation if optimizable.
            switch (unary_op->op) {
                case tokenKey::Not:
                case tokenKey::NotW:
//                  Ensure this operator takes a boolean.
                    if (expr_type != dataType::BoolT) {
                        throw TypeMismatchError(unary_op->op, true, expr_type, dataType::BoolT, true, unary_op->expression->line_number);
                    }

//                  If the expression could be evaluated, negate it.
                    if (expr_opt) {
                        boolContainer* const bool_expression = dynamic_cast<boolContainer*>(unary_op->expression.get());
//                      Update the value data object with the negated boolean.
                        value_data = make_shared<boolContainer>(unary_op->line_number, !bool_expression->boolean);

                        return make_pair(true, dataType::BoolT);
                    }

                    return make_pair(false, dataType::BoolT);
                    
                    
                default:
                    throw FatalError("unexpected unary operator", unary_op->line_number);
            }
        }

        case nodeType::BinaryOp: {
            dataType type1, type2;
            bool opt_expr1, opt_expr2;

//          Retrieve the binary operator object.
            binaryOp* const binary_op = dynamic_cast<binaryOp*>(value_data.get());

//          Analyze each expression in the binary operator. 
//          Retrieve the optimization status and type of each expression.
            tie(opt_expr1, opt_expr2, type1, type2) = _binaryop_analyze(binary_op, scope_env);

//          For each operator, check the expression types and perform the operation if optimizable.
            switch (binary_op->op) {
                case tokenKey::Plus:
                    return _generic_math_operation(opt_expr1, opt_expr1, type1, type2, numAdd(), 
                                                   addId(), addOverflow(), binary_op, value_data);

                case tokenKey::Minus:
                    return _generic_math_operation(opt_expr1, opt_expr1, type1, type2, numSubtract(), 
                                                   subtractId(), subtractOverflow(), binary_op, value_data);

                case tokenKey::Mult: 
                    return _generic_math_operation(opt_expr1, opt_expr1, type1, type2, numMult(), 
                                                   multId(), multOverflow(), binary_op, value_data);

                case tokenKey::Div: 
                    return _analyze_float_operation(opt_expr1, opt_expr1, type1, type2, _float_div, 
                                                    _div_id, _div_overflow, binary_op, value_data);

                case tokenKey::Exp:
                    return _analyze_float_operation(opt_expr1, opt_expr1, type1, type2, _exp, 
                                                    _exp_id, _exp_overflow, binary_op, value_data);

                case tokenKey::And:
                case tokenKey::AndW: 
                    return _analyze_bool_operation(opt_expr1, opt_expr2, type1, type2, logical_and<bool>{}, _and_identity, binary_op, value_data);
                
                case tokenKey::Or:
                case tokenKey::OrW: 
                    return _analyze_bool_operation(opt_expr1, opt_expr2, type1, type2, logical_or<bool>{}, _or_identity, binary_op, value_data);
                    
                case tokenKey::Xor:
                case tokenKey::XorW: 
    //              XOR has no identities, so the boolIdFunc parameter is a function that is always false.
    //              Also note that the not_equal_to functional for booleans functions identically to XOR.
                    return _analyze_bool_operation(opt_expr1, opt_expr2, type1, type2, not_equal_to<bool>{}, _no_bool_id, binary_op, value_data);

                case tokenKey::Greater:
                    return _analyze_comp_operation(opt_expr1, opt_expr2, type1, type2, tokenKey::Greater, greater<>{}, binary_op, value_data);

                case tokenKey::Less:
                    return _analyze_comp_operation(opt_expr1, opt_expr2, type1, type2, tokenKey::Less, less<>{}, binary_op, value_data);

//              Equals is unique in that it can take expressions of any type.
                case tokenKey::Equals:
                case tokenKey::Is: {
                    bool result;

    //              Ensure the expression types are combinable.
                    if (_uncombinable_types(type1, type2)) {
                        throw TypeMismatchError(binary_op->op, true, type1, type2, false, binary_op->expression1->line_number);
                    }

    //              Stop if either expression was not optimized.
                    if (!(opt_expr1 && opt_expr2)) {
                        return make_pair(false, dataType::BoolT);
                    }

    //              Ensure types are comparable. Store the comparison in result.
                    if (type1 == dataType::BoolT) {
                        const pair<bool, bool> bool_vals = _binaryop_booleans(opt_expr1, opt_expr2, binary_op);

                        result = bool_vals.first == bool_vals.second;
                    } else {
                        variant<pair<int64_t, int64_t>, pair<double, double>> nums;
    //                  The types must be numbers since we checked other types already.

    //                  Retrieve 64-bit implementations of numbers.
                        const bool floats = _binaryop_numbers(type1, type2, opt_expr1, opt_expr2, binary_op, nums);

    //                  Cast and compute the comparison value based on the type of the numbers.
                        if (floats) {
                            const pair<double, double> float_vals = get<pair<double, double>>(nums);
                            result = float_vals.first == float_vals.second;
                        } else {
                            const pair<double, double> int_vals = get<pair<int64_t, int64_t>>(nums);
                            result = int_vals.first == int_vals.second;
                        }
                    }

    //              Return that the binary operator was optimized and update value_data.
                    value_data = make_shared<boolContainer>(binary_op->expression1->line_number, result);
                    return make_pair(true, dataType::BoolT);
                }
                case tokenKey::GrEqual: 
                    return _analyze_comp_operation(opt_expr1, opt_expr2, type1, type2, tokenKey::GrEqual, greater_equal<>{}, binary_op, value_data);

                case tokenKey::LessEqual: 
                    return _analyze_comp_operation(opt_expr1, opt_expr2, type1, type2, tokenKey::LessEqual, less_equal<>{}, binary_op, value_data);
        
                default:
                    throw FatalError("binary operator not recognized", binary_op->line_number);
            }
        }

        case nodeType::TernaryOp: {
            dataType type1, type2, type3;
            bool expr1_opt, expr2_opt, expr3_opt;

//          Retrieve the ternary operator object.
            ternaryOp* const ternary_op = dynamic_cast<ternaryOp*>(value_data.get());

//          Analyze each ternary operator expression.
            tie(expr1_opt, type1) = analyze_value_data(ternary_op->expression1, scope_env);
            tie(expr2_opt, type2) = analyze_value_data(ternary_op->expression2, scope_env);
            tie(expr3_opt, type3) = analyze_value_data(ternary_op->expression3, scope_env);
    
//          For each operator, check the expression types and perform the operation if optimizable.
            switch (ternary_op->op) {
                case tokenKey::If:
//                  Ensure the condition is a boolean and the expressions have matching types.
                    if (type2 != dataType::BoolT) {
                        throw TypeMismatchError(ternary_op->op, false, type2, dataType::BoolT, true, ternary_op->expression2->line_number);
                    } else if (_uncombinable_types(type1, type3)) {
                        throw TypeMismatchError(ternary_op->op, false, type1, type3, false, ternary_op->expression1->line_number);
                    }
    
//                  If the condition could be optimized, replace the ternary if with whichever expression should be executed.
                    if (expr2_opt) {
                        const boolContainer* const condition = dynamic_cast<boolContainer*>(ternary_op->expression2.get());
    
                        if (condition->boolean) {
                            value_data = move(ternary_op->expression1);
                            return make_pair(expr1_opt, type1);
                        } else {
                            value_data = move(ternary_op->expression3);
                            return make_pair(expr3_opt, type3);
                        }
    
                    }
    
                    return make_pair(false, type1);
    
                default:
                    throw FatalError("ternary operator not recognized", ternary_op->line_number);
            }
        }

        case nodeType::VarContainer: {
            map<string, variableInfo>::iterator iter;

//          Retrieve the variable container object.
            varContainer* const var_container = dynamic_cast<varContainer*>(value_data.get());

//          Iterate through each local variable, then check the parent scope and repeat.
            environment* current_scope = scope_env.get();
            while ((iter = current_scope->locals.find(var_container->variable)) == current_scope->locals.end()) {
                if (current_scope->parent_scope == nullptr) {
//                  Throw an exception if the variable is not found anywhere.
                    throw VariableInitializationError(var_container->variable, true, var_container->line_number);
                }

//              Check the parent scope.
                current_scope = current_scope->parent_scope.get();
            }

//          Update value data to just be the variable's value.
            value_data = iter->second.value;
            return make_pair(iter->second.optimize_value, iter->second.type);
        }

//      Handle irreducible types.
        case nodeType::Int32Container:
            return make_pair(true, dataType::Int32T);
        case nodeType::Int64Container:
            return make_pair(true, dataType::Int64T);
        case nodeType::Float32Container:
            return make_pair(true, dataType::Float32T);
        case nodeType::Float64Container:
            return make_pair(true, dataType::Float64T);
        case nodeType::BoolContainer:
            return make_pair(true, dataType::BoolT);

//      Throw an exeption for an unrecognized (unimplemented) piece of value data.
        default:
            throw FatalError("value data not recognized during optimization", value_data->line_number);
    }
}
