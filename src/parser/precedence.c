/**
 * @project IFJ26 Compiler
 * @file precedence.c
 * @brief Precedence expression parsing interface for parser
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "precedence.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include "../_utils/stack.h"
#include "../lexer/lexer.h"
#include "ast.h"
#include "parser.h"
#include <stdbool.h>
#include <stdio.h>

#define PREC_TABLE_SIZE 19 /**< Size of both dimensions of precedence table */

/**
 * @brief Precedence table for evaluating expressions
*/
static const char precedence_table = {
	//         u-   !    *    /    +    -    <    >    <=   >=   ==   !=   &&   ||   =    (    )    i    $
	/* u- */ {'<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* !  */ {'<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* *  */ {'<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* /  */ {'<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* +  */ {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* -  */ {'<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* <  */ {'<', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* >  */ {'<', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* <= */ {'<', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* >= */ {'<', '<', '<', '<', '<', '<', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', '<', '>', '<', '>'},
	/* == */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', ' ', '>', '>', '>', '<', '>', '<', '>'},
	/* != */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', ' ', '>', '>', '>', '<', '>', '<', '>'},
	/* && */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '>', '<', '>', '<', '>'},
	/* || */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '<', '>'},
	/* =  */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '>', '<', '>'},
	/* (  */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' '},
	/* )  */ {' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'},
	/* i  */ {' ', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>'},
	/* $  */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', ' '}
};

/**
 * @brief Translation table from operator to index of precedence table
*/
typedef enum {
	IDX_UMINUS = 0, // u-
	IDX_NOT,        // !
	IDX_MUL,        // *
	IDX_DIV,        // /
	IDX_ADD,        // +
	IDX_SUB,        // -
	IDX_LT,         // <
	IDX_GT,         // >
	IDX_LTE,        // <=
	IDX_GTE,        // >=
	IDX_EQ,         // ==
	IDX_NEQ,        // !=
	IDX_AND,        // &&
	IDX_OR,         // ||
	IDX_ASSIGN,     // =
	IDX_LPAREN,     // (
	IDX_RPAREN,     // )
	IDX_ID,         // i (identifier, literal, call, unit '()')
	IDX_DOLLAR,     // $ (end of expression)
} PrecIndex;

/**
 * @brief Structure for items of stack durring parsing exprs
*/
typedef struct {
	PrecIndex term_idx;	/**< Index of terminal */
	ASTNode *node;		/**< ASTNode in case node contains some literal or function call */
	bool is_term;		/**< false -> is not terminal, has ASTnode, true -> is terminal, is operation */
	bool has_handle;	/**< true if '<' handle marker placed right after this */
	OpType op;			/**< Operator type in case item is operator */
	size_t line;		/**< Line of the object */
	size_t col;			/**< Collumn of the object */
} PrecStackItem;

static ParserStatus parse_stack(Token *curr_token);

#define GET_TOKEN(tok_ptr, node, eof_poss)                                                         \
	do {                                                                                           \
		token_destroy(tok_ptr);                                                                    \
		Error err = lexer_get_next_token((tok_ptr));                                               \
		if (ERROR_IS_ERR((err))) {                                                                 \
			if (node != NULL) {                                                                    \
				ast_destroy(node);                                                                 \
			}                                                                                      \
			return PARSE_ERROR((err), LEX_FAIL);                                                   \
		}                                                                                          \
		precedence_trace_token(__func__, (tok_ptr));                                               \
		if ((tok_ptr)->type == TOKEN_EOF) {                                                        \
			if ((eof_poss)) {                                                                      \
				return PARSE_OK(PAR_EOF, node);                                                    \
			} else {                                                                               \
				if (node != NULL) {                                                                \
					ast_destroy(node);                                                             \
				}                                                                                  \
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,       \
									   "EOF received at invalid place\n"),                         \
					PAR_EOF);                                                                      \
			}                                                                                      \
		}                                                                                          \
	} while (0)

#define ALLOC_NODE(node, alloc_expr)                                                               \
	do {                                                                                           \
		(node) = (alloc_expr);                                                                     \
		if ((node) == NULL) {                                                                      \
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,         \
								   "Failed to allocate node for AST tree\n"),                      \
				AST_FAIL);                                                                         \
		}                                                                                          \
	} while (0)

#define CHECK_AST_APPEND(node, append_expr)                                                        \
	do {                                                                                           \
		if (!(append_expr)) {                                                                      \
			ast_destroy(node);                                                                     \
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,         \
								   "Failed to append node to AST tree\n"),                         \
				AST_FAIL);                                                                         \
		}                                                                                          \
	} while (0)

#define APPEND_BEFORE_EOF_RETURN(status, node, append_expr)                                        \
	do {                                                                                           \
		if ((status).flag == PAR_EOF) {                                                            \
			if ((status).new_node != NULL) {                                                       \
				CHECK_AST_APPEND(node, append_expr);                                               \
				return PARSE_OK(PAR_EOF, node);                                                    \
			}                                                                                      \
		}                                                                                          \
	} while (0)

#define CHECK_STATUS(status)                                                                       \
	do {                                                                                           \
		if ((status).flag != PAR_SUCC) {                                                           \
			return (status);                                                                       \
		}                                                                                          \
	} while (0)

/**
 * @brief Macro for checking status flags, releasing the node being built on failure
 * @param status Status structure to be checked
 * @param owner Partially built node owned by the caller (not attached anywhere yet)
 */
#define CHECK_STATUS_OWNED(status, owner)                                                          \
	do {                                                                                           \
		if ((status).flag != PAR_SUCC) {                                                           \
			ast_destroy(owner);                                                                    \
			return (status);                                                                       \
		}                                                                                          \
	} while (0)

/**
 * @brief Macro for checking precedence stack allocating operation
 * @param item Allocated PrecStackItem structure to be checked
 * @param stack Stack to be destroyed in case of failure
*/
#define CHECK_STACK_ITEM(item, stack)															   \
	do {																						   \
		if ((item) == NULL){																	   \
			destroy_prec_stack(stack);															   \
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,         \
								   "Failed to allocate node for AST tree\n"),                      \
				AST_FAIL);																		   \
		}																						   \
	} while (0)
/**
 * @brief Translater operator token into operator AST node type
 * @param curr_token Actual token pointer to be translated
 * @return OpType of AST node to be created
 */
OpType token_to_op(Token *curr_token) {
	switch (curr_token->type) {
		case TOKEN_MUL:
			return OP_MUL;
		case TOKEN_DIV:
			return OP_DIV;
		case TOKEN_PLUS:
			return OP_ADD;
		case TOKEN_MINUS:
			return OP_SUB;
		case TOKEN_LT:
			return OP_LT;
		case TOKEN_GT:
			return OP_GT;
		case TOKEN_LTE:
			return OP_LTE;
		case TOKEN_GTE:
			return OP_GTE;
		case TOKEN_EQ:
			return OP_EQ;
		case TOKEN_NEQ:
			return OP_NEQ;
		case TOKEN_AND:
			return OP_AND;
		case TOKEN_OR:
			return OP_OR;
		case TOKEN_ASSIGN:
			return OP_ASSIGN;
		default:
			return OP_ADD;
	}
}

/**
 * @brief Function for converting token into precedence table index
 * @param curr_token Actual token pointer to be translated
 * @param prev_op Flag if previously parsed token was operation
 * @param paren_depth Current parenthesis depth
*/
static PrecIndex token_to_prec_idx(Token *curr_token, bool prev_was_op, int paren_depth) {
	switch (curr_token->type) {
		case TOKEN_MINUS:
			return prev_was_op ? IDX_SUB : IDX_UMINUS;

		case TOKEN_NOT:
			return IDX_NOT;

		case TOKEN_MUL:
			return IDX_MUL;

		case TOKEN_DIV:
			return IDX_DIV;

		case TOKEN_PLUS:
			return IDX_ADD;

		case TOKEN_LT:
			return IDX_LT;

		case TOKEN_GT:
			return IDX_GT;

		case TOKEN_LTE:
			return IDX_LTE;

		case TOKEN_GTE:
			return IDX_GTE;

		case TOKEN_EQ:
			return IDX_EQ;

		case TOKEN_NEQ:
			return IDX_NEQ;

		case TOKEN_AND:
			return IDX_AND;

		case TOKEN_OR:
			return IDX_OR;

		case TOKEN_ASSIGN:
			return IDX_ASSIGN;

		case TOKEN_LPAREN:
			return IDX_LPAREN;

		case TOKEN_RPAREN:
			return (paren_depth > 0) ? IDX_RPAREN : IDX_DOLLAR;

		case TOKEN_IDENTIFIER:
		case TOKEN_LIT_INT:
		case TOKEN_LIT_FLOAT:
		case TOKEN_LIT_STRING:
		case TOKEN_KEYWORD_TRUE:
		case TOKEN_KEYWORD_FALSE:
			return IDX_ID;

		default:
			return IDX_DOLLAR;
	}
}

/**
 * @brief Function for allocating of stack item, control of pointer done by caller
*/
static PrecStackItem *malloc_stack_item (){
	PrecStackItem *item = malloc(sizeof(PrecStackItem));
	return item;
}

/**
 * @brief Function for destroying precedence stack
 * @param stack Stack structure to destroy
 *
*/
void destroy_prec_stack(Stack stack){
	return;
}

/**
 * @brief Parsing of expression via stack
 * @param curr_token Current token given by scanner
 * @return ParserStatus structure containing flags for parser
 */
static ParserStatus parse_stack(Token *curr_token) {
	Stack stack;
	stack_init(&stack);
	PrecStackItem *dollar = malloc_stack_item();
	CHECK_STACK_ITEM(dollar);
	dollar = {.term = IDX_DOLLAR, .is_term = true, .has_handle = false};
	stack_push(&stack, dollar);
	bool prev_op = false;
	int paren_depth = 0;
	while (true){
		PrecStackItem top_term;
		PrecIndex idx = token_to_prec_idx(curr_token, prev_op, paren_depth);
	}
	stack_destroy();
}

ParserStatus parse_expression(Token *curr_token) {
	ParserStatus status = parse_stack(curr_token, 0);
	CHECK_STATUS(status);
	return PARSE_OK(PAR_SUCC, status.new_node);
}