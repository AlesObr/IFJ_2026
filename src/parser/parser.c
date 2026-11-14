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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declarations of functions with cyclic dependency
ParserStatus parse_param_mem(Token *curr_token);
ParserStatus parse_func_def(Token *curr_token, bool main_flag);
ParserStatus parse_if_statement(Token *curr_token);
ParserStatus parse_statement(Token *curr_token);
ParserStatus parse_code_block(Token *curr_token, bool indent);
ParserStatus parse_func_body(Token *curr_token, bool indent);

static void parser_trace_token(const char *location, const Token *token) {
	const char *value = "";
	if (token->type == TOKEN_IDENTIFIER || token->type == TOKEN_LIT_STRING) {
		value = token->data.str_val ? dstring_get(token->data.str_val) : "<null>";
	}
	fprintf(stderr, "[parser] %s: %s at %zu:%zu", location, token_type_name(token->type),
		token->line, token->col);
	if (*value)
		fprintf(stderr, " value=\"%s\"", value);
	fputc('\n', stderr);
}

/**
 * @brief Macro for safely fetching from scanner
 * @param tok_ptr Token pointer to fetch into
 * @param node Current rule ASTNode to be destroyed on fail or sent back on EOF
 * @param eof_poss Signalizes if EOF is possible in current fetch
 */
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
		parser_trace_token(__func__, (tok_ptr));                                                   \
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
 * @brief Handling of cases when last function returns PAR_EOF, but the valid node is not yet
 * appended
 * @param status Return value of previous function containing the unconnected node
 * @param node Node to appended the last node into
 * @param apennd_expr Appending expression
 * @param err_msg Error messgae to be displayed if the append fails
 */
#define APPEND_BEFORE_EOF_RETURN(status, node, append_expr, err_msg)                               \
	do {                                                                                           \
		if ((status).flag == PAR_EOF) {                                                            \
			if ((status).new_node != NULL) {                                                       \
				CHECK_AST_APPEND(node, append_expr, err_msg);                                      \
				return PARSE_OK(PAR_EOF, node);                                                    \
			}                                                                                      \
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
 * @brief Parsing of mandatory program header
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_header(Token *curr_token) {
	GET_TOKEN(curr_token, NULL, false);
	while (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, NULL, false);
	}
	// Fetched "import"
	if (curr_token->type != TOKEN_KEYWORD_IMPORT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing mandatory import header (import ifj.*)\n"),
			PAR_FAIL);
	}

	// Expected "ifj26"
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_IDENTIFIER ||
		dstring_cmp_str(curr_token->data.str_val, "ifj26") != 0) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}

	// Expected "."
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_DOT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing ifj type in import (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, NULL, false);
	// Expected "*"
	if (curr_token->type != TOKEN_MUL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_EOL) { // Expected EOL at the end of the declaration
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
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, NULL, false);
		while (curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, NULL, false);
		}
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
 * @param indent Flag handling special cases when INDENT was consumed somewhere before
 * @return Status structure containing flags for caller
 */
ParserStatus parse_code_block(Token *curr_token, bool indent) {
	ParserStatus status = {0};
	ASTNode *new_block;
	while (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, NULL, false);
	}
	if (!indent) {
		if (curr_token->type != TOKEN_INDENT) {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Expected start of indented block, got something else\n"),
				PAR_FAIL);
		}
		GET_TOKEN(curr_token, NULL, false);
	}
	ALLOC_NODE(new_block, ast_create_block(curr_token->line, curr_token->col));
	while (curr_token->type != TOKEN_OUTDENT) {
		if (curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, new_block, false);
			continue;
		}
		status = parse_statement(curr_token);
		APPEND_BEFORE_EOF_RETURN(status, new_block, ast_block_add_stmt(new_block, status.new_node),
			"Failed to append statement into code block after returning from EOF\n");
		CHECK_STATUS_OWNED(status, new_block);
		if (status.new_node != NULL) {
			CHECK_AST_APPEND(new_block, ast_block_add_stmt(new_block, status.new_node),
				"Failed to append statement into block in multi line function body\n");
		}
		if (curr_token->type != TOKEN_SEMICOLON && curr_token->type != TOKEN_EOL &&
			curr_token->type != TOKEN_OUTDENT) {
			ast_destroy(new_block);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid TOKEN at the end of statement\n"),
				PAR_FAIL);
		}
		if (curr_token->type == TOKEN_OUTDENT) {
			GET_TOKEN(curr_token, new_block, true);
			continue;
		}
		if (curr_token->type == TOKEN_SEMICOLON) {
			GET_TOKEN(curr_token, new_block, true);
		}
		if (curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, new_block, true);
		}
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
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier name after function declaration\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(new_decl, ast_create_var_decl(curr_token->data.str_val, mutability, curr_token->line,
							 curr_token->col));
	GET_TOKEN(curr_token, new_decl, false);
	if (curr_token->type == TOKEN_COLON) {
		bool is_orig;
		ValType type;
		dstring_t user_type;
		do {
			is_orig = true;
			type = UNDEF;
			GET_TOKEN(curr_token, new_decl, false);
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
				case TOKEN_IDENTIFIER:
					is_orig = false;
					if (!dstring_init(&user_type) ||
						!dstring_copy(curr_token->data.str_val, &user_type)) {
						ast_destroy(new_decl);
						return PARSE_ERROR(
							ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
								"Failed to copy dstring for type unification identification in "
								"func declaration\n"),
							PAR_FAIL);
					}
					break;
				default:
					ast_destroy(new_decl);
					return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
										   "Missing data type in function parameter\n"),
						PAR_FAIL);
			}
			CHECK_AST_APPEND(new_decl, ast_add_type(new_decl, type, &user_type, is_orig),
				"Failed to append data type into function declaration\n");
			GET_TOKEN(curr_token, new_decl, false);
		} while (curr_token->type == TOKEN_PIPE);
	}
	if (curr_token->type == TOKEN_ASSIGN) {
		GET_TOKEN(curr_token, new_decl, false);
		ParserStatus status = parse_expression(curr_token);
		CHECK_STATUS_OWNED(status, new_decl);
		CHECK_AST_APPEND(new_decl, ast_var_decl_add_init(new_decl, status.new_node),
			"Failed to append init expression to variable declaration\n");
		if (curr_token->type != TOKEN_EOL && curr_token->type != TOKEN_SEMICOLON) {
			ast_destroy(new_decl);
			return PARSE_ERROR(
				ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
					"Invalid token after declaration expression (expected semicolon or EOL)\n"),
				PAR_FAIL);
		}

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
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type == TOKEN_EOL) {
		ALLOC_NODE(new_else, ast_create_else(NULL, curr_token->line, curr_token->col));
		GET_TOKEN(curr_token, new_else, false);
		status = parse_code_block(curr_token, false);
		CHECK_STATUS_OWNED(status, new_else);
		CHECK_AST_APPEND(new_else, ast_if_else_add_body(new_else, status.new_node),
			"Failed to append function body of else block (no if)\n");
		return PARSE_OK(PAR_SUCC, new_else);
	} else if (curr_token->type == TOKEN_KEYWORD_IF) {
		GET_TOKEN(curr_token, NULL, false);
		status = parse_expression(curr_token);
		CHECK_STATUS(status);
		ALLOC_NODE(new_else, ast_create_else(status.new_node, curr_token->line, curr_token->col));
		if (curr_token->type != TOKEN_KEYWORD_THEN) {
			ast_destroy(new_else);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing then after else statement with condition\n"),
				PAR_FAIL);
		}
		GET_TOKEN(curr_token, new_else, false);
		if (curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, new_else, false);
			status = parse_code_block(curr_token, false);
			CHECK_STATUS_OWNED(status, new_else);
			CHECK_AST_APPEND(new_else, ast_if_else_add_body(new_else, status.new_node),
				"Failed to append function body of else block (if)\n");
			GET_TOKEN(curr_token, new_else, false);
			return PARSE_OK(PAR_SUCC, new_else);
		} else {
			status = parse_expression(curr_token);
			CHECK_STATUS_OWNED(status, new_else);
			CHECK_AST_APPEND(new_else, ast_if_else_add_body(new_else, status.new_node),
				"Failed to append function body of else block (if)\n");
			return PARSE_OK(PAR_SUCC, new_else);
		}
	} else {
		ALLOC_NODE(new_else, ast_create_else(NULL, curr_token->line, curr_token->col));
		status = parse_expression(curr_token);
		CHECK_STATUS_OWNED(status, new_else);
		CHECK_AST_APPEND(new_else, ast_if_else_add_body(new_else, status.new_node),
			"Failed to append function body of else block (no if oneline)\n");
		return PARSE_OK(PAR_SUCC, new_else);
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
	GET_TOKEN(curr_token, NULL, false);
	status = parse_expression(curr_token);
	CHECK_STATUS(status);
	ALLOC_NODE(new_if, ast_create_if(status.new_node, curr_token->line, curr_token->col));
	if (curr_token->type != TOKEN_KEYWORD_THEN) {
		ast_destroy(new_if);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing then after if statement condition\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, new_if, false);
	if (curr_token->type != TOKEN_EOL) {
		status = parse_expression(curr_token);
		CHECK_STATUS_OWNED(status, new_if);
		CHECK_AST_APPEND(new_if, ast_if_else_add_body(new_if, status.new_node),
			"Failed to append function body of if block\n");
	} else {
		GET_TOKEN(curr_token, new_if, false);
		status = parse_code_block(curr_token, false);
		CHECK_STATUS_OWNED(status, new_if);
		CHECK_AST_APPEND(new_if, ast_if_else_add_body(new_if, status.new_node),
			"Failed to append function body of if block\n");
		GET_TOKEN(curr_token, new_if, false);
	}
	while (true)
		if (curr_token->type == TOKEN_KEYWORD_ELSE) {
			status = parse_else_statement(curr_token);
			CHECK_STATUS_OWNED(status, new_if);
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
	GET_TOKEN(curr_token, NULL, false);
	status = parse_expression(curr_token);
	CHECK_STATUS(status);
	if (curr_token->type != TOKEN_KEYWORD_DO) {
		ast_destroy(status.new_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected do after condition, got other token\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(new_while, ast_create_while(status.new_node, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, new_while, false);
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after do of while statement\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, new_while, false);
	status = parse_code_block(curr_token, false);
	CHECK_STATUS_OWNED(status, new_while);
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
	CHECK_STATUS_OWNED(status, for_node);
	CHECK_AST_APPEND(for_node, ast_for_add_start(for_node, status.new_node),
		"Failed to append generator expressiong to for AST node\n");
	if (curr_token->type == TOKEN_IDENTIFIER &&
		dstring_cmp_str(curr_token->data.str_val, "to") == 0) {
		GET_TOKEN(curr_token, for_node, false);
		status = parse_expression(curr_token);
		CHECK_STATUS_OWNED(status, for_node);
		CHECK_AST_APPEND(for_node, ast_for_add_end(for_node, status.new_node, true),
			"Failed to append generator expressiong to for AST node\n");
	} else if (curr_token->type == TOKEN_IDENTIFIER &&
			   dstring_cmp_str(curr_token->data.str_val, "until") == 0) {
		GET_TOKEN(curr_token, for_node, false);
		status = parse_expression(curr_token);
		CHECK_STATUS_OWNED(status, for_node);
		CHECK_AST_APPEND(for_node, ast_for_add_end(for_node, status.new_node, false),
			"Failed to append generator expressiong to for AST node\n");
	} else {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token inside of for statement generator\n"),
			PAR_FAIL);
	}
	if (curr_token->type == TOKEN_IDENTIFIER &&
		dstring_cmp_str(curr_token->data.str_val, "by") == 0) {
		GET_TOKEN(curr_token, for_node, false);
		status = parse_expression(curr_token);
		CHECK_STATUS_OWNED(status, for_node);
		CHECK_AST_APPEND(for_node, ast_for_add_step(for_node, status.new_node),
			"Failed to append generator expressiong to for AST node\n");
	}
	if (curr_token->type == TOKEN_EOL || curr_token->type == TOKEN_KEYWORD_DO ||
		curr_token->type == TOKEN_KEYWORD_IF) {
		return PARSE_OK(PAR_SUCC, NULL);
	} else {
		ast_destroy(for_node);
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
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, NULL, false);
		while (curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, NULL, false);
		}
		if (curr_token->type == TOKEN_INDENT) {
			GET_TOKEN(curr_token, NULL, false);
		} else {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing generator after for statement\n"),
				PAR_FAIL);
		}
	}
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing generator after for statement\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(
		for_node, ast_create_for(curr_token->data.str_val, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, for_node, false);
	if (curr_token->type != TOKEN_LT) {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing < after for statement generator\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, for_node, false);
	if (curr_token->type != TOKEN_MINUS) {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing - after < in for statement generator\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, for_node, false);
	// parse_for_generator releases for_node itself on failure
	status = parse_for_generator(curr_token, for_node);
	CHECK_STATUS(status);
	if (curr_token->type == TOKEN_EOL) {
		while (true) {
			GET_TOKEN(curr_token, for_node, false);
			while (curr_token->type == TOKEN_EOL) {
				GET_TOKEN(curr_token, for_node, false);
			}
			if (curr_token->type == TOKEN_KEYWORD_IF) {
				GET_TOKEN(curr_token, for_node, false);
				status = parse_expression(curr_token);
				CHECK_STATUS_OWNED(status, for_node);
				CHECK_AST_APPEND(for_node, ast_for_append_guard(for_node, status.new_node),
					"Failed to append guard to for loop\n");
				if (curr_token->type != TOKEN_EOL) {
					ast_destroy(for_node);
					return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
										   "Invalid token at the end of for loop guard\n"),
						PAR_FAIL);
				}
			}
			while (curr_token->type == TOKEN_EOL) {
				GET_TOKEN(curr_token, for_node, false);
			}
			if (curr_token->type == TOKEN_KEYWORD_DO || curr_token->type == TOKEN_OUTDENT) {
				break;
			}
		}
	}
	if (curr_token->type == TOKEN_OUTDENT) {
		GET_TOKEN(curr_token, for_node, true);
	}
	if (curr_token->type != TOKEN_KEYWORD_DO) {
		ast_destroy(for_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected 'do' at the end of for statement, got different token\n"),
			PAR_FAIL);
	}
	status = parse_func_body(curr_token, false);
	CHECK_STATUS_OWNED(status, for_node);
	CHECK_AST_APPEND(for_node, ast_for_add_body(for_node, status.new_node),
		"Failed to append body of for loop\n");
	return PARSE_OK(PAR_SUCC, for_node);
}

/**
 * @brief Parsing of match case (ALGEBDT expansion)
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_match_case(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *case_node = NULL;
	ASTNode *expr_node = NULL;
	GET_TOKEN(curr_token, NULL, false);
	switch (curr_token->type) {
		case TOKEN_KEYWORD_TRUE:
			ALLOC_NODE(expr_node,
				ast_create_bool(true, curr_token->line, curr_token->col));
			GET_TOKEN(curr_token, expr_node, false);
			break;

		case TOKEN_KEYWORD_FALSE:
			ALLOC_NODE(expr_node,
				ast_create_bool(false, curr_token->line, curr_token->col));
			GET_TOKEN(curr_token, expr_node, false);
			break;

		case TOKEN_LIT_FLOAT:
			ALLOC_NODE(expr_node,
				ast_create_float(curr_token->data.float_val, curr_token->line, curr_token->col));
			GET_TOKEN(curr_token, expr_node, false);
			break;
		case TOKEN_LIT_INT:
			ALLOC_NODE(expr_node,
				ast_create_int(curr_token->data.int_val, curr_token->line, curr_token->col));
			GET_TOKEN(curr_token, expr_node, false);
			break;
		case TOKEN_LIT_STRING:
			ALLOC_NODE(expr_node,
				ast_create_string(curr_token->data.str_val, curr_token->line, curr_token->col));
			GET_TOKEN(curr_token, expr_node, false);
			break;
		case TOKEN_IDENTIFIER:
			status = parse_expression(curr_token);
			CHECK_STATUS(status);
			expr_node = status.new_node;
			break;
		case TOKEN_UNDERSCORE:
			ALLOC_NODE(expr_node, ast_create_null(curr_token->line, curr_token->col));
			GET_TOKEN(curr_token, expr_node, false);
			break;
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid token type after case keyword\n"),
				PAR_FAIL);
	}
	ALLOC_NODE(case_node, ast_create_case_node(expr_node, curr_token->line, curr_token->col));
	if (curr_token->type == TOKEN_COLON) {
		GET_TOKEN(curr_token, case_node, false);
		bool is_orig = true;
		ValType type = UNDEF;
		dstring_t user_type;
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
			case TOKEN_IDENTIFIER:
				is_orig = false;
				if (!dstring_init(&user_type) ||
					!dstring_copy(curr_token->data.str_val, &user_type)) {
					ast_destroy(case_node);
					return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
										   "Failed to copy dstring for case in match section\n"),
						PAR_FAIL);
				}
				break;
			default:
				ast_destroy(case_node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing data type in function parameter\n"),
					PAR_FAIL);
		}
		CHECK_AST_APPEND(case_node, ast_add_type(case_node, type, &user_type, is_orig),
			"Failed to addpened data type into case\n");
		GET_TOKEN(curr_token, case_node, false);
	}
	if (curr_token->type == TOKEN_KEYWORD_IF) {
		GET_TOKEN(curr_token, case_node, false);
		status = parse_expression(curr_token);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(case_node, ast_case_add_guard(case_node, status.new_node),
			"Failed to append guard into case node\n");
	}
	if (curr_token->type != TOKEN_ARROW) {
		ast_destroy(case_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected = after case argument / guard, got different token\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, case_node, false);
	while (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, case_node, false);
	}
	if (curr_token->type == TOKEN_INDENT) {
		status = parse_code_block(curr_token, false);
		GET_TOKEN(curr_token, case_node, false);
	} else {
		status = parse_expression(curr_token);
	}
	CHECK_STATUS(status);
	CHECK_AST_APPEND(case_node, ast_case_add_body(case_node, status.new_node),
		"Failed to append expression to be executed if case is matched\n");
	while (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, case_node, true);
	}
	return PARSE_OK(PAR_SUCC, case_node);
}

/**
 * @brief Parsing of match body (ALGEBDT expansion)
 * @param curr_token Token pointer filled with current token
 * @param match_expr Expression the match is matching
 * @return Status structure containing flags for caller
 */
ParserStatus parse_match(Token *curr_token, ASTNode *match_expr) {
	ParserStatus status = {0};
	ASTNode *match_node = NULL;
	ALLOC_NODE(match_node, ast_create_match_node(match_expr, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, match_node, false);
	if (curr_token->type != TOKEN_EOL) {
		ast_destroy(match_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected EOL after match keyword, got something else\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, match_node, false);
	while (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, match_node, false);
	}
	if (curr_token->type != TOKEN_INDENT) {
		ast_destroy(match_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected INDENT inside match body, got something else\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, match_node, false);
	bool case_found = false;
	while (curr_token->type == TOKEN_KEYWORD_CASE) {
		case_found = true;
		status = parse_match_case(curr_token);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(match_node, ast_match_add_case(match_node, status.new_node),
			"Failed to append case node into match node");
	}
	if (!case_found || curr_token->type != TOKEN_OUTDENT) {
		ast_destroy(match_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Failed to find mandatory case inside match block\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, match_node);
}

/**
 * @brief Parsing of statements
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_statement(Token *curr_token) {
	ParserStatus status = {0};
	ASTNode *node = NULL;
	if (curr_token->type == TOKEN_EOL) {
		return PARSE_OK(PAR_SUCC, NULL);
	}
	switch (curr_token->type) {
		case TOKEN_KEYWORD_VAL:
			status = parse_declaration(curr_token, true);
			break;
		case TOKEN_KEYWORD_VAR:
			status = parse_declaration(curr_token, false);
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
			ALLOC_NODE(node, ast_create_return(curr_token->line, curr_token->col));
			GET_TOKEN(curr_token, node, true);
			if (curr_token->type != TOKEN_EOL && curr_token->type != TOKEN_SEMICOLON) {
				status = parse_expression(curr_token);
				CHECK_AST_APPEND(node, ast_return_add_expr(node, status.new_node), "Failed to add return expression to node\n");
				CHECK_STATUS_OWNED(status, node);
				if (curr_token->type != TOKEN_EOL && curr_token->type != TOKEN_SEMICOLON) {
					ast_destroy(status.new_node);
					return PARSE_ERROR(
						ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							"Invalid token after return (expected semicolon or EOL\n"),
						PAR_FAIL);
				}
			}
			break;
		case TOKEN_KEYWORD_TRUE:
		case TOKEN_KEYWORD_FALSE:
		case TOKEN_IDENTIFIER:
		case TOKEN_LIT_INT:
		case TOKEN_LIT_FLOAT:
		case TOKEN_LIT_STRING:
		case TOKEN_LPAREN:
		case TOKEN_MINUS:
		case TOKEN_NOT:
			status = parse_expression(curr_token);
			CHECK_STATUS(status);
			if (curr_token->type == TOKEN_KEYWORD_MATCH) {
				status = parse_match(curr_token, status.new_node);
			}
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
 * @param indent Flag handling special cases when INDENT was consumed somewhere before
 * @return Status structure containing flags for caller
 */
ParserStatus parse_func_body(Token *curr_token, bool indent) {
	ParserStatus status = {0};
	ASTNode *new_block;
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_EOL) {
		ALLOC_NODE(new_block, ast_create_block(curr_token->line, curr_token->col));
		status = parse_statement(curr_token);
		APPEND_BEFORE_EOF_RETURN(status, new_block, ast_block_add_stmt(new_block, status.new_node),
			"Failed to append statement into function body code block (one line) after returning "
			"from EOF\n");
		CHECK_STATUS_OWNED(status, new_block);
		CHECK_AST_APPEND(new_block, ast_block_add_stmt(new_block, status.new_node),
			"Failed to append statement into block in one line function body\n");
	} else {
		while (curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, NULL, false);
		}
		if (curr_token->type == TOKEN_INDENT || (curr_token->type != TOKEN_EOL && indent)) {
			status = parse_code_block(curr_token, indent);
			CHECK_STATUS(status);
			new_block = status.new_node;
		} else if (curr_token->type != TOKEN_EOL) {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid Token after function declaration header\n"),
				PAR_FAIL);
		}
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
	GET_TOKEN(curr_token, param_window, false);
	ASTNode *param_node;
	while (curr_token->type == TOKEN_EOL || curr_token->type == TOKEN_INDENT) {
		GET_TOKEN(curr_token, param_window, false);
	}
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
		GET_TOKEN(curr_token, param_window, false);
		if (curr_token->type != TOKEN_COLON) {
			ast_destroy(param_window);
			return PARSE_ERROR(
				ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
					"Missing colon after identifier of parameter in function declaration\n"),
				PAR_FAIL);
		}
		bool is_orig;
		ValType param_type;
		dstring_t user_type;
		do {
			is_orig = true;
			param_type = UNDEF;
			GET_TOKEN(curr_token, param_window, false);
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
				case TOKEN_IDENTIFIER:
					is_orig = false;
					if (!dstring_init(&user_type) ||
						!dstring_copy(curr_token->data.str_val, &user_type)) {
						ast_destroy(param_node);
						return PARSE_ERROR(
							ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
								"Failed to copy dstring for type unification identification in "
								"parameter section\n"),
							PAR_FAIL);
					}
					break;
				default:
					ast_destroy(param_window);
					return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
										   "Missing data type in function parameter\n"),
						PAR_FAIL);
			}
			CHECK_AST_APPEND(param_node, ast_add_type(param_node, param_type, &user_type, is_orig),
				"Failed to append data type into function parameter node\n");
			GET_TOKEN(curr_token, param_window, false);
		} while (curr_token->type == TOKEN_COLON);
		if (curr_token->type == TOKEN_COMMA) {
			GET_TOKEN(curr_token, param_window, false);
			while (curr_token->type == TOKEN_EOL || curr_token->type == TOKEN_OUTDENT) {
				GET_TOKEN(curr_token, param_window, false);
			}
			if (curr_token->type == TOKEN_RPAREN) {
				return PARSE_OK(PAR_SUCC, param_window);
			} else if (curr_token->type == TOKEN_IDENTIFIER) {
				continue;
			} else {
				ast_destroy(param_window);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid token after , in function declaration params\n"),
					PAR_FAIL);
			}
		} else if (curr_token->type == TOKEN_RPAREN){
			return PARSE_OK(PAR_SUCC, param_window);
		} else {
			ast_destroy(param_window);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									"Invalid token after , in function declaration params\n"),
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
	bool found_indent = false;
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier in function declaration\n"),
			PAR_FAIL);
	}
	ASTNode *new_func;
	ALLOC_NODE(new_func,
		ast_create_func(curr_token->data.str_val, main_flag, curr_token->line, curr_token->col));
	while (true) {
		GET_TOKEN(curr_token, new_func, false);
		if (curr_token->type == TOKEN_LPAREN) {
			status = parse_param_section(curr_token);
			CHECK_STATUS_OWNED(status, new_func);
			CHECK_AST_APPEND(new_func, ast_func_add_param_window(new_func, status.new_node),
				"Failed to add function parameter window into AST\n");
		} else {
			break;
		}
	}
	if (curr_token->type == TOKEN_INDENT) {
		found_indent = true;
		GET_TOKEN(curr_token, new_func, false);
	}
	if (curr_token->type == TOKEN_COLON) {
		bool is_orig;
		ValType type;
		dstring_t user_type;
		do {
			is_orig = true;
			type = UNDEF;
			GET_TOKEN(curr_token, new_func, false);
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
				case TOKEN_IDENTIFIER:
					is_orig = false;
					if (!dstring_init(&user_type) ||
						!dstring_copy(curr_token->data.str_val, &user_type)) {
						ast_destroy(new_func);
						return PARSE_ERROR(
							ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
								"Failed to copy dstring for type unification identification in "
								"function declaration\n"),
							PAR_FAIL);
					}
					break;
				default:
					ast_destroy(new_func);
					return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
										   "Invalid function return type\n"),
						PAR_FAIL);
			}
			CHECK_AST_APPEND(new_func, ast_add_type(new_func, type, &user_type, is_orig),
				"Failed to append data type into function declaration node\n");
			GET_TOKEN(curr_token, new_func, false);
		} while (curr_token->type == TOKEN_PIPE);
	}
	if (curr_token->type == TOKEN_INDENT) {
		found_indent = true;
		GET_TOKEN(curr_token, new_func, false);
	}
	if (curr_token->type == TOKEN_ASSIGN) {
		status = parse_func_body(curr_token, found_indent);
		APPEND_BEFORE_EOF_RETURN(status, new_func, ast_func_add_block(new_func, status.new_node),
			"Failed to append function body into AST after returning from EOF\n");
		CHECK_STATUS_OWNED(status, new_func);
		CHECK_AST_APPEND(new_func, ast_func_add_block(new_func, status.new_node),
			"Failed to append function body into AST\n");
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
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier in extension parameter\n"),
			PAR_FAIL);
	}
	ASTNode *new_node;
	ALLOC_NODE(
		new_node, ast_ext_create(curr_token->data.str_val, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, new_node, false);
	if (curr_token->type != TOKEN_COLON) {
		ast_destroy(new_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing colon in extension parameter\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, new_node, false);
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
	ast_add_type(new_node, type, NULL, true);
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
	GET_TOKEN(curr_token, ext_node, false);
	if (curr_token->type == TOKEN_AT_MAIN) {
		status = parse_at_main(curr_token);
		CHECK_STATUS_OWNED(status, ext_node);
	} else if (curr_token->type == TOKEN_KEYWORD_DEF) {
		status = parse_func_def(curr_token, false);
		CHECK_STATUS_OWNED(status, ext_node);
	} else {
		ast_destroy(ext_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token in function declaration inside extension\n"),
			PAR_FAIL);
	}
	CHECK_AST_APPEND(ext_node, ast_ext_add_func(ext_node, status.new_node),
		"Failed to add function into extension\n");
	GET_TOKEN(curr_token, ext_node, false);
	while (curr_token->type != TOKEN_OUTDENT) {
		switch (curr_token->type) {
			case TOKEN_AT_MAIN:
				status = parse_at_main(curr_token);
				CHECK_STATUS_OWNED(status, ext_node);
				CHECK_AST_APPEND(ext_node, ast_ext_add_func(ext_node, status.new_node),
					"Failed to add function into extension\n");
				break;
			case TOKEN_KEYWORD_DEF:
				status = parse_func_def(curr_token, false);
				CHECK_STATUS_OWNED(status, ext_node);
				CHECK_AST_APPEND(ext_node, ast_ext_add_func(ext_node, status.new_node),
					"Failed to add function into extension\n");
				break;
			case TOKEN_EOL:
				break;
			default:
				ast_destroy(ext_node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid token at end of extension declaration\n"),
					PAR_FAIL);
		}
		GET_TOKEN(curr_token, ext_node, false);
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
	GET_TOKEN(curr_token, NULL, false); // Consume left parenthesis
	if (curr_token->type != TOKEN_LPAREN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing left bracket of extension parameter\n"),
			PAR_FAIL);
	}
	status = parse_extension_param(curr_token); // Parse the type the extension extends
	CHECK_STATUS(status);
	ext_header = status.new_node; // Extension header node was allocated in parse_extesion_param
	GET_TOKEN(curr_token, ext_header, false);
	if (curr_token->type !=
		TOKEN_RPAREN) { // Parse_extension param should stop at right parenthesis
		ast_destroy(ext_header);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing right bracket of extension parameter\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, ext_header, false);
	while (curr_token->type == TOKEN_EOL) { // Consume EOL after right parenthesis and any EOLs
											// between header and body of extension
		GET_TOKEN(curr_token, ext_header, false);
		if (curr_token->type == TOKEN_INDENT) {
			status = parse_extension_body(curr_token, ext_header);
			CHECK_STATUS(status);
			break;
		} else if (curr_token->type != TOKEN_EOL) {
			ast_destroy(ext_header);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing EOL after extension parameter bracket\n"),
				PAR_FAIL);
		}
	}
	if (curr_token->type != TOKEN_OUTDENT) {
		ast_destroy(ext_header);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing outdent at the end of extension\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC, ext_header);
}

/**
 * @brief Parsing of type unification
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_type_unif(Token *curr_token) {
	ASTNode *type_node;
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing new type name after type keyword\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(type_node,
		ast_create_type_unif(curr_token->data.str_val, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, type_node, false);
	if (curr_token->type != TOKEN_ASSIGN) {
		ast_destroy(type_node);
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing = after type identifier\n"),
			PAR_FAIL);
	}
	bool is_orig;
	ValType type;
	dstring_t user_type;
	do {
		is_orig = true;
		type = UNDEF;
		GET_TOKEN(curr_token, type_node, false);
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
			case TOKEN_IDENTIFIER:
				is_orig = false;
				if (!dstring_init(&user_type) ||
					!dstring_copy(curr_token->data.str_val, &user_type)) {
					ast_destroy(type_node);
					return PARSE_ERROR(ERROR_EMIT(ERR_INTERNAL, curr_token->line, curr_token->col,
										   "Failed to copy dstring for type unification "
										   "identification in type unification alias\n"),
						PAR_FAIL);
				}
				break;
			default:
				ast_destroy(type_node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing data type after = in type unification\n"),
					PAR_FAIL);
		}
		CHECK_AST_APPEND(type_node, ast_add_type(type_node, type, &user_type, is_orig),
			"Failed to addpened data type into type unification\n");
		GET_TOKEN(curr_token, type_node, false);
	} while (curr_token->type == TOKEN_PIPE);
	return PARSE_OK(PAR_SUCC, type_node);
}

/**
 * @brief Parsing of data class
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_data_class(Token *curr_token) {
	ASTNode *class_node;
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_KEYWORD_CLASS) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing class keyword after case keyword on top level\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, NULL, false);
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier of new data class on top level\n"),
			PAR_FAIL);
	}
	ALLOC_NODE(class_node,
		ast_create_data_class(curr_token->data.str_val, curr_token->line, curr_token->col));
	GET_TOKEN(curr_token, class_node, false);
	ParserStatus status = parse_param_section(curr_token);
	CHECK_STATUS(status);
	CHECK_AST_APPEND(class_node, ast_class_add_param(class_node, status.new_node),
		"Failed to append function parameter window into data class\n");
	GET_TOKEN(curr_token, class_node, true);
	return PARSE_OK(PAR_SUCC, class_node);
}

/**
 * @brief Parsing of highest level program constructs (functions, extensions, type, class)
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_program(Token *curr_token) {
	ParserStatus status = {0};
	GET_TOKEN(curr_token, NULL, true); // Fetch first body token
	ASTNode *program = NULL;
	ALLOC_NODE(program, ast_create_block(curr_token->line, curr_token->col));
	while (curr_token->type != TOKEN_EOF) {
		switch (curr_token->type) {
			case TOKEN_KEYWORD_EXTENSION:
				status = parse_extension(curr_token); // Parse extension
				APPEND_BEFORE_EOF_RETURN(status, program,
					ast_block_add_stmt(program, status.new_node),
					"Failed to append node into AST after returning from EOF\n");
				CHECK_STATUS_OWNED(status, program);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append extension node into AST Tree\n");
				break;
			case TOKEN_KEYWORD_TYPE:
				status = parse_type_unif(curr_token);
				APPEND_BEFORE_EOF_RETURN(status, program,
					ast_block_add_stmt(program, status.new_node),
					"Failed to append node into AST after returning from EOF\n");
				CHECK_STATUS_OWNED(status, program);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append type unification into AST Tree\n");
				break;
			case TOKEN_KEYWORD_CASE:
				status = parse_data_class(curr_token);
				APPEND_BEFORE_EOF_RETURN(status, program,
					ast_block_add_stmt(program, status.new_node),
					"Failed to append node into AST after returning from EOF\n");
				CHECK_STATUS_OWNED(status, program);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append data class node into AST Tree\n");
				break;
			case TOKEN_AT_MAIN:
				status = parse_at_main(curr_token); // Parse function with @main flag
				APPEND_BEFORE_EOF_RETURN(status, program,
					ast_block_add_stmt(program, status.new_node),
					"Failed to append node into AST after returning from EOF\n");
				CHECK_STATUS_OWNED(status, program);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append function node into AST Tree\n");
				break;
			case TOKEN_KEYWORD_DEF:
				status = parse_func_def(curr_token, false); // Parse function without @main flag
				APPEND_BEFORE_EOF_RETURN(status, program,
					ast_block_add_stmt(program, status.new_node),
					"Failed to append node into AST after returning from EOF\n");
				CHECK_STATUS_OWNED(status, program);
				CHECK_AST_APPEND(program, ast_block_add_stmt(program, status.new_node),
					"Failed to append function node into AST Tree\n");
				break;
			case TOKEN_EOL:
				break; // consumes empty lines
			default:
				ast_destroy(program);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid token at top level\n"),
					PAR_FAIL);
		}
		if (curr_token->type == TOKEN_OUTDENT || curr_token->type == TOKEN_EOL) {
			GET_TOKEN(curr_token, program, true);
		} else {
			ast_destroy(program);
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid token after returning control to top level parsing\n"),
				PAR_FAIL);
		}
	}
	return PARSE_OK(PAR_EOF, program);
}

ParserStatus parse() {
	fprintf(stderr, "[parser] parse: start\n");
	Token *curr_token = malloc(sizeof(Token));
	if (curr_token == NULL) {
		return PARSE_ERROR(
			ERROR_EMIT(ERR_INTERNAL, 0, 0, "Failed to allocate parser token\n"), AST_FAIL);
	}
	token_init(curr_token); // GET_TOKEN destroys the previous token first
	lexer_init(stdin);
	ParserStatus status = parse_header(curr_token);
	if (status.flag == PAR_EOF) {
		token_destroy(curr_token);
		free(curr_token);
		lexer_free();
		return status;
	} else if (status.flag == LEX_FAIL) {
		token_destroy(curr_token);
		free(curr_token);
		lexer_free();
		return status;
	} else if (status.flag == PAR_FAIL) {
		token_destroy(curr_token);
		free(curr_token);
		lexer_free();
		return status;
	} else if (status.flag == AST_FAIL) {
		token_destroy(curr_token);
		free(curr_token);
		lexer_free();
		return status;
	}
	status = parse_program(curr_token);
	fprintf(stderr, "[parser] parse: finished with flag=%d error=%d at %zu:%zu", status.flag,
		status.error.type, status.error.line, status.error.col);
	if (status.error.message)
		fprintf(stderr, " message=\"%s\"", status.error.message);
	fputc('\n', stderr);
	token_destroy(curr_token);
	free(curr_token);
	lexer_free();
	return status;
}
