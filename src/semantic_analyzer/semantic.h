/**
 * @project IFJ26 Compiler
 * @file semantic.h
 * @brief Semantic module interface for IFJ26 compiler.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_SEMANTIC_H
#define IFJ26_SEMANTIC_H

#include "../_utils/error.h"
#include "../parser/ast.h"
#include "ftable.h"
#include "scope.h"

/** @brief Semantic analysis context */
typedef struct {
	ScopeStack scope_stack;		 /**< Stack of scopes for variable declarations */
	FunctionTable func_table;	 /**< Table of functions for semantic analysis */
	ASTNode *current_function;	 /**< Pointer to the current function being analyzed */
	ValType current_return_type; /**< The expected return type of the current function, UNDEF if
									omitted */
} SemanticContext;

/**
 * @brief Main entry point for semantic analysis, initializes semantic context and starts analysis
 * @param outermost_node The top most node of the AST
 * @return Returns error, ERROR_OK if everything went well, smth else otherwise
 */
Error sem_run(ASTNode *outermost_node);

/**
 * @brief Initializes semantic context
 * @param ctx Pointer to the semantic context
 */
void sem_ctx_init(SemanticContext *ctx);

/**
 * @brief Releases semantic context
 * @param ctx Pointer to the semantic context
 */
void sem_ctx_free(SemanticContext *ctx);

/**
 * @brief Runs pass 1 and 2, calls analyze functions and analyzes the AST
 * @param ctx Semantic context
 * @param top_nodes The list of the top most nodes
 * @return Returns error depending on the state at which it ends in
 */
Error sem_analyze(SemanticContext *ctx, const ASTNodeList *top_nodes);

/**
 * @brief Analyze an expression and determine its type
 * @param ctx The semantic analysis context
 * @param expr The AST node representing the expression to analyze
 * @param out Pointer to store the resulting type of the expression
 * @return Error code indicating success or failure
 */
Error analyze_expression(SemanticContext *ctx, ASTNode *expr, ValType *out);

/**
 * @brief Analyze var declarations
 * @param ctx The semantic analysis context
 * @param node The node
 * @return Error code indicating success or failure
 */
Error analyze_var_decl(SemanticContext *ctx, ASTNode *node);

/**
 * @brief Analyze assignemnts
 * @param ctx The semantic analysis context
 * @param node The node
 * @return Error code incicating success or failure
 */
Error analyze_assignment(SemanticContext *ctx, ASTNode *node);

/**
 * @brief Analyze function calls
 * @param ctx The semantic analysis context
 * @param call The AST node representing the function call to analyze
 * @param out Pointer to store the resulting type of the function call
 * @return Error code indicating success or failure
 */
Error analyze_call(SemanticContext *ctx, ASTNode *call, ValType *out);

/**
 * @brief Analyze a block of code, optionally creating a new scope
 * @param ctx The semantic analysis context
 * @param block The AST node representing the block to analyze
 * @param new_scope Whether to create a new scope for this block
 * @param out Pointer to store the resulting type of the block (if applicable)
 * @param returns Says if it returns or not
 * @return Error code indicating success or failure
 */
Error analyze_block(
	SemanticContext *ctx, ASTNode *block, bool new_scope, ValType *out, bool *returns);
/**
 * @brief Anlyzes an if statement or if epxression
 * @param ctx The semantic analysis context
 * @param node The if node
 * @param out Type of the result
 * @param returns Says if every branch returns
 * @return ERROR code indicating success or failure
 */
Error analyze_if(SemanticContext *ctx, ASTNode *node, ValType *out, bool *returns);

/**
 * @brief Analyze a statement
 * @param ctx The semantic analysis context
 * @param stmt The AST node representing the statement to analyze
 * @param out Pointer to store the resulting type of the statement (if applicable)
 * @param returns Says if it returns or not
 * @return Error code indicating success or failure
 */
Error analyze_stmt(SemanticContext *ctx, ASTNode *stmt, ValType *out, bool *returns);

/**
 * @brief Analyze a function definition
 * @param ctx The semantic analysis context
 * @param entry The FunctionEntry representing the function definition to analyze
 * @return Error code indicating success or failure
 */
Error analyze_function(SemanticContext *ctx, FunctionEntry *entry);

#endif /* IFJ26_SEMANTIC_H */
