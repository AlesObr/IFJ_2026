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
 * @brief Consumes symbols until passed token type or TOKEN_EOF
 * @param token_type Type of token to reach by consuming tokens
 * @return Status structure containing flags for caller
*/
ParserStatus consume_tokens(TokenType token_type){
	Token* curr_token = NULL;
	Error err = lexer_get_next_token(curr_token);
	while(curr_token->type != token_type && curr_token->type != TOKEN_EOF && !(ERROR_IS_ERR(err))){
		err = lexer_get_next_token(curr_token);
	}
	if (ERROR_IS_ERR){
		return PARSE_ERROR(err, *curr_token);
	}
	return PARSE_OK(*curr_token);
}

/**
 * @brief Parsing of mandatory program header
 * @return Structure containing error informations
*/
Error parse_header(){
	Token* curr_token = NULL;
	lexer_get_next_token(curr_token);
	if (curr_token->type != TOKEN_KEYWORD_IMPORT){
		return ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing mandatory import header (import ifj.*)");
	}
	lexer_get_next_token(curr_token);
	if (curr_token->type != TOKEN_KEYWORD_IDENTIFIER || strcmp(curr_token->data->str_val, "ifj") != 0){
		return ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Wrong import library in header (ifj.*)");
	}
	lexer_get_next_token(curr_token);
	if (curr_token->type != TOKEN_KEYWORD_DOT){
		return ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing ifj type in import (ifj.*)");
	}
	lexer_get_next_token(curr_token);
	if (curr_token->type != TOKEN_KEYWORD_MUL){
		return ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Wrong import library in header (ifj.*)");
	}
	lexer_get_next_token(curr_token);
	if (curr_token->type != TOKEN_KEYWORD_EOL){
		return ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col, "Missing EOL after header declaration");
	}
	return ERROR_OK();
}

/**
 * @brief Parsing of highest level program constructs (functions, extensions)
 * @return Structure containing error informations
*/
Error parse_program(){
	Token* curr_token = NULL;
	Error err = lexer_get_next_token(curr_token);
	while(curr_token != EOF){
		switch(curr_token->type){
			case TOKEN_KEYWORD_EXTENSION:
				err = parse_extension();
				break;
			case TOKEN_AT_MAIN:
				err = parse_extension();
				break;
			case TOKEN_KEYWORD_DEF:
				err = parse_extension();
				break;
			default:

				break;
		}
		err = lexer_get_next_token(curr_token)
	}
	return err;
}

void parse(){
	Error err = parse_header();
	err = parse_program();
}