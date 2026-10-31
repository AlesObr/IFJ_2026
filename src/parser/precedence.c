/**
 * @project IFJ26 Compiler
 * @file precedence.c
 * @brief Precedence expression parsing interface for parser
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "ast.h"
#include "parser.h"
#include "../_utils/error.h"
#include "../_utils/token.h"
#include "../lexer/lexer.h"
#include <stdbool.h>

/**
 * @brief Translater operator token into its precedence
 * @param curr_token Actual token pointer to be translated
 * @return Precedence level of token, NONE if precedence undefined
*/
static PrecedenceLevel translate_token(Token *curr_token){
	switch(curr_token->type){
		case TOKEN_NOT:
			return UNARY;
		case TOKEN_MULTI:
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
		default:
			return NONE;
	}
}