/**
 * @project IFJ26 Compiler
 * @file parser.c
 * @brief Syntactic analyzer and recursive descent interface.
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
*/

#include "parser.h"
#include "../utils/error.h"
#include "../utils/token.h"
#include "../lexer/lexer.h"
#include <stdbool.h>
#include <string.h>

/**
 * @brief Macro for safely fetching from scanner
 * @param tok_ptr Token pointer to fetch into
 * @param err Error variable to catch scanner errors
 * @param prev_err Optional parameter to not lose initial error when consuming rest of rule and reaching EOF
*/
#define GET_TOKEN (tok_ptr, err, prev_err) do{ \
	(err) = lexer_get_next_token((tok_ptr)); \
	if (ERROR_IS_ERR((err))){ \
		return PARSE_ERROR((err), LEX_FAIL); \
	} \
	if ((tok_ptr)->type == TOKEN_EOF){ \
		if ((prev_err->type) != ERR_OK){ \
			return PARSE_ERROR((prev_err), PAR_EOF); \
		} else { \
			return PARSE_OK(PAR_EOF); \
		} \
	} \
} while(0)

/**
 * @brief Consumes symbols until passed token type or TOKEN_EOF
 * @param prev_err Error that led to call of this method
 * @param token_type Type of token to reach by consuming tokens
 * @return Status structure containing flags for caller
*/
ParserStatus consume_tokens(Error prev_err, TokenType token_type){
	Token* curr_token = NULL;
	do {
		GET_TOKEN(curr_token, (Error){0}, prev_err);
	} while(curr_token->type != token_type);
	return PARSE_ERROR(prev_err, PAR_FAIL);
}

/**
 * @brief Parsing of mandatory program header
 * @return Status structure containing flags for caller
*/
ParserStatus parse_header(){
	Token* curr_token = NULL;
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_KEYWORD_IMPORT){
		return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing mandatory import header (import ifj.*)"), TOKEN_EOL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_KEYWORD_IDENTIFIER || strcmp(curr_token->data->str_val, "ifj") != 0){
		return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Wrong import library in header (ifj.*)"), TOKEN_EOL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_KEYWORD_DOT){
		return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing ifj type in import (ifj.*)"), TOKEN_EOL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_KEYWORD_MUL){
		return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Wrong import library in header (ifj.*)"), TOKEN_EOL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_KEYWORD_EOL){
		return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing EOL after header declaration"), TOKEN_EOL);
	}
	return PARSE_OK(PAR_SUCC);
}

/**
 * @brief Function for parsing extension parameter
 * @return Status structure containing flags for caller
*/
ParseStatus parse_extension_param(){
	Token* curr_token = NULL;
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_IDENTIFIER){
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing identifier in extension parameter"), PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_COLON){
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing colon in extension parameter"), PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	switch(curr_token->type){
		case TOKEN_KEYWORD_INT:
		case TOKEN_KEYWORD_DOUBLE:
		case TOKEN_KEYWORD_UNIT:
		case TOKEN_KEYWORD_STRING:
			return PARSE_OK(PAR_SUCC);
			break;
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing data type in extention parameter"), PAR_FAIL);

	}
}

/**
 * @brief Function for parsing functions defined in extensions
 * @return Status structure containing flags for caller
*/
ParseStatus parse_extension_body(){
	Token* curr_token = NULL;
	GET_TOKEN(curr_token, (Error){0}, Error{0});
	if (curr_token->type == TOKEN_AT_MAIN){
		GET_TOKEN(curr_token, (Error){0}, (Error){0});
		if (curr_token->type == TOKEN_KEYWORD_EOL){
			GET_TOKEN(curr_token, (Error){0}, (Error){0});
			if (curr_token->type == TOKEN_KEYWORD_DEF){
				err = parse_func_def(); // add error handler
			} else {
				return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing definition token after @main"), TOKEN_EOL);
			}
		} else if (curr_token->type == TOKEN_KEYWORD_DEF){
			err = parse_func_def(); // add error handler
		} else {
			return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing definition token after @main on new line"), TOKEN_EOL);
		}
	} else if (curr_token->type == TOKEN_KEYWORD_DEF){
		parse_func_def();
	} else {
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Unexpected token in function declaration inside extension"), PAR_FAIL);
	}
}

/**
 * @brief Function for parsing extension definition
 * @return Status structure containing flags for caller
*/
ParseStatus parse_extension(){
	Token* curr_token = NULL;
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_LPAREN){
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing left bracket of extension parameter"), PAR_FAIL);
	}
	parse_extension_param(); // add error handler
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_RPAREN){
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing right bracket of extension parameter"), PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_EOL){
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing EOL after extension parameter bracket"), PAR_FAIL);
	}
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_INDENT){
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing left bracket of extension parameter"), PAR_FAIL);
	}
	parse_extension_body(); // add error handler
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	if (curr_token->type != TOKEN_OUTDENT){
		return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing outdent at the end of extension"), PAR_FAIL);
	}
	return PARSE_OK(PAR_SUCC);
}


/**
 * @brief Parsing of highest level program constructs (functions, extensions)
 * @return Status structure containing flags for caller
*/
ParserStatus parse_program(){
	Token* curr_token = NULL;
	ParserStatus err = {0};
	GET_TOKEN(curr_token, (Error){0}, (Error){0});
	while(curr_token != EOF){
		switch(curr_token->type){
			case TOKEN_KEYWORD_EXTENSION:
				err = parse_extension(); // add error handler
				break;
			case TOKEN_AT_MAIN:
				GET_TOKEN(curr_token, (Error){0}, (Error){0});
				if (curr_token->type == TOKEN_KEYWORD_EOL){
					GET_TOKEN(curr_token, (Error){0}, (Error){0});
					if (curr_token->type == TOKEN_KEYWORD_DEF){
						err = parse_func_def(); // add error handler
					} else {
						return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing definition token after @main"), TOKEN_EOL);
					}
				} else if (curr_token->type == TOKEN_KEYWORD_DEF){
					err = parse_func_def(); // add error handler
				} else {
					return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing definition token after @main"), TOKEN_EOL);
				}
				break;
			case TOKEN_KEYWORD_DEF:
				err = parse_func_def(); // add error handler
				break;
			default:
				return consume_tokens(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing EOL after header declaration"), TOKEN_EOL);\
		}
		GET_TOKEN(curr_token, (Error){0}, (Error){0});
	}
	return PARSE_OK(PAR_EOF);
}

void parse(){
	ParserStatus status = parse_header();
	if (status->flag == PAR_EOF){
		// end the program
	} else if (status->flag == LEX_FAIL){
		// end the program
	} else if (status->flag == PAR_FAIL){
		// continue
	}
	bool end_flag = false;
	while (!end_flag){
		status = parse_program();
		if (status->flag == PAR_EOF){
			end_flag = true;
		} else if (status->flag == LEX_FAIL){
			end_flag = true;
		} else if (status->flag == PAR_FAIL){
			// continue
		}
	}
}