/**
 * @project IFJ26 Compiler
 * @file ast.h
 * @brief Abstract syntax tree data structures and interface using dstring.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_AST_H
#define IFJ26_AST_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "../_utils/dstring.h"

/**
 * @brief Enumeration of AST node types.
 */
typedef enum {
	NODE_INT_LITERAL,
	NODE_IDENTIFIER,
	NODE_BINARY_OP,
	NODE_ASSIGNMENT,
	NODE_VAR_DECL
} ASTNodeType;

/**
 * @brief Enumeration of binary operation types.
 */
typedef enum {
	OP_ADD,      // +
	OP_SUB,      // -
	OP_MUL,      // *
	OP_DIV,      // /
	OP_ASSIGN,   // =
	OP_EQ,       // ==
	OP_NEQ       // !=
} BinaryOpType;

/**
 * @brief Structure representing an AST node.
 */
typedef struct ASTNode {
	ASTNodeType type;
	size_t line; /**< Source code line where the construct starts. */
	size_t col;  /**< Source code column where the construct starts. */

	union {
		long int_val;
		dstring_t ident_name; /**< Dynamic string holding identifier name. */

		struct {
			BinaryOpType op;
			struct ASTNode *left;
			struct ASTNode *right;
		} binary_op;

		/** @brief Structure for variable declaration nodes. */
		struct {
			dstring_t var_name;       /**< Dynamic string holding declared variable name. */
			struct ASTNode *init_expr; /**< Initializer expression or NULL. */
		} var_decl;
	};
} ASTNode;

/*===================================================*/

/**
 * @brief Creates a new AST node for an integer literal.
 */
ASTNode *ast_create_int(long val, size_t line, size_t col);

/**
 * @brief Creates a new AST node for an identifier using dstring_t.
 * @param name Pointer to source dstring (content is cloned via dstring_copy).
 */
ASTNode *ast_create_ident(const dstring_t *name, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a binary operation.
 */
ASTNode *ast_create_binop(BinaryOpType op, ASTNode *left, ASTNode *right, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a variable declaration.
 * @param name Pointer to variable name dstring (content is cloned via dstring_copy).
 */
ASTNode *ast_create_var_decl(const dstring_t *name, ASTNode *init_expr, size_t line, size_t col);

/**
 * @brief Dumps the AST in S-expression format to the specified output stream.
 */
void ast_dump_sexpr(const ASTNode *node, FILE *out);

/**
 * @brief Recursively destroys the AST and frees all allocated memory (including dstrings).
 */
void ast_destroy(ASTNode *node);

#endif /* IFJ26_AST_H */
