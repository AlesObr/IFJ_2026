/**
 * @project IFJ26 Compiler
 * @file parser.c
 * @brief Syntactic analyzer and recursive descent interface.
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "parser.h"
#include "../_utils/dstring.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include "../lexer/lexer.h"
#include "ast.h"
#include "precedence.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Declarations of functions with cyclic dependency
ParserStatus parse_param_mem(Token *curr_token);
ParserStatus parse_func_def(Token *curr_token, bool main_flag);
ParserStatus parse_if_statement(Token *curr_token);
ParserStatus parse_statement(Token *curr_token);
ParserStatus parse_code_block(Token *curr_token);
ParserStatus parse_func_body(Token *curr_token);

/**
 * @brief Macro for safely fetching from scanner
 * @param tok_ptr Token pointer to fetch into
 * @param err Error variable to catch scanner errors
 * @param destroy_node Current rule ASTNode to be destroyed on fail
 */
#define GET_TOKEN(tok_ptr, err, destroy_node)                                                      \
	do {                                                                                           \
		token_destroy(tok_ptr);                                                                    \
		(err) = lexer_get_next_token((tok_ptr));                                                   \
		if (ERROR_IS_ERR((err))) {                                                                 \
			if (destroy_node != NULL) {                                                            \
				ast_destroy(destroy_node);                                                         \
			}                                                                                      \
			return PARSE_ERROR((err), LEX_FAIL);                                                   \
		}                                                                                          \
		if ((tok_ptr)->type == TOKEN_EOF) {                                                        \
			return PARSE_OK(PAR_EOF, NULL);                                                        \
		}                                                                                          \
	} while (0)

/**
 * @brief Macro for checking whether AST node allocation was successful
 * @param node Node to asign the result of AST node allocation into
 * @param alloc_expr Function call for generating AST nodes
 */
#define ALLOC_NODE(node, alloc_expr)                                                               \
	do {                                                                                           \
		(node) = (alloc_expr);                                                                     \
		if ((node) == NULL) {                                                                      \
			return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,         \
								   "Failed to allocate node for AST tree\n"),                      \
				AST_FAIL);                                                                         \
		}                                                                                          \
	} while (0)

/**
 * @brief Macro for cheking whether appending node into AST tree was success
 * @param node ASTNode into which is appended, to be destroyed on fail
 * @param append_expr Append function call returning boolean
 * @param err_msg Error message to be sent in parser status to previous function caller
 */
#define CHECK_AST_APPEND(node, append_expr, err_msg)                                               \
	do {                                                                                           \
		if (!(append_expr)) {                                                                      \
			ast_destroy(node);                                                                     \
			return PARSE_ERROR(                                                                    \
				ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col, (err_msg)), AST_FAIL); \
		}                                                                                          \
	} while (0)

/**
 * @brief Macro for checking status flags
 * @param status Status structure to be checked
 */
#define CHECK_STATUS(status)                                                                       \
	do {                                                                                           \
		if ((status).flag != PAR_SUCC) {                                                           \
			return (status);                                                                       \
		}                                                                                          \
	} while (0)
// handle not yet assigned nodes after error
/**
 * @brief Parsing of mandatory program header
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_header(Token *curr_token) {
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_KEYWORD_IMPORT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing mandatory import header (import ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_IDENTIFIER ||
		dstring_cmp_str(curr_token->data.str_val, "ifj") != 0) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_KEYWORD_DO) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing ifj type in import (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_MUL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after header declaration\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, NULL);
}

/**
 * @brief Parsing extesion of @main function mark
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_at_main(Token *curr_token) {
	ParserStatus status = {0};
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, (Error){0}, NULL);
		if (curr_token->type == TOKEN_KEYWORD_DEF) {
			status = parse_func_def(curr_token, true);
			CHECK_STATUS(status);
		} else {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing definition token after @main\n"),
				PAR_FAIL);
		}
	} else if (curr_token->type == TOKEN_KEYWORD_DEF) {
		status = parse_func_def(curr_token, true);
		CHECK_STATUS(status);
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing definition token after @main\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, status.new_node);
}

/**
 * @brief Parsing of code blocks
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_code_block(Token *curr_token) {
	ParserStatus status = {0};
	if (curr_token->type != TOKEN_INDENT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected start of indented block, got something else\n"),
			PAR_FAIL);
	}
	ASTNode *new_block;
	ALLOC_NODE(new_block, ast_create_block(curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, (Error){0}, new_block);
	while (curr_token->type != TOKEN_OUTDENT) {
		status = parse_statement(curr_token);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(new_block, ast_block_add_stmt(new_block, status.new_node),
			"Failed to append statement into block in multi line function body\n");
		GET_TOKEN(curr_token, (Error){0}, new_block);
	}
	return PARSE_OK(PAR_SUCC, new_block);
}

/**
 * @brief Parsing of declarations
 * @param curr_token Token pointer filled with current token
 * @param mutability Flag whether declared variable is mutable (true for var, false for val)
 * @return Status structure containing flags for caller
 */
ParserStatus parse_declaration(Token *curr_token, bool mutability) {
	ASTNode *new_decl;
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier name after function declaration\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(new_decl, ast_create_var_decl(curr_token->data.str_val, mutability, curr_token->line,
							 curr_token->col));
	GET_TOKEN(curr_token, (Error){0}, new_decl);
	if (curr_token->type == TOKEN_COLON) {
		GET_TOKEN(curr_token, (Error){0}, new_decl);
		ValType type;
		switch (curr_token->type) {
			case TOKEN_KEYWORD_INT:
				type = INT;
				break;
			case TOKEN_KEYWORD_DOUBLE:
				type = DOUBLE;
				break;
			case TOKEN_KEYWORD_UNIT:
				type = UNIT;
				break;
			case TOKEN_KEYWORD_STRING:
				type = STRING;
				break;
			case TOKEN_KEYWORD_BOOLEAN:
				type = BOOLEAN;
				break;
			default:
				ast_destroy(new_decl);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing data type in function parameter\n"),
					PAR_FAIL);
		}
		ast_add_type(new_decl, type);
	}
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type == TOKEN_EQ) {
		ParserStatus status = parse_expression(curr_token);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(new_decl, ast_var_decl_add_init(new_decl, status.new_node),
			"Failed to append init expression to variable declaration\n");
	} else {
		ast_destroy(new_decl);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing assignment after variable declaration\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, new_decl);
}

/**
 * @brief Parsiong of else decision branch
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_else_statement(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *new_else;
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type == TOKEN_EOL) {
		ALLOC_NODE(new_else, ast_create_else(NULL, curr_token->line, curr_token->col));
		GET_TOKEN(curr_token, (Error){0}, NULL);
		status = parse_code_block(curr_token);
		CHECK_AST_APPEND(new_else, ast_if_else_add_body(new_else, status.new_node),
			"Failed to append function body of else block\n");
		return PARSE_OK(PAR_SUCC, new_else);
	} else if (curr_token->type == TOKEN_KEYWORD_IF) {
		GET_TOKEN(curr_token, (Error){0}, NULL);
		status = parse_expression(curr_token);
		CHECK_STATUS(status);
		ALLOC_NODE(new_else, ast_create_else(status.new_node, curr_token->line, curr_token->col));
		if (curr_token->type != TOKEN_KEYWORD_THEN) {
			ast_destroy(new_else);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing then after else statement with condition\n"),
				PAR_FAIL);
		}
		GET_TOKEN(curr_token, (Error){0}, NULL);
		if (curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, (Error){0}, NULL);
			status = parse_code_block(curr_token);
			CHECK_AST_APPEND(new_else, ast_if_else_add_body(new_else, status.new_node),
				"Failed to append function body of else block\n");
			return PARSE_OK(PAR_SUCC, new_else);
		} else {
			ast_destroy(new_else);
			return PARSE_ERROR(
				ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
					"Unexpected token after else statemenent (expected if or EOL)\n"),
				PAR_FAIL);
		}
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token after else statemenent (expected if or EOL)\n"),
			PAR_FAIL);
	}
}

/**
 * @brief Parsing of if decision statement
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_if_statement(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *new_if;
	status = parse_expression(curr_token);
	CHECK_STATUS(status);
	ALLOC_NODE(new_if, ast_create_if(status.new_node, curr_token->line, curr_token->col));
	if (curr_token->type != TOKEN_KEYWORD_THEN) {
		ast_destroy(new_if);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing then after if statement condition\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, new_if);
	if (curr_token->type != TOKEN_EOL) {
		ast_destroy(new_if);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after then of if statement\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, new_if);
	status = parse_code_block(curr_token);
	CHECK_STATUS(status);
	CHECK_AST_APPEND(new_if, ast_if_else_add_body(new_if, status.new_node),
		"Failed to append function body of if block\n");
	GET_TOKEN(curr_token, (Error){0}, new_if);
	while (true)
		if (curr_token->type == TOKEN_KEYWORD_ELSE) {
			status = parse_else_statement(curr_token);
			CHECK_STATUS(status);
			CHECK_AST_APPEND(new_if, ast_if_else_append_branch(new_if, status.new_node),
				"Failed to append else block of if block\n");
		} else {
			break;
		}
	return PARSE_OK(PAR_SUCC, new_if);
}

/**
 * @brief Parsing of while statement
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_while_statement(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *new_while;
	status = parse_expression(curr_token);
	CHECK_STATUS(status);
	if (curr_token->type != TOKEN_KEYWORD_DO) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected do after condition, got other token\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(new_while, ast_create_while(status.new_node, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, (Error){0}, new_while);
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after do of while statement\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, new_while);
	status = parse_code_block(curr_token);
	CHECK_STATUS(status);
	CHECK_AST_APPEND(new_while, ast_while_add_body(new_while, status.new_node),
		"Failed to append block of while statement\n");
	return PARSE_OK(PAR_SUCC, new_while);
}

/**
 * @brief Parsing of for statement generator
 * @param curr_token Token pointer filled with current token
 * @param for_node For loop node to append into
 * @return Status structure containing flags for caller
 */
ParserStatus parse_for_generator(Token *curr_token, ASTNode *for_node) {
	ParserStatus status = parse_expression(curr_token);
	CHECK_STATUS(status);
	CHECK_AST_APPEND(for_node, ast_for_add_start(for_node, status.new_node),
		"Failed to append generator expressiong to for AST node\n");
	if (dstring_cmp_str(curr_token->data.str_val, "to") == 0) {
		GET_TOKEN(curr_token, (Error){0}, NULL);
		status = parse_expression(curr_token);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(for_node, ast_for_add_end(for_node, status.new_node, true),
			"Failed to append generator expressiong to for AST node\n");
		if (dstring_cmp_str(curr_token->data.str_val, "by") != 0) {
			GET_TOKEN(curr_token, (Error){0}, NULL);
			status = parse_expression(curr_token);
			CHECK_STATUS(status);
			CHECK_AST_APPEND(for_node, ast_for_add_step(for_node, status.new_node),
				"Failed to append generator expressiong to for AST node\n");
		} else {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Unexpected token after end of for statement generator\n"),
				PAR_FAIL);
		}
	} else if (dstring_cmp_str(curr_token->data.str_val, "until") == 0) {
		GET_TOKEN(curr_token, (Error){0}, NULL);
		status = parse_expression(curr_token);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(for_node, ast_for_add_end(for_node, status.new_node, false),
			"Failed to append generator expressiong to for AST node\n");
		if (dstring_cmp_str(curr_token->data.str_val, "by") != 0) {
			GET_TOKEN(curr_token, (Error){0}, NULL);
			status = parse_expression(curr_token);
			CHECK_STATUS(status);
			CHECK_AST_APPEND(for_node, ast_for_add_step(for_node, status.new_node),
				"Failed to append generator expressiong to for AST node\n");
		} else {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Unexpected token after end of for statement generator\n"),
				PAR_FAIL);
		}
	} else {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token inside of for statement generator\n"),
			PAR_FAIL);
	}
	if (curr_token->type != TOKEN_EOL || dstring_cmp_str(curr_token->data.str_val, "do") != 0 ||
		curr_token->type != TOKEN_KEYWORD_IF) {
		return PARSE_OK(PAR_SUCC, NULL);
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token after end of for statement generator\n"),
			PAR_FAIL);
	}
}

/**
 * @brief Parsing of for statement
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_for_statement(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *for_node;
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, (Error){0}, NULL);
	}
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing generator after for statement\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(
		for_node, ast_create_for(curr_token->data.str_val, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, (Error){0}, for_node);
	if (curr_token->type != TOKEN_LT) {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing < after for statement generator\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, for_node);
	if (curr_token->type != TOKEN_MINUS) {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing - after < in for statement generator\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, for_node);
	CHECK_STATUS(parse_for_generator(curr_token, for_node));
	if (curr_token->type == TOKEN_EOL) {
		while (true) {
			GET_TOKEN(curr_token, (Error){0}, for_node);
			if (curr_token->type == TOKEN_KEYWORD_IF) {
				status = parse_expression(curr_token);
				CHECK_STATUS(status);
				CHECK_AST_APPEND(for_node, ast_for_append_guard(for_node, status.new_node),
					"Failed to append guard to for loop\n");
				if (curr_token->type != TOKEN_EOL) {
					return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
										   "Invalid token at the end of for loop guard\n"),
						PAR_FAIL);
				}
			}
			if (dstring_cmp_str(curr_token->data.str_val, "do") == 0 ||
				curr_token->type == TOKEN_OUTDENT) {
				break;
			}
		}
	}
	if (curr_token->type == TOKEN_OUTDENT) {
		GET_TOKEN(curr_token, (Error){0}, for_node);
	}
	if (dstring_cmp_str(curr_token->data.str_val, "do") != 0) {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected 'do' at the end of for statement, got different token\n"),
			PAR_FAIL);
	}
	CHECK_STATUS(parse_func_body(curr_token));
	return PARSE_OK(PAR_SUCC, for_node);
}

/**
 * @brief Parsing of statements
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_statement(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *node = NULL;
	switch (curr_token->type) {
		case TOKEN_KEYWORD_VAL:
			status = parse_declaration(curr_token, false);
			break;
		case TOKEN_KEYWORD_VAR:
			status = parse_declaration(curr_token, true);
			break;
		case TOKEN_KEYWORD_IF:
			status = parse_if_statement(curr_token);
			break;
		case TOKEN_KEYWORD_WHILE:
			status = parse_while_statement(curr_token);
			break;
		case TOKEN_KEYWORD_FOR:
			status = parse_for_statement(curr_token);
			break;
		case TOKEN_KEYWORD_RETURN:
			GET_TOKEN(curr_token, (Error){0}, NULL);
			if (curr_token->type != TOKEN_EOL) {
				status = parse_expression(curr_token);
				CHECK_STATUS(status);
				ALLOC_NODE(
					node, ast_create_return(status.new_node, curr_token->line, curr_token->col));
			} else {
				ALLOC_NODE(node, ast_create_return(NULL, curr_token->line, curr_token->col));
			}
			break;
		case TOKEN_IDENTIFIER:
			// add parsing of expression
			break;
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid token at the start of statement\n"),
				PAR_FAIL);
	}
	CHECK_STATUS(status);
	if (node == NULL) {
		return PARSE_OK(PAR_SUCC, status.new_node);
	} else {
		return PARSE_OK(PAR_SUCC, node);
	}
}

/**
 * @brief Parsing of inner function body
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_func_body(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *new_block;
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_EOL) {
		ALLOC_NODE(new_block, ast_create_block(curr_token->line, curr_token->col));
		status = parse_statement(curr_token);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(new_block, ast_block_add_stmt(new_block, status.new_node),
			"Failed to append statement into block in one line function body\n");
	}
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, (Error){0}, new_block);
		status = parse_code_block(curr_token);
		CHECK_STATUS(status);
		new_block = status.new_node;
	} else {
		ast_destroy(new_block);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Invalid Token after function declaration header\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, new_block);
}

/**
 * @brief Parsing of param sections of function declarations
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_param_section(Token *curr_token) {
	ASTNode *param_window;
	ALLOC_NODE(param_window, ast_create_param_window(curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, (Error){0}, param_window);
	ASTNode *param_node;
	while (true) {
		if (curr_token->type == TOKEN_RPAREN) {
			return PARSE_OK(PAR_SUCC, param_window);
		} else if (curr_token->type != TOKEN_IDENTIFIER) {
			ast_destroy(param_window);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing identifier of parameter in function declaration\n"),
				PAR_FAIL);
		}
		ALLOC_NODE(param_node,
			ast_create_func_param(curr_token->data.str_val, curr_token->line, curr_token->col));
		CHECK_AST_APPEND(param_window, ast_window_add_param(param_window, param_node),
			"Failed to add function parameter into AST\n");
		GET_TOKEN(curr_token, (Error){0}, param_window);
		if (curr_token->type != TOKEN_COLON) {
			ast_destroy(param_window);
			return PARSE_ERROR(
				ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
					"Missing colon after identifier of parameter in function declaration\n"),
				PAR_FAIL);
		}
		GET_TOKEN(curr_token, (Error){0}, param_window);
		ValType param_type = UNDEF;
		switch (curr_token->type) {
			case TOKEN_KEYWORD_INT:
				param_type = INT;
				break;
			case TOKEN_KEYWORD_DOUBLE:
				param_type = DOUBLE;
				break;
			case TOKEN_KEYWORD_UNIT:
				param_type = UNIT;
				break;
			case TOKEN_KEYWORD_STRING:
				param_type = STRING;
				break;
			case TOKEN_KEYWORD_BOOLEAN:
				param_type = BOOLEAN;
				break;
			default:
				ast_destroy(param_window);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing data type in function parameter\n"),
					PAR_FAIL);
		}
		ast_add_type(param_node, param_type);
		GET_TOKEN(curr_token, (Error){0}, param_window);
		if (curr_token->type == TOKEN_COMMA) {
			GET_TOKEN(curr_token, (Error){0}, param_window);
			if (curr_token->type == TOKEN_EOL) {
				GET_TOKEN(curr_token, (Error){0}, param_window);
			} else if (curr_token->type == TOKEN_IDENTIFIER) {
				continue;
			} else {
				ast_destroy(param_window);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid token after , in function declaration params\n"),
					PAR_FAIL);
			}
		}
		if (curr_token->type == TOKEN_RPAREN) {
			return PARSE_OK(PAR_SUCC, param_window);
		} else {
			ast_destroy(param_window);
			return PARSE_ERROR(
				ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
					"Invalid parameter list in function declaration (missing right brace)\n"),
				PAR_FAIL);
		}
	}
}

/**
 * @brief Parsing of function declarations
 * @param curr_token Token pointer filled with current token
 * @param main_flag Indicates whether function is main one (for AST tree)
 * @return Status structure containing flags for caller
 */
ParserStatus parse_func_def(Token *curr_token, bool main_flag) {
	ParserStatus status = {0};
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier in function declaration\n"),
			PAR_FAIL);
	}
	ASTNode *new_func;
	ALLOC_NODE(new_func,
		ast_create_func(curr_token->data.str_val, main_flag, curr_token->line, curr_token->col));
	while (true) {
		GET_TOKEN(curr_token, (Error){0}, new_func);
		if (curr_token->type == TOKEN_LPAREN) {
			status = parse_param_section(curr_token);
			CHECK_STATUS(status);
			CHECK_AST_APPEND(new_func, ast_func_add_param_window(new_func, status.new_node),
				"Failed to add function parameter window into AST\n");
		} else {
			break;
		}
	}
	if (curr_token->type == TOKEN_COLON) {
		GET_TOKEN(curr_token, (Error){0}, new_func);
		ValType type = UNDEF;
		switch (curr_token->type) {
			case TOKEN_KEYWORD_INT:
				type = INT;
				break;
			case TOKEN_KEYWORD_DOUBLE:
				type = DOUBLE;
				break;
			case TOKEN_KEYWORD_UNIT:
				type = UNIT;
				break;
			case TOKEN_KEYWORD_STRING:
				type = STRING;
				break;
			case TOKEN_KEYWORD_BOOLEAN:
				type = BOOLEAN;
				break;
			default:
				ast_destroy(new_func);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid function return type\n"),
					PAR_FAIL);
		}
		ast_add_type(new_func, type);
	}
	GET_TOKEN(curr_token, (Error){0}, new_func);
	if (curr_token->type == TOKEN_EQ) {
		status = parse_func_body(curr_token);
		new_func->func_decl.body = status.new_node;
		CHECK_STATUS(status);
	} else {
		ast_destroy(new_func);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token in function declaration\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, new_func);
}

/**
 * @brief Function for parsing extension parameter
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_extension_param(Token *curr_token) {
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier in extension parameter\n"),
			PAR_FAIL);
	}
	ASTNode *new_node;
	ALLOC_NODE(
		new_node, ast_ext_create(curr_token->data.str_val, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, (Error){0}, new_node);
	if (curr_token->type != TOKEN_COLON) {
		ast_destroy(new_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing colon in extension parameter\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, new_node);
	ValType type = UNDEF;
	switch (curr_token->type) {
		case TOKEN_KEYWORD_INT:
			type = INT;
			break;
		case TOKEN_KEYWORD_DOUBLE:
			type = DOUBLE;
			break;
		case TOKEN_KEYWORD_UNIT:
			type = UNIT;
			break;
		case TOKEN_KEYWORD_STRING:
			type = STRING;
			break;
		case TOKEN_KEYWORD_BOOLEAN:
			type = BOOLEAN;
			break;
		default:
			ast_destroy(new_node);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing data type in extention parameter\n"),
				PAR_FAIL);
	}
	ast_add_type(new_node, type);
	return PARSE_OK(PAR_SUCC, new_node);
}

/**
 * @brief Function for parsing functions defined in extensions
 * @param curr_token Token pointer filled with current token
 * @param ext_node Extension node to add functions into
 * @return Status structure containing flags for caller
 */
ParserStatus parse_extension_body(Token *curr_token, ASTNode *ext_node) {
	ParserStatus status = {0};
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type == TOKEN_AT_MAIN) {
		status = parse_at_main(curr_token);
		CHECK_STATUS(status);
	} else if (curr_token->type == TOKEN_KEYWORD_DEF) {
		status = parse_func_def(curr_token, false);
		CHECK_STATUS(status);
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token in function declaration inside extension\n"),
			PAR_FAIL);
	}
	CHECK_AST_APPEND(ext_node, ast_ext_add_func(ext_node, status.new_node),
		"Failed to add function into extension\n");
	while (curr_token->type != TOKEN_OUTDENT) {
		GET_TOKEN(curr_token, (Error){0}, ext_node);
		switch (curr_token->type) {
			case TOKEN_AT_MAIN:
				status = parse_at_main(curr_token);
				CHECK_STATUS(status);
				break;
			case TOKEN_KEYWORD_DEF:
				status = parse_func_def(curr_token, false);
				CHECK_STATUS(status);
				break;
			default:
				ast_destroy(ext_node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid token at end of extension declaration\n"),
					PAR_FAIL);
		}
		CHECK_AST_APPEND(ext_node, ast_ext_add_func(ext_node, status.new_node),
			"Failed to add function into extension\n");
	}
	return PARSE_OK(PAR_SUCC, NULL);
}

/**
 * @brief Function for parsing extension definition
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_extension(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *ext_header;
	GET_TOKEN(curr_token, (Error){0}, NULL);
	if (curr_token->type != TOKEN_LPAREN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing left bracket of extension parameter\n"),
			PAR_FAIL);
	}
	status = parse_extension_param(curr_token);
	CHECK_STATUS(status);
	ext_header = status.new_node;
	GET_TOKEN(curr_token, (Error){0}, ext_header);
	if (curr_token->type != TOKEN_RPAREN) {
		ast_destroy(ext_header);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing right bracket of extension parameter\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, ext_header);
	if (curr_token->type != TOKEN_EOL) {
		ast_destroy(ext_header);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after extension parameter bracket\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, ext_header);
	if (curr_token->type != TOKEN_INDENT) {
		ast_destroy(ext_header);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing left bracket of extension parameter\n"),
			PAR_FAIL);
	}
	status = parse_extension_body(curr_token, ext_header);
	CHECK_STATUS(status);
	if (curr_token->type != TOKEN_OUTDENT) {
		ast_destroy(ext_header);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing outdent at the end of extension\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, ext_header);
}

/**
 * @brief Parsing of highest level program constructs (functions, extensions)
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_program(Token *curr_token) {
	ParserStatus status = {0};
	GET_TOKEN(curr_token, (Error){0}, NULL);
	ASTNode *program = NULL;
	ALLOC_NODE(program, ast_create_block(curr_token->line, curr_token->col));
	while (curr_token->type != TOKEN_EOF) {
		switch (curr_token->type) {
			case TOKEN_KEYWORD_EXTENSION:
				status = parse_extension(curr_token);
				CHECK_STATUS(status);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append extension node into AST Tree\n");
				break;
			case TOKEN_AT_MAIN:
				status = parse_at_main(curr_token);
				CHECK_STATUS(status);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append function node into AST Tree\n");
				break;
			case TOKEN_KEYWORD_DEF:
				status = parse_func_def(curr_token, false);
				CHECK_STATUS(status);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append function node into AST Tree\n");
				break;
			case TOKEN_EOL:
				break; // consumes empty lines
			default:
				ast_destroy(program);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing EOL after header declaration\n"),
					PAR_FAIL);
		}
		GET_TOKEN(curr_token, (Error){0}, program);
	}
	return PARSE_OK(PAR_EOF, program);
}

ParserStatus parse(void) {
	Token *curr_token = malloc(sizeof(Token));
	if (curr_token == NULL) {
		return PARSE_ERROR(
			ERROR_EMIT(ERR_INTERNAL, 0, 0, "Failed to allocate parser token\n"), AST_FAIL);
	}
	lexer_init(stdin);
	ParserStatus status = parse_header(curr_token);
	if (status.flag == PAR_EOF) {
		free(curr_token);
		return status;
	} else if (status.flag == LEX_FAIL) {
		free(curr_token);
		return status;
	} else if (status.flag == PAR_FAIL) {
		free(curr_token);
		return status;
	} else if (status.flag == AST_FAIL) {
		free(curr_token);
		return status;
	}
	status = parse_program(curr_token);
	free(curr_token);
	return status;
}
