/**
 * @project IFJ26 Compiler
 * @file parser.c
 * @brief Syntactic analyzer and recursive descent interface.
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "parser.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include "../lexer/lexer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

ParserStatus parse_expression(Token *curr_token, ...);
ParserStatus parse_code_block(Token *curr_token);
ParserStatus parse_func_def(Token *curr_token, bool main_flag);
ParserStatus parse_if_statement(Token *curr_token);
ParserStatus parse_while_statement(Token *curr_token);
ParserStatus parse_statement(Token *curr_token);
ParserStatus parse_param_mem(Token *curr_token);
ParserStatus parse_param_tail(Token *curr_token);
ParserStatus parse_param_section(Token *curr_token);
ParserStatus parse_func_body(Token *curr_token);
ParserStatus parse_else_statement(Token *curr_token);
ParserStatus parse_at_main(Token *curr_token);
ParserStatus parse_extension_param(Token *curr_token);
ParserStatus parse_extension_body(Token *curr_token);
ParserStatus parse_extension(Token *curr_token);

/**
 * @brief Macro for safely fetching from scanner
 * @param tok_ptr Token pointer to fetch into
 * @param err Error variable to catch scanner errors
 */
static inline ParserStatus fetch_token(Token *tok_ptr) {
	Error err = lexer_get_next_token(tok_ptr);
	if (ERROR_IS_ERR(err)) {
		return PARSE_ERROR(err, LEX_FAIL);
	}
	if (tok_ptr->type == TOKEN_EOF) {
		return PARSE_OK(PAR_EOF);
	}
	return PARSE_OK(PAR_SUCC);
}

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
 * @brief Parsing of mandatory program header
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_header(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}

	if (curr_token->type != TOKEN_KEYWORD_IMPORT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing mandatory import header (import ifj.*)\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_IDENTIFIER ||
		strcmp(curr_token->data.str_val->data, "ifj") != 0) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_DOT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing ifj type in import (ifj.*)\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_MUL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after header declaration\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing extesion of @main function mark
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_at_main(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_AT_MAIN) {
		status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
		if (curr_token->type == TOKEN_EOL) {
			status = fetch_token(curr_token);
			if (status.flag != PAR_SUCC) {
				return status;
			}
			if (curr_token->type == TOKEN_KEYWORD_DEF) {
				CHECK_STATUS(parse_func_def(curr_token, true));
			} else {
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing definition token after @main\n"),
					PAR_FAIL);
			}
		} else {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing definition token after @main\n"),
				PAR_FAIL);
		}
	} else if (curr_token->type == TOKEN_KEYWORD_DEF) {
		CHECK_STATUS(parse_func_def(curr_token, true));
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing definition token after @main\n"),
			PAR_FAIL);
	}
	PARSE_OK(PAR_SUCC)
}

/**
 * @brief Parsing of declarations
 * @param curr_token Token pointer filled with current token
 * @param mutability Flag whether declared variable is mutable (true for var, false for val)
 * @return Status structure containing flags for caller
 */
ParserStatus parse_declaration(Token *curr_token, bool mutability) {
	(void)mutability;
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier name after function declaration\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_COLON) {
		status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
		switch (curr_token->type) {
			case TOKEN_KEYWORD_INT:
			case TOKEN_KEYWORD_DOUBLE:
			case TOKEN_KEYWORD_UNIT:
			case TOKEN_KEYWORD_STRING:
			case TOKEN_KEYWORD_BOOLEAN:
				break;
			default:
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing data type in function parameter\n"),
					PAR_FAIL);
		}
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_EQ) {
		CHECK_STATUS(parse_expression(curr_token));
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing assignment after variable declaration\n"),
			PAR_FAIL);
	}
	PARSE_OK(PAR_SUCC)
}

/**
 * @brief Parsiong of else decision branch
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_else_statement(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_EOL) {
		return PARSE_OK(PAR_SUCC);
	} else if (curr_token->type == TOKEN_KEYWORD_IF) {
		CHECK_STATUS(parse_if_statement(curr_token));
		return PARSE_OK(PAR_SUCC);
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
	CHECK_STATUS(parse_expression(curr_token, TOKEN_KEYWORD_THEN));
	if (curr_token->type != TOKEN_KEYWORD_THEN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing then after if statement condition\n"),
			PAR_FAIL);
	}
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after then of if statement\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	CHECK_STATUS(parse_code_block(curr_token));
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_KEYWORD_ELSE) {
		CHECK_STATUS(parse_else_statement(curr_token));
	} else {
		return PARSE_OK(PAR_SUCC);
	}
	PARSE_OK(PAR_SUCC)
}

/**
 * @brief Parsing of while statement
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_while_statement(Token *curr_token) {
	CHECK_STATUS(parse_expression(curr_token, TOKEN_KEYWORD_DO));
	if (curr_token->type != TOKEN_KEYWORD_THEN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing do after while statement condition\n"),
			PAR_FAIL);
	}
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after do of while statement\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	CHECK_STATUS(parse_code_block(curr_token));
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing of statements
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_statement(Token *curr_token) {
	switch (curr_token->type) {
		case TOKEN_KEYWORD_VAL:
			CHECK_STATUS(parse_declaration(curr_token, false));
			break;
		case TOKEN_KEYWORD_VAR:
			CHECK_STATUS(parse_declaration(curr_token, true));
			break;
		case TOKEN_KEYWORD_IF:
			CHECK_STATUS(parse_if_statement(curr_token));
			break;
		case TOKEN_KEYWORD_WHILE:
			CHECK_STATUS(parse_while_statement(curr_token));
			break;
		case TOKEN_KEYWORD_FOR:
			CHECK_STATUS(parse_while_statement(curr_token));
			break;
		case TOKEN_KEYWORD_RETURN:
			// add check for next variable
			break;
		case TOKEN_IDENTIFIER:
			// add parsing of expression
			break;
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Invalid token at the start of statement\n"),
				PAR_FAIL);
	}
	PARSE_OK(PAR_SUCC)
}

/**
 * @brief Parsing of inner function body
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_func_body(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	CHECK_STATUS(parse_statement(curr_token));
	if (curr_token->type == TOKEN_EOL) {
		CHECK_STATUS(parse_code_block(curr_token));
	}
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing tail of mutliple function parameters
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_param_tail(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_EOL) {
		status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
		return PARSE_OK(PAR_SUCC);
	} else if (curr_token->type == TOKEN_IDENTIFIER) {
		status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
		CHECK_STATUS(parse_param_mem(curr_token));
		return PARSE_OK(PAR_SUCC);
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token in function parameter tail\n"),
			PAR_FAIL);
	}
}

/**
 * @brief Parsing of param member of function declaration
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_param_mem(Token *curr_token) {
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier of parameter in function declaration\n"),
			PAR_FAIL);
	}
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_COLON) {
		return PARSE_ERROR(
			ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
				"Missing colon after identifier of parameter in function declaration\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	switch (curr_token->type) {
		case TOKEN_KEYWORD_INT:
		case TOKEN_KEYWORD_DOUBLE:
		case TOKEN_KEYWORD_UNIT:
		case TOKEN_KEYWORD_STRING:
		case TOKEN_KEYWORD_BOOLEAN:
			status = fetch_token(curr_token);
			if (status.flag != PAR_SUCC) {
				return status;
			}
			if (curr_token->type == TOKEN_RBRACE) {
				return PARSE_OK(PAR_SUCC);
			}
			break;
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing data type in function parameter\n"),
				PAR_FAIL);
	}
	PARSE_OK(PAR_SUCC)
}

/**
 * @brief Parsing of param sections of function declarations
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_param_section(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_IDENTIFIER) {
		CHECK_STATUS(parse_param_mem(curr_token));
	}
	if (curr_token->type == TOKEN_RPAREN) {
		return PARSE_OK(PAR_SUCC);
	} else {
		return PARSE_ERROR(
			ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
				"Invalid parameter list in function declaration (missing right brace)\n"),
			PAR_FAIL);
	}
}

/**
 * @brief Parsing of function declarations
 * @param curr_token Token pointer filled with current token
 * @param main_flag Indicates whether function is main one (for AST tree)
 * @return Status structure containing flags for caller
 */
ParserStatus parse_func_def(Token *curr_token, bool main_flag) {
	(void)main_flag; // @dev TODO: This is a placeholder, delete it
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier in function declaration\n"),
			PAR_FAIL);
	}
	while (true) {
		ParserStatus status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
		if (curr_token->type == TOKEN_LPAREN) {
			CHECK_STATUS(parse_param_section(curr_token));
		} else {
			break;
		}
	}
	if (curr_token->type == TOKEN_COLON) {
		ParserStatus status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
		switch (curr_token->type) {
			case TOKEN_KEYWORD_INT:
			case TOKEN_KEYWORD_DOUBLE:
			case TOKEN_KEYWORD_UNIT:
			case TOKEN_KEYWORD_STRING:
			case TOKEN_KEYWORD_BOOLEAN:
				break;
			case TOKEN_EQ:
				break;
			default:
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid function return type\n"),
					PAR_FAIL);
		}
	}
	if (curr_token->type == TOKEN_EQ) {
		CHECK_STATUS(parse_func_body(curr_token));
	}
	return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
						   "Unexpected token in function declaration\n"),
		PAR_FAIL);
}

/**
 * @brief Function for parsing extension parameter
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_extension_param(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier in extension parameter\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_COLON) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing colon in extension parameter\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	switch (curr_token->type) {
		case TOKEN_KEYWORD_INT:
		case TOKEN_KEYWORD_DOUBLE:
		case TOKEN_KEYWORD_UNIT:
		case TOKEN_KEYWORD_STRING:
		case TOKEN_KEYWORD_BOOLEAN:
			return PARSE_OK(PAR_SUCC);
			break;
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
								   "Missing data type in extention parameter\n"),
				PAR_FAIL);
	}
}

/**
 * @brief Function for parsing functions defined in extensions
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_extension_body(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type == TOKEN_AT_MAIN) {
		CHECK_STATUS(parse_at_main(curr_token));
	} else if (curr_token->type == TOKEN_KEYWORD_DEF) {
		CHECK_STATUS(parse_func_def(curr_token, false));
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Unexpected token in function declaration inside extension\n"),
			PAR_FAIL);
	}
	while (curr_token->type != TOKEN_OUTDENT) {
		status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
		switch (curr_token->type) {
			case TOKEN_AT_MAIN:
				CHECK_STATUS(parse_at_main(curr_token));
				break;
			case TOKEN_KEYWORD_DEF:
				CHECK_STATUS(parse_func_def(curr_token, false));
				break;
			default:
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Invalid token at end of extension declaration\n"),
					PAR_FAIL);
		}
	}
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Function for parsing extension definition
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_extension(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_LPAREN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing left bracket of extension parameter\n"),
			PAR_FAIL);
	}
	CHECK_STATUS(parse_extension_param(curr_token));
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_RPAREN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing right bracket of extension parameter\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after extension parameter bracket\n"),
			PAR_FAIL);
	}
	status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	if (curr_token->type != TOKEN_INDENT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing left bracket of extension parameter\n"),
			PAR_FAIL);
	}
	CHECK_STATUS(parse_extension_body(curr_token));
	if (curr_token->type != TOKEN_OUTDENT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing outdent at the end of extension\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing of highest level program constructs (functions, extensions)
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_program(Token *curr_token) {
	ParserStatus status = fetch_token(curr_token);
	if (status.flag != PAR_SUCC) {
		return status;
	}
	while (curr_token->type != TOKEN_EOF) {
		switch (curr_token->type) {
			case TOKEN_KEYWORD_EXTENSION:
				CHECK_STATUS(parse_extension(curr_token));
				break;
			case TOKEN_AT_MAIN:
				CHECK_STATUS(parse_at_main(curr_token));
				break;
			case TOKEN_KEYWORD_DEF:
				CHECK_STATUS(parse_func_def(curr_token, false));
				break;
			case TOKEN_EOL:
				break; // consumes empty lines
			default:
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									   "Missing EOL after header declaration\n"),
					PAR_FAIL);
		}
		ParserStatus status = fetch_token(curr_token);
		if (status.flag != PAR_SUCC) {
			return status;
		}
	}
	return PARSE_OK(PAR_EOF);
}

void parse() {
	Token *curr_token = malloc(sizeof(Token));
	if (curr_token == NULL) {
		// throw error code 99 and abort
	}
	ParserStatus status = parse_header(curr_token);
	if (status.flag == PAR_EOF) {
		free(curr_token);
		// end the program
	} else if (status.flag == LEX_FAIL) {
		free(curr_token);
		// end the program
	} else if (status.flag == PAR_FAIL) {
		free(curr_token);
		// end the program
	}
	while (true) {
		status = parse_program(curr_token);
		if (status.flag != PAR_SUCC) {
			break;
		}
	}
	switch (status.flag) {
		case PAR_SUCC:
		case PAR_FAIL:
		case LEX_FAIL:
		case PAR_EOF:
			break;
	}
	free(curr_token);
	return;
}
