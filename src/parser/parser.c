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
 * @brief Parsing of mandatory program header
 * @return Structure containing error information
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
 * @return Structure containing error information
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