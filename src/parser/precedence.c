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
static PrecLevel translate_token(Token *curr_token){
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

ParserStatus parse_prefix(Token *curr_token){
	ASTNode *node;
	OpType op;
	switch(curr_token->type){
		case TOKEN_MINUS:
		case TOKEN_NOT:
			op = (curr_token->type == TOKEN_MINUS) ? OP_UNARY_MINUS : OP_UNARY_NOP;
			GET_TOKEN(curr_token, (Error){0}, NULL);
			status = parse_prefix(curr_token);
			CHECK_STATUS(status);
			ALLOC_NODE(node, ast_create_unop(op, status.new_node, curr_token->line, curr_token->col));
			return PARSE_OK(PAR_SUCC, node);
		default:
			break;
	}
}

ParserStatus parse_climb(Token *curr_token, PrecLevel min_prec){
	ParserStatus status = {0};
	CHECK_STATUS(status);
	parse_prefix(curr_token);
	while (true){
		PrecLevel prec = translate_token(curr_token);
		if (prec < min_prec){
			break;
		}
	}
}

ParserStatus parse_expression(Token *curr_token){
	ParserStatus status = parse_climb(curr_token, 0);
	CHECK_STATUS(status);
	return PARSE_OK(PAR_SUCC, status.new_node);
}