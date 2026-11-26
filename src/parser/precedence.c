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

#define PREC_TABLE_DIM 19  	/**< Size of both dimensions of precedence table */
#define MAX_RULE_LEN 4		/**< Max length of rule to be handled in reducing */

/**
 * @brief Precedence table for evaluating expressions
*/
static const char precedence_table[PREC_TABLE_DIM][PREC_TABLE_DIM] = {
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

static void precedence_trace_token(const char *location, const Token *token) {
	const char *value = "";
	if (token->type == TOKEN_IDENTIFIER || token->type == TOKEN_LIT_STRING) {
		value = token->data.str_val ? dstring_get(token->data.str_val) : "<null>";
	}
	fprintf(stderr, "[precedence] %s: %s at %zu:%zu", location, token_type_name(token->type),
		token->line, token->col);
	if (*value)
		fprintf(stderr, " value=\"%s\"", value);
	fputc('\n', stderr);
}

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
			destroy_prec_stack(stack, true);													   \
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,         \
								   "Failed to allocate stack node"),                      		   \
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
		case TOKEN_KEYWORD_IF:
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
 * @param stack Pointer of stack structure to destroy
 * @param destroy_ast Flag whether inner AST should be destroyed
*/
static void destroy_prec_stack(Stack *stack, bool destroy_ast){
	while (stack->top != NULL) {
		PrecStackItem *item = (PrecStackItem *)stack_pop(stack);
		if (item != NULL) {
			if (destroy_ast && item->node != NULL) {
				ast_destroy(item->node);
			}
			free(item);
		}
	}
}

/**
 * @brief Function for returning top terminal from stack
 * @param stack Stack to get the top symbol from
 * @return Terminal stack node if success, NULL if terminal not found
*/
static PrecStackItem *get_top_term(Stack *stack) {
	StackElement *elem = stack_top(stack);
	while (elem != NULL) {
		PrecStackItem *item = (PrecStackItem *)elem->data;
		if (item != NULL && item->is_term) {
			return item;
		}
		elem = elem->next;
	}
	return NULL;
}

/**
 * @brief Parsing of call parameters of member function
 * @param curr_token Actual token pointer to be translated
 * @param node Node to append parameters into
 */
ParserStatus parse_call_parameters(Token *curr_token, ASTNode *node) {
	bool first_iter = true;
	do {
		if (curr_token->type == TOKEN_LPAREN && !first_iter) {
			GET_TOKEN(curr_token, node, false);
		}
		ASTNode *window = NULL;
		ALLOC_NODE(window, ast_create_param_window(curr_token->line, curr_token->col));
		CHECK_AST_APPEND(node, ast_call_add_window(node, window));
		if (curr_token->type == TOKEN_RPAREN) {
			GET_TOKEN(curr_token, node, false);
			first_iter = false;
			if (curr_token->type == TOKEN_EOL || curr_token->type == TOKEN_SEMICOLON) {
				break;
			}
			continue;
		}
		ParserStatus status = parse_stack(curr_token);
		APPEND_BEFORE_EOF_RETURN(status, window, ast_window_add_param(window, status.new_node));
		CHECK_STATUS_OWNED(status, node);
		CHECK_AST_APPEND(node, ast_window_add_param(window, status.new_node));
		while (true) {
			if (curr_token->type == TOKEN_RPAREN) {
				break;
			} else if (curr_token->type == TOKEN_COMMA) {
				GET_TOKEN(curr_token, node, false);
				if (curr_token->type == TOKEN_RPAREN) {
					break;
				}
				status = parse_stack(curr_token);
				APPEND_BEFORE_EOF_RETURN(
					status, window, ast_window_add_param(window, status.new_node));
				CHECK_STATUS_OWNED(status, node);
				if (status.new_node != NULL) {
					CHECK_AST_APPEND(node, ast_window_add_param(window, status.new_node));
				}
			} else {
				ast_destroy(node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid token in expression call parameter parsing "
									   "(expected TOKEN_RBRACE or TOKEN_COMMA)\n"),
					PAR_FAIL);
			}
		}
		GET_TOKEN(curr_token, node, false);
		if (curr_token->type == TOKEN_EOL) {
			break;
		}
		first_iter = false;
	} while (curr_token->type == TOKEN_LPAREN);
	return PARSE_OK(PAR_SUCC, node);
}

/**
 * @brief Parsing of non-terminal after operand
 * @param curr_token Current token given by scanner
 * @return ParserStatus structure containing flags for parser
 */
static ParserStatus parse_operand(Token *curr_token){
	ParserStatus status = {0};
	ASTNode *node = NULL;
	bool skip_fetch = false;
	switch (curr_token->type) {
		case TOKEN_LIT_INT:
			ALLOC_NODE(
				node, ast_create_int(curr_token->data.int_val, curr_token->line, curr_token->col));
			break;

		case TOKEN_KEYWORD_TRUE:
			ALLOC_NODE(node, ast_create_bool(true, curr_token->line, curr_token->col));
			break;

		case TOKEN_KEYWORD_FALSE:
			ALLOC_NODE(node, ast_create_bool(false, curr_token->line, curr_token->col));
			break;

		case TOKEN_KEYWORD_IF:
			skip_fetch = true;
			status = parse_if_statement(curr_token);
			CHECK_STATUS(status);
			node = status.new_node;
			break;

		case TOKEN_LIT_FLOAT:
			ALLOC_NODE(node,
				ast_create_float(curr_token->data.float_val, curr_token->line, curr_token->col));
			break;

		case TOKEN_LIT_STRING:
			ALLOC_NODE(node,
				ast_create_string(curr_token->data.str_val, curr_token->line, curr_token->col));
			break;

		case TOKEN_IDENTIFIER:
			ALLOC_NODE(node,
				ast_create_ident(curr_token->data.str_val, curr_token->line, curr_token->col));
			break;

		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid token in expression operand parsing\n"),
				PAR_FAIL);
			break;
	}

	if (!skip_fetch) {
		GET_TOKEN(curr_token, node, true);
	}

	while (curr_token->type == TOKEN_DOT || curr_token->type == TOKEN_LPAREN) {
		if (curr_token->type == TOKEN_DOT) {
			GET_TOKEN(curr_token, node, false);
			if (curr_token->type != TOKEN_IDENTIFIER) {
				ast_destroy(node);
				return PARSE_ERROR(
					ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
						"Expected identifier after . function call, got something else\n"),
					PAR_FAIL);
			}
			ASTNode *tmp_node = NULL;
			ALLOC_NODE(tmp_node,
				ast_create_call(curr_token->data.str_val, curr_token->line, curr_token->col));
			CHECK_AST_APPEND(tmp_node, ast_call_add_target(tmp_node, node));
			// node is now the call target, so tmp_node owns it
			GET_TOKEN(curr_token, tmp_node, false);
			if (curr_token->type == TOKEN_LPAREN) {
				GET_TOKEN(curr_token, tmp_node, false);
				status = parse_call_parameters(curr_token, tmp_node);
				CHECK_STATUS(status);
			}
			node = tmp_node;
		} else {
			ASTNode *tmp_node = NULL;
			dstring_t name;
			if (node->type == NODE_IDENTIFIER) {
				name = node->str_val;
			} else {
				if (!dstring_init(&name)) {
					ast_destroy(node);
					return PARSE_ERROR(
						ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
							"Failed to create dummy string for wrong function call\n"),
						AST_FAIL);
				}
			}
			ALLOC_NODE(tmp_node, ast_create_call(&name, curr_token->line, curr_token->col));
			// ast_create_call copies the name, so only the placeholder needs releasing here
			if (node->type != NODE_IDENTIFIER) {
				dstring_free(&name);
			}
			GET_TOKEN(curr_token, node, false);
			status = parse_call_parameters(curr_token, tmp_node);
			if (status.flag != PAR_SUCC) {
				ast_destroy(node);
				return status;
			}
			ast_destroy(node);
			node = status.new_node;
		}
	}
	return PARSE_OK(PAR_SUCC, node);
}

/**
 * @brief Handling of reduction
 * @param curr_token Current token given by scanner
 * @param stack Precedence analysis stack
 * @return ParserStatus structure containing flags for parser
 */
static ParserStatus reduce_handle(Token *curr_token, Stack *stack){
	if (stack->top == NULL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Empty stack during reduction\n"),
			PAR_FAIL);
	}

	PrecStackItem *s0 = (PrecStackItem *)stack->top->data; // Topmost rihtmost item
	PrecStackItem *s1 = (stack->top->next != NULL) ? (PrecStackItem *)stack->top->next->data : NULL;
	PrecStackItem *s2 = (s1 != NULL && stack->top->next->next != NULL)
		? (PrecStackItem *)stack->top->next->next->data
		: NULL;

	// 1. Rule: E -> ( E )   [stack top: s0=')', s1=E, s2='(']
	if (s0 != NULL && s1 != NULL && s2 != NULL &&
		s0->is_term && s0->term_idx == IDX_RPAREN &&
		!s1->is_term &&
		s2->is_term && s2->term_idx == IDX_LPAREN) {
		free(stack_pop(stack)); // pop ')'
		PrecStackItem *expr = (PrecStackItem *)stack_pop(stack); // pop E
		free(stack_pop(stack)); // pop '('
		stack_push(stack, expr);
		return PARSE_OK(PAR_SUCC, NULL);
	}

	// 2. Rule: E -> ( )     [stack top: s0=')', s1='(']
	if (s0 != NULL && s1 != NULL &&
		s0->is_term && s0->term_idx == IDX_RPAREN &&
		s1->is_term && s1->term_idx == IDX_LPAREN) {
		PrecStackItem *rparen = (PrecStackItem *)stack_pop(stack);
		PrecStackItem *lparen = (PrecStackItem *)stack_pop(stack);
		ASTNode *unit_node = ast_create_null(lparen->line, lparen->col);
		free(rparen);
		if (unit_node == NULL) {
			free(lparen);
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
								   "Failed to allocate unit node\n"),
				PAR_FAIL);
		}
		lparen->is_term = false;
		lparen->node = unit_node;
		stack_push(stack, lparen);
		return PARSE_OK(PAR_SUCC, NULL);
	}

	// 3. Rule: E -> E binop E   [stack top: s0=RHS E, s1=binop, s2=LHS E]
	if (s0 != NULL && s1 != NULL && s2 != NULL &&
		!s0->is_term && s1->is_term && !s2->is_term) {
		PrecStackItem *rhs = (PrecStackItem *)stack_pop(stack);
		PrecStackItem *op = (PrecStackItem *)stack_pop(stack);
		PrecStackItem *lhs = (PrecStackItem *)stack_pop(stack);

		if (op->op == OP_ASSIGN && lhs->node->type != NODE_IDENTIFIER) {
			ast_destroy(lhs->node);
			ast_destroy(rhs->node);
			free(lhs);
			free(op);
			free(rhs);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Tried assigning into literal or expression\n"),
				PAR_FAIL);
		}
		ASTNode *bin_node = ast_create_binop(op->op, lhs->node, rhs->node, op->line, op->col);
		free(op);
		if (bin_node == NULL) {
			ast_destroy(lhs->node);
			ast_destroy(rhs->node);
			free(rhs);
			free(lhs);
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
								   "Failed to allocate binop node\n"),
				PAR_FAIL);
		}
		free(rhs);
		lhs->node = bin_node;
		stack_push(stack, lhs);
		return PARSE_OK(PAR_SUCC, NULL);
	}

	// 4. Rule: E -> unop E   [stack top: s0=E, s1=u- or !]
	if (s0 != NULL && s1 != NULL &&
		!s0->is_term && s1->is_term &&
		(s1->term_idx == IDX_UMINUS || s1->term_idx == IDX_NOT)) {
		PrecStackItem *expr = (PrecStackItem *)stack_pop(stack);
		PrecStackItem *op = (PrecStackItem *)stack_pop(stack);
		ASTNode *un_node = ast_create_unop(op->op, expr->node, op->line, op->col);
		if (un_node == NULL) {
			ast_destroy(expr->node);
			free(expr);
			free(op);
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
								   "Failed to allocate unop node\n"),
				PAR_FAIL);
		}
		free(expr);
		op->is_term = false;
		op->node = un_node;
		stack_push(stack, op);
		return PARSE_OK(PAR_SUCC, NULL);
	}

	return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
						   "Error while reducing the expression\n"),
		PAR_FAIL);
}

/**
 * @brief Parses an expression using bottom-up operator-precedence parsing
 * @param curr_token Current token from scanner (updated in-place as tokens are consumed)
 * @return ParserStatus containing the resulting expression AST root or a syntax/lexical error
 */
static ParserStatus parse_stack(Token *curr_token) {
	Stack stack;
	stack_init(&stack);

	// Push '$' sentinel as the bottom boundary of the precedence stack
	PrecStackItem *dollar = malloc_stack_item();
	CHECK_STACK_ITEM(dollar, &stack);
	*dollar = (PrecStackItem){.term_idx = IDX_DOLLAR, .is_term = true, .has_handle = false};
	stack_push(&stack, dollar);

	bool prev_op = false;  // Tracks whether the previous token was an operand/value (for unary '-' disambiguation)
	int paren_depth = 0;   // Tracks open '(' depth inside this expression

	while (true) {
		// Find topmost terminal symbol on the stack to compare against incoming token
		PrecStackItem *top_term = get_top_term(&stack);
		if (top_term == NULL) {
			destroy_prec_stack(&stack, true);
			return PARSE_ERROR(
				ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
					"Expected terminal in stack, found none\n"),
				PAR_FAIL);
		}

		PrecIndex curr_idx = token_to_prec_idx(curr_token, prev_op, paren_depth);

		// Acceptance check: both stack top terminal and incoming symbol are '$'
		if (top_term->term_idx == IDX_DOLLAR && curr_idx == IDX_DOLLAR) {
			StackElement *top_el = stack_top(&stack);
			PrecStackItem *top_item = (top_el != NULL) ? (PrecStackItem *)top_el->data : NULL;
			if (stack.size != 2 || top_item == NULL || top_item->is_term || top_item->node == NULL) {
				destroy_prec_stack(&stack, true);
				return PARSE_ERROR(
					ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
						"Invalid or incomplete expression\n"),
					PAR_FAIL);
			}
			// Detach finished AST root before freeing the remaining stack items
			ASTNode *res_node = top_item->node;
			top_item->node = NULL;
			destroy_prec_stack(&stack, true);
			return PARSE_OK((curr_token->type == TOKEN_EOF) ? PAR_EOF : PAR_SUCC, res_node);
		}

		char action = precedence_table[top_term->term_idx][curr_idx];

		if (action == '<') {
			// Shift: incoming token has higher precedence (or opens a sub-expression)
			top_term->has_handle = true;
			if (curr_idx == IDX_ID) {
				// Reject two consecutive operands without an operator between them
				if (prev_op) {
					destroy_prec_stack(&stack, true);
					return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
										"Unexpected operand without operator\n"),
						PAR_FAIL);
				}
				// Parse atomic operand (literal, identifier, call, or if-expression) as non-terminal E
				ParserStatus status = parse_operand(curr_token);
				if (status.flag != PAR_SUCC && status.flag != PAR_EOF) {
					destroy_prec_stack(&stack, true);
					return status;
				}
				PrecStackItem *item = malloc_stack_item();
				CHECK_STACK_ITEM(item, &stack);
				*item = (PrecStackItem){
					.term_idx = IDX_ID, .is_term = false, .node = status.new_node};
				stack_push(&stack, item);
				prev_op = true;
			} else {
				// Shift operator or opening '(' onto the stack
				if (curr_idx == IDX_LPAREN) {
					paren_depth++;
				}
				PrecStackItem *item = malloc_stack_item();
				CHECK_STACK_ITEM(item, &stack);
				OpType oper = (curr_idx == IDX_UMINUS) ? OP_UNARY_MINUS
							: (curr_idx == IDX_NOT)    ? OP_NOT
													   : token_to_op(curr_token);
				*item = (PrecStackItem){.term_idx = curr_idx,
					.is_term = true,
					.op = oper,
					.line = curr_token->line,
					.col = curr_token->col};
				stack_push(&stack, item);
				prev_op = false;
				GET_TOKEN(curr_token, NULL, false);
			}
		} else if (action == '>') {
			// Reduce: stack top has higher precedence than incoming token; fold top handle into AST node
			ParserStatus status = reduce_handle(curr_token, &stack);
			if (status.flag != PAR_SUCC) {
				destroy_prec_stack(&stack, true);
				return status;
			}
		} else if (action == '=') {
			// Equal precedence: match closing ')' with opening '(' without returning early on EOF
			paren_depth--;
			PrecStackItem *item = malloc_stack_item();
			CHECK_STACK_ITEM(item, &stack);
			*item = (PrecStackItem){.term_idx = IDX_RPAREN,
				.is_term = true,
				.line = curr_token->line,
				.col = curr_token->col};
			stack_push(&stack, item);
			prev_op = true;

			token_destroy(curr_token);
			Error err = lexer_get_next_token(curr_token);
			if (ERROR_IS_ERR(err)) {
				destroy_prec_stack(&stack, true);
				return PARSE_ERROR(err, LEX_FAIL);
			}
			precedence_trace_token(__func__, curr_token);
		} else {
			// Blank entry in precedence table -> invalid token combination
			destroy_prec_stack(&stack, true);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid operator sequence in expression\n"),
				PAR_FAIL);
		}
	}
}


ParserStatus parse_expression(Token *curr_token) {
	ParserStatus status = parse_stack(curr_token);
	CHECK_STATUS(status);
	return PARSE_OK(PAR_SUCC, status.new_node);
}