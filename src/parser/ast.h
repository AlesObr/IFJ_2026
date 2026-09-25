/**
 * @project IFJ26 Compiler
 * @file ast.h
 * @brief Abstract syntax tree data structures and interface using dstring.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 * @cite https://keleshev.com/abstract-syntax-tree-an-example-in-c/ by Vladimir Keleshev 20. 6. 2022
 * available online
 *
 * @par Ownership contract
 * Every constructor that receives child ASTNode pointers takes ownership of
 * them unconditionally. If the constructor fails (allocation error or invalid
 * argument), it destroys the children it was given and returns NULL. The
 * caller therefore never has to clean up arguments it already handed over --
 * it only destroys the nodes it still holds itself.
 *
 * The same rule applies to ast_call_add_arg() and ast_block_add_stmt():
 * on failure the appended node is destroyed and false is returned.
 */

#ifndef IFJ26_AST_H
#define IFJ26_AST_H

#include "../_utils/dstring.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Enumeration of AST node types.
 */
typedef enum {
	/* Literals */
	NODE_INT_LITERAL,
	NODE_FLOAT_LITERAL,
	NODE_STRING_LITERAL,
	NODE_NULL_LITERAL,

	/* Identifiers and Variables */
	NODE_IDENTIFIER,
	NODE_VAR_DECL,
	NODE_ASSIGNMENT,

	/* Expressions and Operations */
	NODE_BINARY_OP,
	NODE_UNARY_OP,

	/* Function Calls */
	NODE_CALL,

	/* Control Flow and Statements */
	NODE_BLOCK,
	NODE_IF,
	NODE_WHILE,
	NODE_RETURN
} ASTNodeType;

/**
 * @brief Enumeration of binary and unary operation types.
 */
typedef enum {
	OP_ADD,			// +
	OP_SUB,			// -
	OP_MUL,			// *
	OP_DIV,			// /
	OP_CONCAT,		// ~
	OP_ASSIGN,		// =
	OP_EQ,			// ==
	OP_NEQ,			// !=
	OP_LT,			// <
	OP_LTE,			// <=
	OP_GT,			// >
	OP_GTE,			// >=
	OP_NOT,			// ! (unary)
	OP_UNARY_MINUS, // - (unary)
	OP_UNAARY_PLUS	// + (unary)
} OpType;

typedef struct ASTNode ASTNode;

/**
 * @brief Dynamic list of AST node pointers for statement blocks and call arguments.
 */
typedef struct ASTNodeList {
	ASTNode **items; /**< Dynamic array of ASTNode pointers. */
	size_t count;	 /**< Number of stored items. */
	size_t capacity; /**< Allocated capacity of the array. */
} ASTNodeList;

/**
 * @brief Structure representing an AST node.
 */
struct ASTNode {
	ASTNodeType type;
	size_t line; /**< Source code line where the construct starts. */
	size_t col;	 /**< Source code column where the construct starts. */

	union {
		long int_val;	   /**< Literal integer value. */
		double float_val;  /**< Literal float value. */
		dstring_t str_val; /**< Dynamic string for literal strings and identifiers. */

		/** @brief Structure for binary and unary operations. */
		struct {
			OpType op;
			ASTNode *left;
			ASTNode *right; /**< Set to NULL for unary operations. */
		} op;

		/** @brief Structure for variable declaration nodes. */
		struct {
			dstring_t var_name;	 /**< Dynamic string holding declared variable name. */
			dstring_t type_name; /**< Dynamic string holding explicit type name. */
			bool is_const;		 /**< True if declared as const, false for var. */
			ASTNode *init_expr;	 /**< Initializer expression or NULL. */
		} var_decl;

		/** @brief Structure for variable assignment nodes. */
		struct {
			dstring_t target_name; /**< Target variable identifier name. */
			ASTNode *expr;		   /**< Assigned expression. */
		} assignment;

		/** @brief Structure for function call nodes. */
		struct {
			dstring_t func_name; /**< Identifier of the called function. */
			ASTNodeList args;	 /**< List of argument expressions. */
		} call;

		/** @brief Structure for composite statement blocks. */
		struct {
			ASTNodeList stmts; /**< Sequential list of statement nodes. */
		} block;

		/** @brief Structure for if-else branching nodes. */
		struct {
			ASTNode *cond;		  /**< Branching condition expression. */
			ASTNode *then_branch; /**< Statements executed on condition true. */
			ASTNode *else_branch; /**< Statements executed on condition false or NULL. */
		} if_stmt;

		/** @brief Structure for while loop nodes. */
		struct {
			ASTNode *cond; /**< Loop condition expression. */
			ASTNode *body; /**< Loop body statement or block. */
		} while_stmt;

		/** @brief Structure for return statement nodes. */
		struct {
			ASTNode *expr; /**< Returned expression or NULL for empty return. */
		} return_stmt;
	};
};

/*===================================================*/

/**
 * @brief Creates a new AST node for an integer literal.
 * @param val Integer constant value.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_int(long val, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a floating point literal.
 * @param val Double precision float constant value.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_float(double val, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a string literal.
 * @param val Pointer to source string (content is cloned via dstring_copy).
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_string(const dstring_t *val, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a null literal.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_null(size_t line, size_t col);

/**
 * @brief Creates a new AST node for an identifier using dstring_t.
 * @param name Pointer to identifier name (content is cloned via dstring_copy).
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_ident(const dstring_t *name, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a binary operation.
 * @param op Type of binary operator.
 * @param left Left-hand side operand node.
 * @param right Right-hand side operand node.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_binop(OpType op, ASTNode *left, ASTNode *right, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a unary operation.
 * @param op Type of unary operator.
 * @param expr Operand expression node.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_unop(OpType op, ASTNode *expr, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a variable or constant declaration.
 * @param name Variable identifier name (content is cloned).
 * @param type_name Explicit type identifier name or NULL (content is cloned).
 * @param is_const Flag specifying whether the symbol is immutable.
 * @param init_expr Initializer expression node or NULL.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_var_decl(const dstring_t *name, const dstring_t *type_name, bool is_const,
	ASTNode *init_expr, size_t line, size_t col);

/**
 * @brief Creates a new AST node for an assignment operation.
 * @param target Identifier name of target variable (content is cloned).
 * @param expr Evaluated expression node to store.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_assignment(const dstring_t *target, ASTNode *expr, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a function invocation.
 * @param func_name Name of function to invoke (content is cloned).
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_call(const dstring_t *func_name, size_t line, size_t col);

/**
 * @brief Appends an argument expression node to a function call ASTNode.
 * @param call_node Function call node created via ast_create_call.
 * @param arg Evaluated argument expression node.
 */
bool ast_call_add_arg(ASTNode *call_node, ASTNode *arg);

/**
 * @brief Creates a new AST node for an encapsulated statement block.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_block(size_t line, size_t col);

/**
 * @brief Appends an executable statement node to a block node.
 * @param block_node Block node created via ast_create_block.
 * @param stmt Executable statement ASTNode to append.
 */
bool ast_block_add_stmt(ASTNode *block_node, ASTNode *stmt);

/**
 * @brief Creates a new AST node for an if-else branching construct.
 * @param cond Boolean condition expression node.
 * @param then_b Statements executed when condition holds true.
 * @param else_b Statements executed when condition evaluates false or NULL.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_if(ASTNode *cond, ASTNode *then_b, ASTNode *else_b, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a while iteration loop.
 * @param cond Boolean loop condition expression node.
 * @param body Executed statement or block for each iteration.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_while(ASTNode *cond, ASTNode *body, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a return statement.
 * @param expr Expression to return from current function or NULL for void return.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_return(ASTNode *expr, size_t line, size_t col);

/**
 * @brief Dumps the AST in S-expression format to the specified output stream.
 * @param node Root or branch ASTNode pointer to dump.
 * @param out Output stream for serialized text (e.g. stdout).
 */
void ast_dump_sexpr(const ASTNode *node, FILE *out);

/**
 * @brief Recursively destroys the AST and frees all allocated memory (including dstrings).
 * @param node Root ASTNode pointer to recursively clean up.
 */
void ast_destroy(ASTNode *node);

#endif /* IFJ26_AST_H */
