/**
 * @project IFJ26 Compiler
 * @file ast.c
 * @brief AST node constructors, recursive destructor and S-expression dumper.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "ast.h"
#include <stdlib.h>
#include "../_utils/dstring.h"

ASTNode *ast_create_int(long val, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_INT_LITERAL;
	node->line = line;
	node->col = col;
	node->int_val = val;
	return node;
}

ASTNode *ast_create_ident(const dstring_t *name, size_t line, size_t col) {
	if (!name) {
		return NULL;
	}
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}

	if (!dstring_init(&node->ident_name)) {
		free(node);
		return NULL;
	}

	if (!dstring_copy(&node->ident_name, name)) {
		dstring_free(&node->ident_name);
		free(node);
		return NULL;
	}

	node->type = NODE_IDENTIFIER;
	node->line = line;
	node->col = col;
	return node;
}

ASTNode *ast_create_binop(BinaryOpType op, ASTNode *left, ASTNode *right, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_BINARY_OP;
	node->line = line;
	node->col = col;
	node->binary_op.op = op;
	node->binary_op.left = left;
	node->binary_op.right = right;
	return node;
}

ASTNode *ast_create_var_decl(const dstring_t *name, ASTNode *init_expr, size_t line, size_t col) {
	if (!name) {
		return NULL;
	}
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}

	if (!dstring_init(&node->var_decl.var_name)) {
		free(node);
		return NULL;
	}

	if (!dstring_copy(&node->var_decl.var_name, name)) {
		dstring_free(&node->var_decl.var_name);
		free(node);
		return NULL;
	}

	node->type = NODE_VAR_DECL;
	node->line = line;
	node->col = col;
	node->var_decl.init_expr = init_expr;
	return node;
}

static const char *binop_to_str(BinaryOpType op) {
	switch (op) {
		case OP_ADD:    return "+";
		case OP_SUB:    return "-";
		case OP_MUL:    return "*";
		case OP_DIV:    return "/";
		case OP_ASSIGN: return "=";
		case OP_EQ:     return "==";
		case OP_NEQ:    return "!=";
		default:        return "?";
	}
}

void ast_dump_sexpr(const ASTNode *node, FILE *out) {
	if (!node) {
		fputs("()", out);
		return;
	}

	switch (node->type) {
		case NODE_INT_LITERAL:
			fprintf(out, "%ld", node->int_val);
			break;

		case NODE_IDENTIFIER: {
			const char *raw_str = dstring_get(&node->ident_name);
			fputs(raw_str ? raw_str : "<null>", out);
			break;
		}

		case NODE_BINARY_OP:
			fprintf(out, "(%s ", binop_to_str(node->binary_op.op));
			ast_dump_sexpr(node->binary_op.left, out);
			fputc(' ', out);
			ast_dump_sexpr(node->binary_op.right, out);
			fputc(')', out);
			break;

		case NODE_VAR_DECL: {
			const char *var_str = dstring_get(&node->var_decl.var_name);
			fprintf(out, "(var %s ", var_str ? var_str : "<null>");
			ast_dump_sexpr(node->var_decl.init_expr, out);
			fputc(')', out);
			break;
		}

		case NODE_ASSIGNMENT:
			fputs("(= ", out);
			ast_dump_sexpr(node->binary_op.left, out);
			fputc(' ', out);
			ast_dump_sexpr(node->binary_op.right, out);
			fputc(')', out);
			break;
	}
}

void ast_destroy(ASTNode *node) {
	if (!node) {
		return;
	}

	switch (node->type) {
		case NODE_INT_LITERAL:
			break;

		case NODE_IDENTIFIER:
			dstring_free(&node->ident_name);
			break;

		case NODE_BINARY_OP:
			ast_destroy(node->binary_op.left);
			ast_destroy(node->binary_op.right);
			break;

		case NODE_VAR_DECL:
			dstring_free(&node->var_decl.var_name);
			ast_destroy(node->var_decl.init_expr);
			break;

		case NODE_ASSIGNMENT:
			ast_destroy(node->binary_op.left);
			ast_destroy(node->binary_op.right);
			break;
	}

	free(node);
}
