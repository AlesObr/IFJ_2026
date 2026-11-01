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

/**
 * @brief Parsing of call parameters of member function
 * @param curr_token
 * @param node Node to append parameters into
*/
ParserStatus parse_call_pameters(Token *curr_token, ASTNode *node){
	while(curr_token->type == TOKEN_LPAREN){
		ASTNode *window = NULL;
		ALLOC_NODE(window, ast_create_param_window(curr_token->line, curr_token->col));
		CHECK_AST_APPEND(node, ast_call_add_window(node, window));
		ParserStatus status = parse_climb(curr_token, 0);
		CHECK_STATUS(status);
		CHECK_AST_APPEND(node, ast_window_add_param(window, status.new_node));
		while (true) {
			if (curr_token->type == TOKEN_RPAREN){
				break;
			} else if (curr_token->type == TOKEN_COMMA){
				GET_TOKEN(curr_token, (Error){0}, node);
				status = parse_climb(curr_token, 0);
				CHECK_STATUS(status);
				CHECK_AST_APPEND(node, ast_window_add_param(window, status.new_node));
			} else {
				ast_destroy(node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Invalid token in expression call parameter parsing (expected TOKEN_RBRACE or TOKEN_COMMA)\n"),
					PAR_FAIL);
			}
		}
		GET_TOKEN(curr_token, (Error){0}, node);
	}
	return PARSE_OK(PAR_SUCC, node);
}

/**
 * @brief Parsing of right asociativity operators and function calls
 * @param curr_token Current token given by scanner
 * @return ParserStatus structure containing flags for parser
 */
ParserStatus parse_prefix(Token *curr_token){
	ParserStatus status = {0};
	ASTNode *node;
	OpType op;
	switch(curr_token->type){
		case TOKEN_MINUS:
		case TOKEN_NOT:
			op = (curr_token->type == TOKEN_MINUS) ? OP_UNARY_MINUS : OP_UNARY_NOT;
			GET_TOKEN(curr_token, (Error){0}, NULL);
			status = parse_prefix(curr_token);
			CHECK_STATUS(status);
			ALLOC_NODE(node, ast_create_unop(op, status.new_node, curr_token->line, curr_token->col));
			return PARSE_OK(PAR_SUCC, node);
			break;
		case TOKEN_LIT_INT:
			ALLOC_NODE(node, ast_create_int(curr_token->data.int_val, curr_token->line, curr_token->col));
			break;
		case TOKEN_LIT_FLOAT:
			ALLOC_NODE(node, ast_create_float(curr_token->data.float_val, curr_token->line, curr_token->col));
			break;
		case TOKEN_LIT_STRING:
			ALLOC_NODE(node, ast_create_string(curr_token->data.str_val, curr_token->line, curr_token->col));
			break;
		case TOKEN_IDENTIFIER:
			ALLOC_NODE(node, ast_create_ident(curr_token->data.str_val, curr_token->line, curr_token->col));
			break;
		case TOKEN_LPAREN:
			GET_TOKEN(curr_token, (Error){0}, NULL);
			status = parse_climb(curr_token, 0);
			CHECK_STATUS(status);
			if (curr_token->type != TOKEN_RPAREN){
				ast_destroy(status.new_node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Invalid token in expression prefix parsing (expected TOKEN_RBRACE)\n"),
					PAR_FAIL);
			}
			node = status.new_node;
			break;
		default:
			return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Invalid token in expression prefix parsing\n"),
					PAR_FAIL);
			break;
	}
	GET_TOKEN(curr_token, (Error){0}, node);
	while (curr_token->type == TOKEN_DOT || curr_token->type == TOKEN_LPAREN) {
		if (curr_token->type == TOKEN_DOT){
			GET_TOKEN(curr_token, (Error){0}, node);
			if (curr_token->type != TOKEN_IDENTIFIER){
				ast_destroy(node);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expected identifier after . function call, got something else\n"),
					PAR_FAIL);
			}
			ASTNode *tmp_node;
			ALLOC_NODE(tmp_node, ast_create_call(curr_token->data.str_val, curr_token->line, curr_token->col));
			CHECK_AST_APPEND(tmp_node, ast_call_add_target(tmp_node, node));
			GET_TOKEN(curr_token, (Error){0}, node);
			if(curr_token->type == TOKEN_LPAREN){
				status = parse_call_parameters(curr_token, tmp_node);
				CHECK_STATUS(status);
			}
			node = tmp_node;
		} else {
			status = parse_call_expression(curr_token);
			CHECK_STATUS(status);
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
ParserStatus parse_climb(Token *curr_token, PrecLevel min_prec){
	ParserStatus status = parse_prefix(curr_token);
	CHECK_STATUS(status);
	ASTNode* node = status.new_node;
	while (true){
		PrecLevel prec = translate_token(curr_token);
		if (prec < min_prec){
			break;
		}
		GET_TOKEN(curr_token, (Error){0}, node);
		status = parse_climb(curr_token, min_prec + 1);
		CHECK_STATUS(status);
		ASTNode *bin_op;
		ALLOC_NODE(bin_op, ast_create_binop(bin_op, node, status.new_node, curr_token->line, curr_token->col));
		if (op == RELATIONAL || op == EQUALITY){
			GET_TOKEN(curr_token, (Error){0}, node);
			if ((translate_token(curr_token)) != NONE){
				ast_destroy(bin_op);
				return PARSE_ERROR(ERROR_EMIT(ERR_SYNTAX, curr_token->line, curr_token->col,
							   "Expression parsing error, two relational operators back to back\n"),
					PAR_FAIL);
			}
		}
		node = bin_op;
	}
	return PARSE_OK(PAR_SUCC, node);
}

ParserStatus parse_expression(Token *curr_token){
	ParserStatus status = parse_climb(curr_token, 0);
	CHECK_STATUS(status);
	return PARSE_OK(PAR_SUCC, status.new_node);
}