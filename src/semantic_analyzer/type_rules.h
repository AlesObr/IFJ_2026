/**
 * @project IFJ26 Compiler
 * @file type_rules.h
 * @brief Type checking rules for semantic analysis.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_TYPE_RULES_H
#define IFJ26_TYPE_RULES_H

#include "../parser/ast.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Checks if a value of type `actual` can be assigned to a variable of type `expected`.
 * @param expected The type of the variable to assign to
 * @param actual The type of the value to assign
 * @param convert Will be set to true if an Int -> Double conversion is needed.
 * @return true if the assignment is valid, false otherwise
 */
bool type_compatible(ValType expected, ValType actual, bool *convert);

/**
 * @brief Checks the result type of a binary operation given the operator and operand types.
 * @param op The binary operator
 * @param l The type of the left operand
 * @param r The type of the right operand
 * @param conv_l Will be set to true if the left operand needs an Int -> Double conversion.
 * @param conv_r Will be set to true if the right operand needs an Int -> Double
 * @return Result type of the binary operation
 */
ValType binop_result(OpType op, ValType l, ValType r, bool *conv_l, bool *conv_r);

/**
 * @brief Checks the result type of a unary operation given the operator and operand types.
 * @param op The unary operator
 * @param operand The type of the operand
 * @return Result type of the unary operation, or UNDEF if illegal
 */
ValType unop_result(OpType op, ValType operand);

#endif /* IFJ26_TYPE_RULES_H */
