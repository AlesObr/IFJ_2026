/**
 * @project IFJ26 Compiler
 * @file fregister.h
 * @brief Registers built-in and user-defined functions into the function table.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_FREGISTER_H
#define IFJ26_FREGISTER_H

#include "../_utils/error.h"
#include "../parser/ast.h"
#include "ftable.h"

/**
 * @brief Registers all builtin functions in the function table
 * @param t The function table to register functions in
 * @returns false on allocation failure or duplicate entry
 */
bool register_builtin_function(FunctionTable *t);

/** @brief Registers a user-defined function in the function table
 * @param t The function table to register the function in
 * @param func_node The AST node representing the function
 * @returns Error code
 */
Error register_user_function(FunctionTable *t, ASTNode *func_node);

/** @brief Loop over top level nodes and register functions
 * @param t The function table to register functions in
 * @param top_level_nodes The list of top level nodes
 * @returns Error code
 */
Error register_functions(FunctionTable *t, const ASTNodeList *top_level_nodes);

#endif /* IFJ26_FREGISTER_H */
