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

/* Helper functions for ASTNodeList */

static bool ast_nodelist_init(ASTNodeList *list) {
	if (!list) {
		return false;
	}
	list->items = NULL;
	list->count = 0;
	list->capacity = 0;
	return true;
}

static bool ast_nodelist_append(ASTNodeList *list, ASTNode *item) {
	if (!list || !item) {
		return false;
	}
	if (list->count >= list->capacity) {
		size_t new_cap = list->capacity == 0 ? 4 : list->capacity * 2;
		ASTNode **new_items = realloc(list->items, new_cap * sizeof(ASTNode *));
		if (!new_items) {
			return false;
		}
		list->items = new_items;
		list->capacity = new_cap;
	}
	list->items[list->count++] = item;
	return true;
}

static void ast_nodelist_destroy(ASTNodeList *list) {
	if (!list || !list->items) {
		return;
	}
	for (size_t i = 0; i < list->count; ++i) {
		ast_destroy(list->items[i]);
	}
	free(list->items);
	list->items = NULL;
	list->count = 0;
	list->capacity = 0;
}

/* --- Node constructors --- */

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

ASTNode *ast_create_float(double val, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_FLOAT_LITERAL;
	node->line = line;
	node->col = col;
	node->float_val = val;
	return node;
}

ASTNode *ast_create_string(const dstring_t *val, size_t line, size_t col) {
	if (!val) {
		return NULL;
	}
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	if (!dstring_init(&node->str_val)) {
		free(node);
		return NULL;
	}
	if (!dstring_copy(&node->str_val, val)) {
		dstring_free(&node->str_val);
		free(node);
		return NULL;
	}
	node->type = NODE_STRING_LITERAL;
	node->line = line;
	node->col = col;
	return node;
}

ASTNode *ast_create_null(size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_NULL_LITERAL;
	node->line = line;
	node->col = col;
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
	if (!dstring_init(&node->str_val)) {
		free(node);
		return NULL;
	}
	if (!dstring_copy(&node->str_val, name)) {
		dstring_free(&node->str_val);
		free(node);
		return NULL;
	}
	node->type = NODE_IDENTIFIER;
	node->line = line;
	node->col = col;
	return node;
}

ASTNode *ast_create_binop(OpType op, ASTNode *left, ASTNode *right, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_BINARY_OP;
	node->line = line;
	node->col = col;
	node->op.op = op;
	node->op.left = left;
	node->op.right = right;
	return node;
}

ASTNode *ast_create_unop(OpType op, ASTNode *expr, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_UNARY_OP;
	node->line = line;
	node->col = col;
	node->op.op = op;
	node->op.left = expr;
	node->op.right = NULL;
	return node;
}

ASTNode *ast_create_var_decl(const dstring_t *name, const dstring_t *type_name, bool is_const, ASTNode *init_expr, size_t line, size_t col) {
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

	if (!dstring_init(&node->var_decl.type_name)) {
		dstring_free(&node->var_decl.var_name);
		free(node);
		return NULL;
	}
	if (type_name && !dstring_copy(&node->var_decl.type_name, type_name)) {
		dstring_free(&node->var_decl.type_name);
		dstring_free(&node->var_decl.var_name);
		free(node);
		return NULL;
	}

	node->type = NODE_VAR_DECL;
	node->line = line;
	node->col = col;
	node->var_decl.is_const = is_const;
	node->var_decl.init_expr = init_expr;
	return node;
}

ASTNode *ast_create_assignment(const dstring_t *target, ASTNode *expr, size_t line, size_t col) {
	if (!target) {
		return NULL;
	}
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	if (!dstring_init(&node->assignment.target_name)) {
		free(node);
		return NULL;
	}
	if (!dstring_copy(&node->assignment.target_name, target)) {
		dstring_free(&node->assignment.target_name);
		free(node);
		return NULL;
	}
	node->type = NODE_ASSIGNMENT;
	node->line = line;
	node->col = col;
	node->assignment.expr = expr;
	return node;
}

ASTNode *ast_create_call(const dstring_t *func_name, size_t line, size_t col) {
	if (!func_name) {
		return NULL;
	}
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	if (!dstring_init(&node->call.func_name)) {
		free(node);
		return NULL;
	}
	if (!dstring_copy(&node->call.func_name, func_name)) {
		dstring_free(&node->call.func_name);
		free(node);
		return NULL;
	}
	ast_nodelist_init(&node->call.args);
	node->type = NODE_CALL;
	node->line = line;
	node->col = col;
	return node;
}

void ast_call_add_arg(ASTNode *call_node, ASTNode *arg) {
	if (call_node && call_node->type == NODE_CALL && arg) {
		ast_nodelist_append(&call_node->call.args, arg);
	}
}

ASTNode *ast_create_block(size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	ast_nodelist_init(&node->block.stmts);
	node->type = NODE_BLOCK;
	node->line = line;
	node->col = col;
	return node;
}

void ast_block_add_stmt(ASTNode *block_node, ASTNode *stmt) {
	if (block_node && block_node->type == NODE_BLOCK && stmt) {
		ast_nodelist_append(&block_node->block.stmts, stmt);
	}
}

ASTNode *ast_create_if(ASTNode *cond, ASTNode *then_b, ASTNode *else_b, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_IF;
	node->line = line;
	node->col = col;
	node->if_stmt.cond = cond;
	node->if_stmt.then_branch = then_b;
	node->if_stmt.else_branch = else_b;
	return node;
}

ASTNode *ast_create_while(ASTNode *cond, ASTNode *body, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_WHILE;
	node->line = line;
	node->col = col;
	node->while_stmt.cond = cond;
	node->while_stmt.body = body;
	return node;
}

ASTNode *ast_create_return(ASTNode *expr, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (!node) {
		return NULL;
	}
	node->type = NODE_RETURN;
	node->line = line;
	node->col = col;
	node->return_stmt.expr = expr;
	return node;
}

/* --- Serialization to S-expression --- */
/*
 *	This functions were implemented primarily for debugging and testing purposes - mainly for unit testing the parser and AST construction.
 */




static const char *op_to_str(OpType op) {
	switch (op) {
		case OP_ADD:         return "+";
		case OP_SUB:         return "-";
		case OP_MUL:         return "*";
		case OP_DIV:         return "/";
		case OP_CONCAT:      return "~";
		case OP_ASSIGN:      return "=";
		case OP_EQ:          return "==";
		case OP_NEQ:         return "!=";
		case OP_LT:          return "<";
		case OP_LTE:         return "<=";
		case OP_GT:          return ">";
		case OP_GTE:         return ">=";
		case OP_NOT:         return "!";
		case OP_UNARY_MINUS: return "-";
		default:             return "?";
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

		case NODE_FLOAT_LITERAL:
			fprintf(out, "%g", node->float_val);
			break;

		case NODE_STRING_LITERAL: {
			const char *raw = dstring_get(&node->str_val);
			fprintf(out, "\"%s\"", raw ? raw : "");
			break;
		}

		case NODE_NULL_LITERAL:
			fputs("null", out);
			break;

		case NODE_IDENTIFIER: {
			const char *raw = dstring_get(&node->str_val);
			fputs(raw ? raw : "<null>", out);
			break;
		}

		case NODE_BINARY_OP:
			fprintf(out, "(%s ", op_to_str(node->op.op));
			ast_dump_sexpr(node->op.left, out);
			fputc(' ', out);
			ast_dump_sexpr(node->op.right, out);
			fputc(')', out);
			break;

		case NODE_UNARY_OP:
			fprintf(out, "(%s ", op_to_str(node->op.op));
			ast_dump_sexpr(node->op.left, out);
			fputc(')', out);
			break;

		case NODE_VAR_DECL: {
			const char *name = dstring_get(&node->var_decl.var_name);
			const char *type = dstring_get(&node->var_decl.type_name);
			fprintf(out, "(%s %s%s%s ",
					node->var_decl.is_const ? "const" : "var",
					name ? name : "<null>",
					type ? ":" : "",
					type ? type : "");
			ast_dump_sexpr(node->var_decl.init_expr, out);
			fputc(')', out);
			break;
		}

		case NODE_ASSIGNMENT: {
			const char *target = dstring_get(&node->assignment.target_name);
			fprintf(out, "(= %s ", target ? target : "<null>");
			ast_dump_sexpr(node->assignment.expr, out);
			fputc(')', out);
			break;
		}

		case NODE_CALL: {
			const char *fn = dstring_get(&node->call.func_name);
			fprintf(out, "(call %s", fn ? fn : "<null>");
			for (size_t i = 0; i < node->call.args.count; ++i) {
				fputc(' ', out);
				ast_dump_sexpr(node->call.args.items[i], out);
			}
			fputc(')', out);
			break;
		}

		case NODE_BLOCK:
			fputs("(block", out);
			for (size_t i = 0; i < node->block.stmts.count; ++i) {
				fputc(' ', out);
				ast_dump_sexpr(node->block.stmts.items[i], out);
			}
			fputc(')', out);
			break;

		case NODE_IF:
			fputs("(if ", out);
			ast_dump_sexpr(node->if_stmt.cond, out);
			fputc(' ', out);
			ast_dump_sexpr(node->if_stmt.then_branch, out);
			if (node->if_stmt.else_branch) {
				fputc(' ', out);
				ast_dump_sexpr(node->if_stmt.else_branch, out);
			}
			fputc(')', out);
			break;

		case NODE_WHILE:
			fputs("(while ", out);
			ast_dump_sexpr(node->while_stmt.cond, out);
			fputc(' ', out);
			ast_dump_sexpr(node->while_stmt.body, out);
			fputc(')', out);
			break;

		case NODE_RETURN:
			fputs("(return", out);
			if (node->return_stmt.expr) {
				fputc(' ', out);
				ast_dump_sexpr(node->return_stmt.expr, out);
			}
			fputc(')', out);
			break;
	}
}

/* --- Recursive destructor --- */

void ast_destroy(ASTNode *node) {
	if (!node) {
		return;
	}

	switch (node->type) {
		case NODE_INT_LITERAL:
		case NODE_FLOAT_LITERAL:
		case NODE_NULL_LITERAL:
			break;

		case NODE_STRING_LITERAL:
		case NODE_IDENTIFIER:
			dstring_free(&node->str_val);
			break;

		case NODE_BINARY_OP:
			ast_destroy(node->op.left);
			ast_destroy(node->op.right);
			break;

		case NODE_UNARY_OP:
			ast_destroy(node->op.left);
			break;

		case NODE_VAR_DECL:
			dstring_free(&node->var_decl.var_name);
			dstring_free(&node->var_decl.type_name);
			ast_destroy(node->var_decl.init_expr);
			break;

		case NODE_ASSIGNMENT:
			dstring_free(&node->assignment.target_name);
			ast_destroy(node->assignment.expr);
			break;

		case NODE_CALL:
			dstring_free(&node->call.func_name);
			ast_nodelist_destroy(&node->call.args);
			break;

		case NODE_BLOCK:
			ast_nodelist_destroy(&node->block.stmts);
			break;

		case NODE_IF:
			ast_destroy(node->if_stmt.cond);
			ast_destroy(node->if_stmt.then_branch);
			ast_destroy(node->if_stmt.else_branch);
			break;

		case NODE_WHILE:
			ast_destroy(node->while_stmt.cond);
			ast_destroy(node->while_stmt.body);
			break;

		case NODE_RETURN:
			ast_destroy(node->return_stmt.expr);
			break;
	}

	free(node);
}
