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
#include "../lexer/lexer.h"
#include "ast.h"
#include "parser.h"
#include <stdbool.h>
#include <stdio.h>

static ParserStatus parse_climb(Token *curr_token, PrecLevel min_prec);

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
 * @brief Translater operator token into its precedence
 * @param curr_token Actual token pointer to be translated
 * @return Precedence level of token, NONE if precedence undefined
 */
static PrecLevel translate_token(Token *curr_token) {
	switch (curr_token->type) {
		case TOKEN_NOT:
			return UNARY;
		case TOKEN_MUL:
		case TOKEN_DIV:
			return MULTI;
		case TOKEN_PLUS:
		case TOKEN_MINUS:
			return ADDITIVE;
		case TOKEN_LT:
		case TOKEN_GT:
		case TOKEN_LTE:
		case TOKEN_GTE:
			return RELATIONAL;
		case TOKEN_EQ:
		case TOKEN_NEQ:
			return EQUALITY;
		case TOKEN_AND:
			return AND;
		case TOKEN_OR:
			return OR;
		case TOKEN_ASSIGN:
			return ASSIGN;
		default:
			return NONE;
	}
}

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
 * @brief Parsing of call parameters of member function
 * @param curr_token
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
		ParserStatus status = parse_climb(curr_token, 0);
		CHECK_STATUS_OWNED(status, node);
		CHECK_AST_APPEND(node, ast_window_add_param(window, status.new_node));
		while (true) {
			if (curr_token->type == TOKEN_RPAREN) {
				break;
			} else if (curr_token->type == TOKEN_COMMA) {
				fprintf(stderr, "Last_iter\n");
				GET_TOKEN(curr_token, node, false);
				status = parse_climb(curr_token, 0);
				CHECK_STATUS_OWNED(status, node);
				if (status.new_node != NULL){
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
 * @brief Parsing of right asociativity operators and function calls
 * @param curr_token Current token given by scanner
 * @return ParserStatus structure containing flags for parser
 */
ParserStatus parse_prefix(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *node = NULL;
	OpType op;
	bool skip_fetch = false;
	switch (curr_token->type) {
		case TOKEN_MINUS:
		case TOKEN_NOT:
			op = (curr_token->type == TOKEN_MINUS) ? OP_UNARY_MINUS : OP_NOT;
			GET_TOKEN(curr_token, NULL, false);
			status = parse_prefix(curr_token);
			CHECK_STATUS(status);
			ALLOC_NODE(
				node, ast_create_unop(op, status.new_node, curr_token->line, curr_token->col));
			skip_fetch = true;
			break;
		case TOKEN_LIT_INT:
			ALLOC_NODE(
				node, ast_create_int(curr_token->data.int_val, curr_token->line, curr_token->col));
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
		case TOKEN_LPAREN:
			GET_TOKEN(curr_token, NULL, false);
			if (curr_token->type == TOKEN_RPAREN) {
				ALLOC_NODE(node, ast_create_null(curr_token->line, curr_token->col));
			} else {
				status = parse_climb(curr_token, 0);
				CHECK_STATUS(status);
				if (curr_token->type != TOKEN_RPAREN) {
					ast_destroy(status.new_node);
					return PARSE_ERROR(
						ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							"Invalid token in expression prefix parsing (expected TOKEN_RBRACE)\n"),
						PAR_FAIL);
				}
				node = status.new_node;
			}
			break;
		case TOKEN_RPAREN:
			return PARSE_OK(PAR_SUCC, NULL);
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid token in expression prefix parsing\n"),
				PAR_FAIL);
			break;
	}
	if (curr_token->type != TOKEN_RBRACE && curr_token->type != TOKEN_EOL &&
		curr_token->type != TOKEN_COMMA && curr_token->type != TOKEN_SEMICOLON && !skip_fetch) {
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
 * @brief Parsing of operators
 * @param curr_token Current token given by scanner
 * @param min_prec Minimal precedence expected to be parsed
 * @return ParserStatus structure containing flags for parser
 */
static ParserStatus parse_climb(Token *curr_token, PrecLevel min_prec) {
	fprintf(stderr, "[precedence] parse_climb: min_prec=%d current=%s at %zu:%zu\n", min_prec,
		token_type_name(curr_token->type), curr_token->line, curr_token->col);
	ParserStatus status = parse_prefix(curr_token);
	CHECK_STATUS(status);
	ASTNode *node = status.new_node;
	while (true) {
		PrecLevel prec = translate_token(curr_token);
		if (prec == NONE || prec < min_prec) {
			break;
		}
		OpType op_type = token_to_op(curr_token);
		fprintf(stderr, "[precedence] parse_climb: operator=%s precedence=%d\n",
			token_type_name(curr_token->type), prec);
		GET_TOKEN(curr_token, node, false);
		PrecLevel new_min_prec = (op_type == OP_ASSIGN) ? prec : (prec + 1);
		status = parse_climb(curr_token, new_min_prec);
		CHECK_STATUS_OWNED(status, node);
		ASTNode *bin_op = NULL;
		ALLOC_NODE(bin_op,
			ast_create_binop(op_type, node, status.new_node, curr_token->line, curr_token->col));
		prec = translate_token(curr_token);
		if (op_type == OP_EQ || op_type == OP_NEQ || op_type == OP_LT || op_type == OP_GT ||
			op_type == OP_LTE || op_type == OP_GTE) {
			prec = translate_token(curr_token);
			if (prec == RELATIONAL || prec == EQUALITY) {
				ast_destroy(bin_op);
				return PARSE_ERROR(
					ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
						"Expression parsing error, two relational operators back to back\n"),
					PAR_FAIL);
			}
		}
		node = bin_op;
	}
	return PARSE_OK(PAR_SUCC, node);
}

ParserStatus parse_expression(Token *curr_token) {
	ParserStatus status = parse_climb(curr_token, 0);
	CHECK_STATUS(status);
	return PARSE_OK(PAR_SUCC, status.new_node);
}