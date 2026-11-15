/**
 * @project IFJ26 Compiler
 * @file code_gen.h
 * @brief Code gen module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_CODE_GEN_H
#define IFJ26_CODE_GEN_H

#include "../parser/ast.h"
#include <stdbool.h>

/**
 * @brief Gets operand type and it's result type and prints the corresponding instruction to std
 * output
 * @param op The operand type
 * @param result_type The resulting type
 * @return Returns false if the operand type is not supported or when division the result_type is
 * neither INT or DOUBLE, true otherwise
 */
bool gen_binary_op_stack(OpType op, ValType result_type);

/**
 * @brief Gets the root node of an expression and recursivelly prints instructions onto std output
 * @param exp The root node of the expression
 * @return Returns false if it failed, true otherwise
 */
bool gen_expr(const ASTNode *exp);

/**
 * @brief Analyze a statement
 * @param stmt The node
 * @return Returns false if it failed or the stmt is of type that's not supported, true otherwise
 */
bool gen_stmt(const ASTNode *stmt);

#endif /* IFJ26_CODE_GEN_H */
