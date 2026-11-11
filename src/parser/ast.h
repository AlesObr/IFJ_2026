/**
 * @project IFJ26 Compiler
 * @file ast.h
 * @brief Abstract syntax tree data structures and interface using dstring.
 * @author Aleš Obr (xobrale00)
 * @author Štěpán Hořenek (xhorens00)
 * @note Standard: ISO C11
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

	/* Data types*/
	NODE_TYPE_ORIG,
	NODE_TYPE_USER,
	NODE_UNIF,

	/* Expressions and Operations */
	NODE_BINARY_OP,
	NODE_UNARY_OP,

	/* Function Calls */
	NODE_CALL,

	/* Control Flow and Statements */
	NODE_FUNC_PARAM,
	NODE_FUNC_WINDOW,
	NODE_FUNC_DECL,
	NODE_EXT_DECL,
	NODE_CLASS_DECL,
	NODE_BLOCK,
	NODE_IF,
	NODE_ELSE,
	NODE_WHILE,
	NODE_FOR,
	NODE_MATCH,
	NODE_CASE,
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
	OP_UNARY_PLUS,	// + (unary)
	OP_OR,			// ||
	OP_AND			// &&
} OpType;

/**
 * @brief Enumeration for function and parameter types
 */
typedef enum {
	UNDEF,
	INT,
	DOUBLE,
	UNIT,
	STRING,
	BOOLEAN,
	ANY,
} ValType;

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
	size_t line; 		/**< Source code line where the construct starts. */
	size_t col;	 		/**< Source code column where the construct starts. */

	ASTNodeList expr_types;	/**< Determines which type of operation to use (DIV / IDIV) for example*/
	bool to_double;				/**< Determines if conversion to int / double needed*/
	size_t sym_id;				/**< Identifier of the variable in symtable if needed */

	union {
		long int_val;	   /**< Literal integer value. */
		double float_val;  /**< Literal float value. */
		dstring_t str_val; /**< Dynamic string for literal strings and identifiers. */

		/** @brief Structure for data types */
		struct{
			dstring_t defined_type;	/**< Type unification defined by user */
			ValType orig_type;		/**< Languages predefined type */
		} data_type;

		/** @brief Structure for type unification */
		struct{
			dstring_t unif_name;	/**< Name of type unification*/
			ASTNodeList types;		/**< List of types in type unification */
		} type_unif;

		/** @brief Structure for data class */
		struct {
			dstring_t class_name;		/**< Name of the data_class */
			ASTNode *param_list;		/**< List of data_class parameters */
		} data_class;

		/** @brief Structure for matching cases */
		struct {
			ASTNode *switch_expr; 	/**< Expression by which match is switching */
			ASTNodeList cases;		/**< Cases of the match */
		} match;

		/** @brief Structure for switching cases of match */
		struct {
			ASTNode *case_member;	/**< Expression based on which is match switching */
			ASTNode *guard;			/**< Unmandatory condition after switching expression */
			ASTNode *body_expr;		/**< Expression to be executed in case pattern is matched */
		} case_node;

		/** @brief Structure for binary and unary operations. */
		struct {
			OpType op;
			ASTNode *left;
			ASTNode *right; /**< Set to NULL for unary operations. */
		} op;

		/** @brief Structure for variable declaration nodes. */
		struct {
			dstring_t var_name; 	/**< Dynamic string holding declared variable name. */
			ASTNodeList type_name;	/**< Types of the variable */
			bool is_const;			/**< True if declared as const, false for var. */
			ASTNode *init_expr; 	/**< Initializer expression or NULL. */
		} var_decl;

		/** @brief Structure for function call nodes. */
		struct {
			dstring_t func_name; /**< Identifier of the called function. */
			ASTNode *target;	 /**< Target of the function call (used for chain calling) */
			ASTNodeList args;	 /**< List of argument expressions. */
		} call;

		/** @brief Structure for function parameters */
		struct {
			dstring_t param_name; 		/**< Name of the function parameter. */
			ASTNodeList param_type;	/**< Type of the function parameter. */
		} func_param;

		/** @brief Structure for function param window */
		struct {
			ASTNodeList params; /**< List of parameters for said function parameter window */
		} param_window;

		/** @brief Structure for function declaration */
		struct {
			dstring_t func_name;	   		/**< Identifier of declared function */
			ASTNodeList return_type;	    /**< Types that function should return */
			bool is_main;			   		/**< Flag tracking whether function is the main one */
			ASTNodeList param_windows; 		/**< List of parameter windows */
			ASTNode *body;			   		/**< Expressions inside function body */
		} func_decl;

		/** @brief Structure for extension declaration */
		struct {
			dstring_t param_name; 		/**< Dynamic string holding extension parameter  */
			ASTNodeList type_name;	  	/**< Type that extension extension */
			ASTNodeList funcs;	  		/**< List of functions inside said extension */
		} ext_decl;

		/** @brief Structure for composite statement blocks. */
		struct {
			ASTNodeList stmts; /**< Sequential list of statement nodes. */
		} block;

		/** @brief Structure for if-else branching nodes. */
		struct {
			ASTNode *cond;			/**< Branching condition expression. */
			ASTNode *body;			/**< Code block indented by if statement */
			ASTNodeList next_conds; /**< Optional else / else if branches */
		} if_else_stmt;

		/** @brief Structure for while loop nodes. */
		struct {
			ASTNode *cond; /**< Loop condition expression. */
			ASTNode *body; /**< Loop body statement or block. */
		} while_stmt;

		/** @brief Structure of for loop nodes. */
		struct {
			dstring_t gen_id;	/**< Id of generator */
			ASTNode *gen_start; /**< Expression serving as start of generator */
			ASTNode *gen_end;	/**< Expression serving as end of generator */
			ASTNode *gen_step;	/**< Expression serving as step of generator */
			bool last_in;		/**< Flag symbolizing whether gen_end is included in interval */
			ASTNodeList guards; /**< Guards of the for loop */
			ASTNode *body;		/**< Body of for loop */
		} for_stmt;

		/** @brief Structure for return statement nodes. */
		struct {
			ASTNode *expr; /**< Returned expression or NULL for empty return. */
		} return_stmt;
	};
};

/*===================================================*/

/**
 * @brief Creates a new AST node for match
 * @param match_expr Expression to match with cases
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_match_node(ASTNode *match_expr, size_t line, size_t col);

/**
 * @brief Creates a new AST node for match case
 * @param case_expr Expression that can be matched
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_case_node(ASTNode *case_expr, size_t line, size_t col);

/**
 * @brief Appends case node into match node
 * @param match_node Match node to append case into
 * @param case_node Case node to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_match_add_case(ASTNode *match_node, ASTNode *case_node);

/**
 * @brief Appends nonmandatory guard to case ASTNode created by ast_create_case_node
 * @param case_node Node of case the guard belongs to
 * @param guard Condition under wich the expreession in case can be matched
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_case_add_guard(ASTNode *case_node, ASTNode *guard);

/**
 * @brief Appends expression to case ASTNode created by ast_create_case_node
 * @param case_node Node of case the expression belongs to
 * @param body Expression that executes if the case is matched
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_case_add_body(ASTNode *case_node, ASTNode *body);

/**
 * @brief Creates a new AST node for for data type unification
 * @param unif_name Name of the newly created unification
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_type_unif(const dstring_t *unif_name, size_t line, size_t col);

/**
 * @brief Creates a new AST node for for data class
 * @param unif_name Name of the newly created data class
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_data_class(const dstring_t *class_name, size_t line, size_t col);

/**
 * @brief Inserts parameter list into data class ASTNode
 * @param data_class Data class node
 * @param param_window Node containing windows of parameters
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_class_add_param(ASTNode *data_class, ASTNode *param_window);

/**
 * @brief Creates a new AST node for for loop statement
 * @param gen_id Name of the for loop generator
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_for(const dstring_t *gen_id, size_t line, size_t col);

/**
 * @brief Function for appending generator start expression to for loop AST node
 * @param for_node Node of for loop to be appended into
 * @param expr Expression to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_for_add_start(ASTNode *for_node, ASTNode *expr);

/**
 * @brief Function for appending generator end expression and setting flag last_in to for loop AST
 * node
 * @param for_node Node of for loop to be appended into
 * @param expr Expression to be appended
 * @param last_in Flag checking whether the last number is also included in interval
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_for_add_end(ASTNode *for_node, ASTNode *expr, bool last_in);

/**
 * @brief Function for appending generator step expression to for loop AST node
 * @param for_node Node of for loop to be appended into
 * @param expr Expression to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_for_add_step(ASTNode *for_node, ASTNode *expr);

/**
 * @brief Function for appending code block to for loop AST node
 * @param for_node Node of for loop to be appended into
 * @param code_block Code block to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_for_add_body(ASTNode *for_node, ASTNode *code_block);

/**
 * @brief Function for appending guards to for loop AST node
 * @param for_node Node of for loop to be appended into
 * @param guard For loop guard to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_for_append_guard(ASTNode *for_node, ASTNode *guard);

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
 * @param is_const Flag specifying whether the symbol is immutable.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_var_decl(const dstring_t *name, bool is_const, size_t line, size_t col);

/**
 * @brief Appends initiallizer epxression to variable declaration AST node
 * @param var_decl_node Var declaration node to append initializer expression into
 * @param init_expr Initializer expression to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_var_decl_add_init(ASTNode *var_decl_node, ASTNode *init_expr);

/**
 * @brief Creates a new AST node for a function invocation.
 * @param func_name Name of function to invoke (content is cloned).
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_call(const dstring_t *func_name, size_t line, size_t col);

/**
 * @brief Appends parameter window into function call node
 * @param func_call Function call node
 * @param param_window Parameter window to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_call_add_window(ASTNode *func_call, ASTNode *param_window);

/**
 * @brief Sets node that the function call node extend
 * @param call_node Function call node with target to be set
 * @param target Target node to be appended to call node
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_call_add_target(ASTNode *call_node, ASTNode *target);

/**
 * @brief Appends an argument expression node to a function call ASTNode.
 * @param call_node Function call node created via ast_create_call.
 * @param arg Evaluated argument expression node.
 * @return True if success, false if problem occurs or wrong parameters passed
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
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_block_add_stmt(ASTNode *block_node, ASTNode *stmt);

/**
 * @brief Creates a new AST node for an if branching construct.
 * @param cond Boolean condition expression node.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_if(ASTNode *cond, size_t line, size_t col);

/**
 * @brief Creates a new AST node for an else branching construct.
 * @param cond Boolean condition expression node.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_else(ASTNode *cond, size_t line, size_t col);

/**
 * @brief Appends code block to if / else AST node
 * @param if_else_node If-else statement to node to append code block into
 * @param expr_block Code block or expression (viz Control) to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_if_else_add_body(ASTNode *if_else_node, ASTNode *expr_block);

/**
 * @brief Appends code block to while AST node
 * @param while_node While statement to node to append code block into
 * @param code_block Code block to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_while_add_body(ASTNode *while_node, ASTNode *code_block);

/**
 * @brief Appends else branch to if statement AST node
 * @param if_else_node If-else statement to node to append new branch into
 * @param next_condition If-else statement to node to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_if_else_append_branch(ASTNode *if_else_node, ASTNode *next_condition);

/**
 * @brief Creates a new AST node for a while iteration loop.
 * @param cond Boolean loop condition expression node.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_while(ASTNode *cond, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a return statement.
 * @param expr Expression to return from current function or NULL for void return.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_return(ASTNode *expr, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a function parameter.
 * @param param_name Identifier of funtion param
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_func_param(const dstring_t *param_name, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a function declaration.
 * @param func_name Function declaration node.
 * @param is_main Flag whether function is main one
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_func(const dstring_t *func_name, bool is_main, size_t line, size_t col);

/**
 * @brief Creates a new AST node for a function parameter window.
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_create_param_window(size_t line, size_t col);

/**
 * @brief Appends an parameter window to function declaration AST_NODE
 * @param func_node Function declaration node created via ast_create_func_decl
 * @param param_window Parameter window AST Node to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_func_add_param_window(ASTNode *func_node, ASTNode *param_window);

/**
 * @brief Appends an parameter to parameter window AST_NODE
 * @param param_window Function parameter window node created via ast_create_func_decl
 * @param param Parameter AST Node to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_window_add_param(ASTNode *param_window, ASTNode *param);

/**
 * @brief Adds function body into function AST node
 * @param func_node Function node created via ast_create_func_decl
 * @param code_block Code_block AST Node to be assigned to function
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_func_add_block(ASTNode *func_node, ASTNode *code_block);

/**
 * @brief Creates a new AST node for a extension declaration.
 * @param param_name Name of type the extension extends
 * @param line Source line index.
 * @param col Source column index.
 * @return Allocated ASTNode pointer or NULL on failure.
 */
ASTNode *ast_ext_create(const dstring_t *param_name, size_t line, size_t col);

/**
 * @brief Addes a type AST node if node is valid (compatible only with some nodes)
 * @param node ASTNode to add type into
 * @param type_name_orig Name of original type to be added
 * @param type_name_def Name of defined type to be added
 * @param is_orig Determines if data type is original or user defined
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_add_type(ASTNode *node, ValType type_name_orig, const dstring_t *type_name_def, bool is_orig);

/**
 * @brief Appends a function to extension AST_NODE
 * @param ext_node Extension node created via ast_create_func_decl
 * @param func_node Function declaration AST Node to be appended
 * @return True if success, false if problem occurs or wrong parameters passed
 */
bool ast_ext_add_func(ASTNode *ext_node, ASTNode *func_node);

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
