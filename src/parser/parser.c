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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Declarations of functions with cyclic dependency
ParserStatus parse_param_mem(Token *curr_token);
ParserStatus parse_func_def(Token *curr_token, bool main_flag);
ParserStatus parse_expression(Token *curr_token, TokenType type);
ParserStatus parse_if_statement(Token* curr_token);
ParserStatus parse_code_block(Token* curr_token);
ParserStatus parse_func_body(Token *curr_token);

/**
 * @brief Macro for safely fetching from scanner
 * @param tok_ptr Token pointer to fetch into
 * @param err Error variable to catch scanner errors
 */
#define GET_TOKEN(tok_ptr, err)                                                                    \
	do {                                                                                           \
		(err) = lexer_get_next_token((tok_ptr));                                                   \
		if (ERROR_IS_ERR((err))) {                                                                 \
			return PARSE_ERROR((err), LEX_FAIL);                                                   \
		}                                                                                          \
		if ((tok_ptr)->type == TOKEN_EOF) {                                                        \
			return PARSE_OK(PAR_EOF);                                                              \
		}                                                                                          \
	} while (0)

/**
 * @brief Macro for checking status flags
 * @param status Status structure to be checked
 */
#define CHECK_STATUS(status)                                                                       \
	do {                                                                                           \
		ParserStatus tmp = (status);                                                               \
		if (tmp.flag != PAR_SUCC) {                                                                \
			return tmp;                                                                            \
		}                                                                                          \
	} while (0)

/**
 * @brief Parsing of mandatory program header
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_header(Token *curr_token) {
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_KEYWORD_IMPORT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing mandatory import header (import ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_IDENTIFIER ||
		dstring_cmp_str(curr_token->data.str_val, "ifj") != 0) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_KEYWORD_DO) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing ifj type in import (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_MUL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Wrong import library in header (ifj.*)\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, (Error){0});
		if (curr_token->type == TOKEN_KEYWORD_DEF) {
			CHECK_STATUS(parse_func_def(curr_token, true));
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
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing of declarations
 * @param curr_token Token pointer filled with current token
 * @param mutability Flag whether declared variable is mutable (true for var, false for val)
 * @return Status structure containing flags for caller
 */
ParserStatus parse_declaration(Token *curr_token, bool mutability) {
	(void)mutability; // remove later
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier name after function declaration\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type == TOKEN_COLON) {
		GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type == TOKEN_EQ) {
		CHECK_STATUS(parse_expression(curr_token, TOKEN_EOL));
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing assignment after variable declaration\n"),
			PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsiong of else decision branch
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_else_statement(Token *curr_token) {
	GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after then of if statement\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	CHECK_STATUS(parse_code_block(curr_token));
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type == TOKEN_KEYWORD_ELSE) {
		CHECK_STATUS(parse_else_statement(curr_token));
	}
	return PARSE_OK(PAR_SUCC);
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after do of while statement\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	CHECK_STATUS(parse_code_block(curr_token));
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing of for statement generator
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
*/
ParserStatus parse_for_generator(Token *curr_token){
	CHECK_STATUS(parse_expression(curr_token, TOKEN_EOL));
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type == TOKEN_EOL){
		GET_TOKEN(curr_token, (Error){0});
	}
	if (dstring_cmp_str(curr_token->data.str_val, "to") == 0) {
		GET_TOKEN(curr_token, (Error){0});
		CHECK_STATUS(parse_expression(curr_token, TOKEN_EOL)); //change
		if (dstring_cmp_str(curr_token->data.str_val, "by") != 0) {
			GET_TOKEN(curr_token, (Error){0});
			CHECK_STATUS(parse_expression(curr_token, TOKEN_EOL)); //change
		} else {
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									"Unexpected token after end of for statement generator\n"),
					PAR_FAIL);
		}
	} else if (dstring_cmp_str(curr_token->data.str_val, "until") == 0) {
		GET_TOKEN(curr_token, (Error){0});
		CHECK_STATUS(parse_expression(curr_token, TOKEN_EOL)); //change
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									"Unexpected token inside of for statement generator\n"),
					PAR_FAIL);
	}
	if (curr_token->type != TOKEN_EOL || dstring_cmp_str(curr_token->data.str_val, "do") != 0 || curr_token->type != TOKEN_KEYWORD_IF) {
			return PARSE_OK(PAR_SUCC);
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
ParserStatus parse_for_statement(Token* curr_token){
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, (Error){0});
	}
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									"Missing generator after for statement\n"),
					PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_LT) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									"Missing < after for statement generator\n"),
					PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_MINUS) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									"Missing - after < in for statement generator\n"),
					PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	CHECK_STATUS(parse_for_generator(curr_token));
	if (curr_token->type == TOKEN_EOL){
		while (true) {
			GET_TOKEN(curr_token, (Error){0});
			if (curr_token->type == TOKEN_KEYWORD_IF){
				CHECK_STATUS(parse_expression(curr_token, TOKEN_EOL)); // change
			}
			if (dstring_cmp_str(curr_token->data.str_val, "do") == 0){
				break;
			}
		}
	}
	if (dstring_cmp_str(curr_token->data.str_val, "do") != 0) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
									"Expected 'do' at the end of for statement, got different token\n"),
					PAR_FAIL);
	}
	CHECK_STATUS(parse_func_body(curr_token));
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
			CHECK_STATUS(parse_for_statement(curr_token));
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
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing of inner function body
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_func_body(Token *curr_token) {
	GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type == TOKEN_EOL) {
		GET_TOKEN(curr_token, (Error){0});
		return PARSE_OK(PAR_SUCC);
	} else if (curr_token->type == TOKEN_IDENTIFIER) {
		GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_COLON) {
		return PARSE_ERROR(
			ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
				"Missing colon after identifier of parameter in function declaration\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	switch (curr_token->type) {
		case TOKEN_KEYWORD_INT:
		case TOKEN_KEYWORD_DOUBLE:
		case TOKEN_KEYWORD_UNIT:
		case TOKEN_KEYWORD_STRING:
		case TOKEN_KEYWORD_BOOLEAN:
			GET_TOKEN(curr_token, (Error){0});
			if (curr_token->type == TOKEN_COMMA) {
				CHECK_STATUS(parse_param_tail(curr_token));
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
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Parsing of param sections of function declarations
 * @param curr_token Token pointer filled with current token
 * @return Status structure containing flags for caller
 */
ParserStatus parse_param_section(Token *curr_token) {
	GET_TOKEN(curr_token, (Error){0});
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
		GET_TOKEN(curr_token, (Error){0});
		if (curr_token->type == TOKEN_LPAREN) {
			CHECK_STATUS(parse_param_section(curr_token));
		} else {
			break;
		}
	}
	if (curr_token->type == TOKEN_COLON) {
		GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_IDENTIFIER) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing identifier in extension parameter\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_COLON) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing colon in extension parameter\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
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
		GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_LPAREN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing left bracket of extension parameter\n"),
			PAR_FAIL);
	}
	CHECK_STATUS(parse_extension_param(curr_token));
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_RPAREN) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing right bracket of extension parameter\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
	if (curr_token->type != TOKEN_EOL) {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Missing EOL after extension parameter bracket\n"),
			PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0});
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
	GET_TOKEN(curr_token, (Error){0});
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
		GET_TOKEN(curr_token, (Error){0});
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
	} else if (status.flag == LEX_FAIL) {
		free(curr_token);
		// end the program
	} else if (status.flag == PAR_FAIL) {
		switch (status.flag) {
			case PAR_FAIL:
			case LEX_FAIL:
			case PAR_EOF:
			case PAR_SUCC:
				break;
		}
		free(curr_token);
		return;
	}
}
