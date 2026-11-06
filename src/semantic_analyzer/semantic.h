/**
 * @project IFJ26 Compiler
 * @file semantic.h
 * @brief Semantic module interface for IFJ26 compiler.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_SEMANTIC_H
#define IFJ26_SEMANTIC_H

#include "../parser/ast.h"
#include "ftable.h"
#include "scope.h"

/** @brief Semantic analysis context */
typedef struct {
	ScopeStack scope_stack;	  /**< Stack of scopes for variable declarations */
	FunctionTable func_table; /**< Table of functions for semantic analysis */
} SemanticContext;

/**
 * @brief Analyze an expression and determine its type
 * @param ctx The semantic analysis context
 * @param expr The AST node representing the expression to analyze
 * @param out Pointer to store the resulting type of the expression
 * @return Error code indicating success or failure
 */
Error analyze_expression(SemanticContext *ctx, ASTNode *expr, ValType *out);

#endif /* IFJ26_SEMANTIC_H */
