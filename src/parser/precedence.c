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
 * @brief Structure for items of stack durring parsing exprs
*/
typedef struct {
	ASTNode *node;		/**< ASTNode in case node contains some literal or function call */
	bool is_term;		/**< Tracks whether construct is terminal or no */
	bool has_handle;
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
 * @brief Function for allocating of stack item, control of pointer done by caller
*/
static PrecStackItem malloc_stack_item (){
	PrecStackItem item = malloc(sizeof(PrecStackItem));
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
	PrecStackItem dollar = malloc_stack_item();
	CHECK_STACK_ITEM(dollar);
	stack_free(&stack);
}

ParserStatus parse_expression(Token *curr_token) {
	ParserStatus status = parse_stack(curr_token, 0);
	CHECK_STATUS(status);
	return PARSE_OK(PAR_SUCC, status.new_node);
}