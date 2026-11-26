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

#include "../_utils/error.h"
#include "../parser/ast.h"
#include <stdbool.h>

/**
 * @brief The main function to be called to run gencode
 * @param outermost_node the topmost node
 * @return Returns ERROR_OK if no error occured, other errors otherwise
 */
Error gen_run(const ASTNode *outermost_node);

/**
 * @brief Takes a function node and prints it using instructions onto std output
 * @param func The func node
 * @return Returns Error code according to how it passed or failed
 */
Error gen_function(ASTNode *func);

/**
 * @brief Generates and iterates through if statements
 * @param node The if node
 * @param as_val Set to true if the if is in an expression
 * @return Error code according to how it passed or failed
 */
Error gen_if(const ASTNode *node, const bool as_val);

/**
 * @brief Gets operand type and it's result type and prints the corresponding instruction to std
 * output
 * @param exp The operand node
 * @return Returns Error code according to how it passed or failed
 */
Error gen_binary_op_stack(const ASTNode *exp);

/**
 * @brief Gets the root node of an expression and recursivelly prints instructions onto std output
 * @param exp The root node of the expression
 * @return Returns Error code according to how it passed or failed
 */
Error gen_expr(const ASTNode *exp);

/**
 * @brief Analyze a statement
 * @param stmt The node
 * @return Returns Error code according to how it passed or failed
 */
Error gen_stmt(const ASTNode *stmt);

#endif /* IFJ26_CODE_GEN_H */
