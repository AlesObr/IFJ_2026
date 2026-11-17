/**
 * @project IFJ26 Compiler
 * @file ast.c
 * @brief AST node constructors, recursive destructor and S-expression dumper.
 * @author Aleš Obr (xobrale00)
 * @author Štěpán Hořenek (xhorens00)
 * @note Standard: ISO C11
 */

#include "ast.h"
#include "../_utils/dstring.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* --- Helper functions for ASTNodeList --- */

static void ast_nodelist_init(ASTNodeList *list) {
	list->items = NULL;
	list->count = 0;
	list->capacity = 0;
}

static bool ast_nodelist_append(ASTNodeList *list, ASTNode *item) {
	if (list == NULL || item == NULL) {
		return false;
	}
	if (list->count >= list->capacity) {
		size_t new_cap = list->capacity == 0 ? 4 : list->capacity * 2;
		if (new_cap > SIZE_MAX / sizeof(ASTNode *)) {
			return false;
		}
		ASTNode **new_items = realloc(list->items, new_cap * sizeof(ASTNode *));
		if (new_items == NULL) {
			return false;
		}
		list->items = new_items;
		list->capacity = new_cap;
	}
	list->items[list->count++] = item;
	return true;
}

static void ast_nodelist_destroy(ASTNodeList *list) {
	if (list == NULL) {
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

static const char *val_type_to_str(ValType type) {
	switch (type) {
		case INT:
			return "Int";
		case DOUBLE:
			return "Double";
		case UNIT:
			return "Unit";
		case STRING:
			return "String";
		case BOOLEAN:
			return "Boolean";
		case ANY:
			return "Any";
		case UNDEF:
			return "Undef";
		default:
			return "";
	}
}

static void ast_dump_nodelist(const ASTNodeList *list, FILE *out) {
	if (list == NULL) {
		return;
	}
	for (size_t i = 0; i < list->count; ++i) {
		fputc(' ', out);
		ast_dump_sexpr(list->items[i], out);
	}
}

/* --- Internal allocation helper --- */

/**
 * @brief Allocates a bare node and fills in the common header fields.
 * @return Newly allocated node, or NULL on allocation failure.
 */
static ASTNode *ast_alloc(ASTNodeType type, size_t line, size_t col) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (node == NULL) {
		return NULL;
	}
	node->type = type;
	node->line = line;
	node->col = col;
	node->expr_types = UNDEF;
	node->to_double = false;
	node->sym_id = 0;
	return node;
}

/**
 * @brief Initializes @p dst and copies @p src into it.
 *
 * A NULL @p src yields an initialized but empty string, which is how an
 * omitted (inferred) type annotation is represented.
 *
 * @return true on success; on failure @p dst is left uninitialized/freed.
 */
static bool ast_dstring_clone(dstring_t *dst, const dstring_t *src) {
	if (!dstring_init(dst)) {
		return false;
	}
	if (src != NULL && !dstring_copy(dst, src)) {
		dstring_free(dst);
		return false;
	}
	return true;
}

/* --- Node constructors --- */

ASTNode *ast_create_int(long val, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_INT_LITERAL, line, col);
	if (node == NULL) {
		return NULL;
	}
	node->int_val = val;
	return node;
}

ASTNode *ast_create_float(double val, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_FLOAT_LITERAL, line, col);
	if (node == NULL) {
		return NULL;
	}
	node->float_val = val;
	return node;
}

ASTNode *ast_create_string(const dstring_t *val, size_t line, size_t col) {
	if (val == NULL) {
		return NULL;
	}
	ASTNode *node = ast_alloc(NODE_STRING_LITERAL, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->str_val, val)) {
		free(node);
		return NULL;
	}
	return node;
}

ASTNode *ast_create_null(size_t line, size_t col) {
	return ast_alloc(NODE_NULL_LITERAL, line, col);
}

ASTNode *ast_create_ident(const dstring_t *name, size_t line, size_t col) {
	if (name == NULL) {
		return NULL;
	}
	ASTNode *node = ast_alloc(NODE_IDENTIFIER, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->str_val, name)) {
		free(node);
		return NULL;
	}
	return node;
}

ASTNode *ast_create_binop(OpType op, ASTNode *left, ASTNode *right, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_BINARY_OP, line, col);
	if (node == NULL) {
		ast_destroy(left);
		ast_destroy(right);
		return NULL;
	}
	node->op.op = op;
	node->op.left = left;
	node->op.right = right;
	return node;
}

ASTNode *ast_create_unop(OpType op, ASTNode *expr, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_UNARY_OP, line, col);
	if (node == NULL) {
		ast_destroy(expr);
		return NULL;
	}
	node->op.op = op;
	node->op.left = expr;
	node->op.right = NULL;
	return node;
}

ASTNode *ast_create_var_decl(const dstring_t *name, bool is_const, size_t line, size_t col) {
	ASTNode *node = NULL;
	if (name == NULL) {
		return NULL;
	}
	node = ast_alloc(NODE_VAR_DECL, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->var_decl.var_name, name)) {
		free(node);
		return NULL;
	}
	ast_nodelist_init(&node->var_decl.type_name);
	node->var_decl.is_const = is_const;
	node->var_decl.init_expr = NULL;
	return node;
}

bool ast_var_decl_add_init(ASTNode *var_decl_node, ASTNode *init_expr) {
	if (var_decl_node == NULL || var_decl_node->type != NODE_VAR_DECL || init_expr == NULL) {
		ast_destroy(init_expr);
		return false;
	}
	var_decl_node->var_decl.init_expr = init_expr;
	return true;
}

ASTNode *ast_create_call(const dstring_t *func_name, size_t line, size_t col) {
	if (func_name == NULL) {
		return NULL;
	}
	ASTNode *node = ast_alloc(NODE_CALL, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->call.func_name, func_name)) {
		free(node);
		return NULL;
	}
	node->call.target = NULL;
	ast_nodelist_init(&node->call.args);
	return node;
}

bool ast_call_add_window(ASTNode *func_call, ASTNode *param_window) {
	if (func_call == NULL || func_call->type != NODE_CALL || param_window == NULL ||
		param_window->type != NODE_FUNC_WINDOW) {
		ast_destroy(param_window);
		return false;
	}
	if (!ast_nodelist_append(&func_call->call.args, param_window)) {
		ast_destroy(param_window);
		return false;
	}
	return true;
}

bool ast_call_add_target(ASTNode *call_node, ASTNode *target) {
	if (call_node == NULL || call_node->type != NODE_CALL || target == NULL) {
		return false;
	}
	call_node->call.target = target;
	return true;
}

bool ast_call_add_arg(ASTNode *call_node, ASTNode *arg) {
	if (call_node == NULL || call_node->type != NODE_CALL || arg == NULL) {
		ast_destroy(arg);
		return false;
	}
	if (!ast_nodelist_append(&call_node->call.args, arg)) {
		ast_destroy(arg);
		return false;
	}
	return true;
}

ASTNode *ast_create_func_param(const dstring_t *param_name, size_t line, size_t col) {
	ASTNode *node = NULL;
	if (param_name == NULL) {
		return NULL;
	}
	node = ast_alloc(NODE_FUNC_PARAM, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->func_param.param_name, param_name)) {
		free(node);
		return NULL;
	}
	ast_nodelist_init(&node->func_param.param_type);
	return node;
}

ASTNode *ast_create_func(const dstring_t *func_name, bool is_main, size_t line, size_t col) {
	if (func_name == NULL) {
		return NULL;
	}
	ASTNode *node = ast_alloc(NODE_FUNC_DECL, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->func_decl.func_name, func_name)) {
		free(node);
		return NULL;
	}
	node->func_decl.is_main = is_main;
	node->func_decl.body = NULL;
	ast_nodelist_init(&node->func_decl.param_windows);
	ast_nodelist_init(&node->func_decl.return_type);
	return node;
}

ASTNode *ast_create_param_window(size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_FUNC_WINDOW, line, col);
	if (node == NULL) {
		return NULL;
	}
	ast_nodelist_init(&node->param_window.params);
	return node;
}

bool ast_func_add_param_window(ASTNode *func_node, ASTNode *param_window) {
	if (func_node == NULL || func_node->type != NODE_FUNC_DECL || param_window == NULL ||
		param_window->type != NODE_FUNC_WINDOW) {
		ast_destroy(param_window);
		return false;
	}
	if (!ast_nodelist_append(&func_node->func_decl.param_windows, param_window)) {
		ast_destroy(param_window);
		return false;
	}
	return true;
}

bool ast_window_add_param(ASTNode *param_window, ASTNode *param) {
	if (param_window == NULL || param_window->type != NODE_FUNC_WINDOW || param == NULL) {
		ast_destroy(param);
		return false;
	}
	if (!ast_nodelist_append(&param_window->param_window.params, param)) {
		ast_destroy(param);
		return false;
	}
	return true;
}

bool ast_func_add_block(ASTNode *func_node, ASTNode *code_block) {
	if (func_node == NULL || func_node->type != NODE_FUNC_DECL || code_block == NULL) {
		ast_destroy(code_block);
		return false;
	}
	func_node->func_decl.body = code_block;
	return true;
}

ASTNode *ast_ext_create(const dstring_t *param_name, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_EXT_DECL, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->ext_decl.param_name, param_name)) {
		free(node);
		return NULL;
	}
	ast_nodelist_init(&node->ext_decl.funcs);
	ast_nodelist_init(&node->ext_decl.type_name);
	return node;
}

bool ast_add_type(
	ASTNode *node, ValType type_name_orig, const dstring_t *type_name_def, bool is_orig) {
	if (node != NULL) {
		ASTNode *new_node = NULL;
		bool res;
		if (is_orig) {
			new_node = ast_alloc(NODE_TYPE_ORIG, node->line, node->col);
			if (new_node == NULL) {
				return false;
			}
			new_node->data_type.orig_type = type_name_orig;
		} else {
			new_node = ast_alloc(NODE_TYPE_USER, node->line, node->col);
			if (new_node == NULL) {
				return false;
			}
			if (!ast_dstring_clone(&new_node->data_type.defined_type, type_name_def)) {
				ast_destroy(new_node);
				return NULL;
			}
		}
		switch (node->type) {
			case NODE_EXT_DECL:
				res = ast_nodelist_append(&node->ext_decl.type_name, new_node);
				break;
			case NODE_FUNC_PARAM:
				res = ast_nodelist_append(&node->func_param.param_type, new_node);
				break;
			case NODE_FUNC_DECL:
				res = ast_nodelist_append(&node->func_decl.return_type, new_node);
				break;
			case NODE_VAR_DECL:
				res = ast_nodelist_append(&node->var_decl.type_name, new_node);
				break;
			case NODE_UNIF:
				res = ast_nodelist_append(&node->type_unif.types, new_node);
				break;
			case NODE_CASE:
				res = true;
				node->case_node.type = new_node;
				break;
			default:
				res = false;
				break;
		}
		if (!res) {
			ast_destroy(new_node);
			return false;
		}
		return true;
	} else {
		return false;
	}
}

bool ast_ext_add_func(ASTNode *ext_node, ASTNode *func_node) {
	if (ext_node == NULL || ext_node->type != NODE_EXT_DECL || func_node == NULL ||
		func_node->type != NODE_FUNC_DECL) {
		ast_destroy(func_node);
		return false;
	}
	if (!ast_nodelist_append(&ext_node->ext_decl.funcs, func_node)) {
		ast_destroy(func_node);
		return false;
	}
	return true;
}

ASTNode *ast_create_block(size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_BLOCK, line, col);
	if (node == NULL) {
		return NULL;
	}
	ast_nodelist_init(&node->block.stmts);
	return node;
}

bool ast_block_add_stmt(ASTNode *block_node, ASTNode *stmt) {
	if (block_node == NULL || block_node->type != NODE_BLOCK || stmt == NULL) {
		ast_destroy(stmt);
		return false;
	}
	if (!ast_nodelist_append(&block_node->block.stmts, stmt)) {
		ast_destroy(stmt);
		return false;
	}
	return true;
}

ASTNode *ast_create_if(ASTNode *cond, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_IF, line, col);
	if (node == NULL) {
		ast_destroy(cond);
		return NULL;
	}
	node->if_else_stmt.cond = cond;
	node->if_else_stmt.body = NULL;
	ast_nodelist_init(&node->if_else_stmt.next_conds);
	return node;
}

ASTNode *ast_create_else(ASTNode *cond, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_ELSE, line, col);
	if (node == NULL) {
		ast_destroy(cond);
		return NULL;
	}
	node->if_else_stmt.cond = cond;
	node->if_else_stmt.body = NULL;
	ast_nodelist_init(&node->if_else_stmt.next_conds);
	return node;
}

bool ast_if_else_add_body(ASTNode *if_else_node, ASTNode *expr_block) {
	if (if_else_node == NULL ||
		!(if_else_node->type == NODE_IF || if_else_node->type == NODE_ELSE) || expr_block == NULL) {
		ast_destroy(expr_block);
		return false;
	}
	if_else_node->if_else_stmt.body = expr_block;
	return true;
}

bool ast_while_add_body(ASTNode *while_node, ASTNode *code_block) {
	if (while_node == NULL || while_node->type != NODE_WHILE || code_block == NULL ||
		code_block->type != NODE_BLOCK) {
		ast_destroy(code_block);
		return false;
	}
	while_node->while_stmt.body = code_block;
	return true;
}

bool ast_if_else_append_branch(ASTNode *if_else_node, ASTNode *next_condition) {
	if (if_else_node == NULL ||
		!(if_else_node->type == NODE_IF || if_else_node->type == NODE_ELSE) ||
		next_condition == NULL || next_condition->type != NODE_ELSE) {
		ast_destroy(next_condition);
		return false;
	}
	if (!ast_nodelist_append(&if_else_node->if_else_stmt.next_conds, next_condition)) {
		ast_destroy(next_condition);
		return false;
	}
	return true;
}

ASTNode *ast_create_while(ASTNode *cond, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_WHILE, line, col);
	if (node == NULL) {
		ast_destroy(cond);
		return NULL;
	}
	node->while_stmt.cond = cond;
	node->while_stmt.body = NULL;
	return node;
}

ASTNode *ast_create_return(ASTNode *expr, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_RETURN, line, col);
	if (node == NULL) {
		ast_destroy(expr);
		return NULL;
	}
	node->return_stmt.expr = expr;
	return node;
}

bool ast_class_add_param(ASTNode *data_class, ASTNode *param_window) {
	if (data_class == NULL || data_class->type != NODE_CLASS_DECL || param_window == NULL ||
		param_window->type != NODE_FUNC_WINDOW) {
		ast_destroy(param_window);
		return false;
	}
	data_class->data_class.param_list = param_window;
	return true;
}

ASTNode *ast_create_for(const dstring_t *gen_id, size_t line, size_t col) {
	ASTNode *node = ast_alloc(NODE_FOR, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->for_stmt.gen_id, gen_id)) {
		free(node);
		return NULL;
	}
	node->for_stmt.gen_start = NULL;
	node->for_stmt.gen_end = NULL;
	node->for_stmt.gen_step = NULL;
	node->for_stmt.body = NULL;
	node->for_stmt.last_in = false;
	ast_nodelist_init(&node->for_stmt.guards);
	return node;
}

bool ast_for_add_start(ASTNode *for_node, ASTNode *expr) {
	if (for_node == NULL || for_node->type != NODE_FOR || expr == NULL) {
		ast_destroy(expr);
		return false;
	}
	for_node->for_stmt.gen_start = expr;
	return true;
}

bool ast_for_add_end(ASTNode *for_node, ASTNode *expr, bool last_in) {
	if (for_node == NULL || for_node->type != NODE_FOR || expr == NULL) {
		ast_destroy(expr);
		return false;
	}
	for_node->for_stmt.gen_end = expr;
	for_node->for_stmt.last_in = last_in;
	return true;
}

bool ast_for_add_step(ASTNode *for_node, ASTNode *expr) {
	if (for_node == NULL || for_node->type != NODE_FOR || expr == NULL) {
		ast_destroy(expr);
		return false;
	}
	for_node->for_stmt.gen_step = expr;
	return true;
}

bool ast_for_add_body(ASTNode *for_node, ASTNode *code_block) {
	if (for_node == NULL || for_node->type != NODE_FOR || code_block == NULL ||
		code_block->type != NODE_BLOCK) {
		ast_destroy(code_block);
		return false;
	}
	for_node->for_stmt.body = code_block;
	return true;
}

bool ast_for_append_guard(ASTNode *for_node, ASTNode *guard) {
	if (for_node == NULL || for_node->type != NODE_FOR || guard == NULL) {
		ast_destroy(guard);
		return false;
	}
	if (!ast_nodelist_append(&for_node->for_stmt.guards, guard)) {
		ast_destroy(guard);
		return false;
	}
	return true;
}

ASTNode *ast_create_match_node(ASTNode *match_expr, size_t line, size_t col) {
	if (match_expr == NULL) {
		return NULL;
	}
	ASTNode *new_node = ast_alloc(NODE_MATCH, line, col);
	if (new_node == NULL) {
		ast_destroy(match_expr);
		return NULL;
	}
	new_node->match.switch_expr = match_expr;
	ast_nodelist_init(&new_node->match.cases);
	return new_node;
}

ASTNode *ast_create_case_node(ASTNode *case_expr, size_t line, size_t col) {
	if (case_expr == NULL) {
		return NULL;
	}
	ASTNode *new_node = ast_alloc(NODE_CASE, line, col);
	if (new_node == NULL) {
		ast_destroy(case_expr);
		return NULL;
	}
	new_node->case_node.case_member = case_expr;
	new_node->case_node.guard = NULL;
	new_node->case_node.body_expr = NULL;
	return new_node;
}

bool ast_match_add_case(ASTNode *match_node, ASTNode *case_node) {
	if (match_node == NULL || match_node->type != NODE_MATCH || case_node == NULL ||
		case_node->type != NODE_CASE) {
		ast_destroy(case_node);
		return false;
	}
	if (!ast_nodelist_append(&match_node->match.cases, case_node)) {
		ast_destroy(case_node);
		return false;
	}
	return true;
}

bool ast_case_add_guard(ASTNode *case_node, ASTNode *guard) {
	if (case_node == NULL || case_node->type != NODE_CASE || guard == NULL) {
		ast_destroy(guard);
		return false;
	}
	case_node->case_node.guard = guard;
	return true;
}

bool ast_case_add_body(ASTNode *case_node, ASTNode *body) {
	if (case_node == NULL || case_node->type != NODE_CASE || body == NULL) {
		ast_destroy(body);
		return false;
	}
	case_node->case_node.body_expr = body;
	return true;
}

ASTNode *ast_create_type_unif(const dstring_t *unif_name, size_t line, size_t col) {
	if (unif_name == NULL) {
		return false;
	}
	ASTNode *node = ast_alloc(NODE_UNIF, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->type_unif.unif_name, unif_name)) {
		free(node);
		return NULL;
	}
	ast_nodelist_init(&node->type_unif.types);
	return node;
}

ASTNode *ast_create_data_class(const dstring_t *class_name, size_t line, size_t col) {
	if (class_name == NULL) {
		return false;
	}
	ASTNode *node = ast_alloc(NODE_CLASS_DECL, line, col);
	if (node == NULL) {
		return NULL;
	}
	if (!ast_dstring_clone(&node->data_class.class_name, class_name)) {
		free(node);
		return NULL;
	}
	return node;
}

/* --- Serialization to S-expression --- */
/*
 * These functions were implemented primarily for debugging and testing purposes --
 * mainly for unit testing the parser and AST construction.
 */

static const char *op_to_str(OpType op) {
	switch (op) {
		case OP_ADD:
			return "+";
		case OP_SUB:
			return "-";
		case OP_MUL:
			return "*";
		case OP_DIV:
			return "/";
		case OP_CONCAT:
			return "~";
		case OP_ASSIGN:
			return "=";
		case OP_EQ:
			return "==";
		case OP_NEQ:
			return "!=";
		case OP_LT:
			return "<";
		case OP_LTE:
			return "<=";
		case OP_GT:
			return ">";
		case OP_GTE:
			return ">=";
		case OP_NOT:
			return "!";
		case OP_UNARY_MINUS:
			return "-";
		case OP_UNARY_PLUS:
			return "+";
		case OP_AND:
			return "&&";
		case OP_OR:
			return "||";
		default:
			return "?";
	}
}

/**
 * @brief Dumps a type annotation as ":T" or ":T1|T2" for a union; no annotation is ":Undef".
 * @param types List of NODE_TYPE_ORIG / NODE_TYPE_USER nodes.
 * @param out Output stream.
 */
static void ast_dump_type_list(const ASTNodeList *types, FILE *out) {
	fputc(':', out);
	if (types->count == 0) {
		fputs(val_type_to_str(UNDEF), out);
		return;
	}
	for (size_t i = 0; i < types->count; ++i) {
		const ASTNode *type = types->items[i];
		if (i > 0) {
			fputc('|', out);
		}
		if (type->type == NODE_TYPE_USER) {
			const char *name = dstring_get(&type->data_type.defined_type);
			fputs(name ? name : "<null>", out);
		} else {
			fputs(val_type_to_str(type->data_type.orig_type), out);
		}
	}
}

void ast_dump_sexpr(const ASTNode *node, FILE *out) { // add new nodes into this thing????
	if (out == NULL) {
		return;
	}
	if (node == NULL) {
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
		case NODE_ASSIGNMENT:
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
			fprintf(
				out, "(%s %s", node->var_decl.is_const ? "const" : "var", name ? name : "<null>");
			ast_dump_type_list(&node->var_decl.type_name, out);
			fputc(' ', out);
			ast_dump_sexpr(node->var_decl.init_expr, out);
			fputc(')', out);
			break;
		}

		case NODE_MATCH:
		case NODE_CASE:
		case NODE_TYPE_ORIG:
		case NODE_TYPE_USER:
		case NODE_CLASS_DECL:
		case NODE_UNIF:
			break;

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
			ast_dump_nodelist(&node->block.stmts, out);
			fputc(')', out);
			break;

		case NODE_IF:
			fputs("(if ", out);
			ast_dump_sexpr(node->if_else_stmt.cond, out);
			fputc(' ', out);
			ast_dump_sexpr(node->if_else_stmt.body, out);
			ast_dump_nodelist(&node->if_else_stmt.next_conds, out);
			fputc(')', out);
			break;

		case NODE_ELSE:
			fputs("(else", out);
			if (node->if_else_stmt.cond != NULL) {
				fputc(' ', out);
				ast_dump_sexpr(node->if_else_stmt.cond, out);
			}
			fputc(' ', out);
			ast_dump_sexpr(node->if_else_stmt.body, out);
			ast_dump_nodelist(&node->if_else_stmt.next_conds, out);
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

		case NODE_FUNC_PARAM: {
			const char *name = dstring_get(&node->func_param.param_name);
			fprintf(out, "(param %s", name ? name : "<null>");
			ast_dump_type_list(&node->func_param.param_type, out);
			fputc(')', out);
			break;
		}

		case NODE_FUNC_WINDOW:
			fputs("(params", out);
			ast_dump_nodelist(&node->param_window.params, out);
			fputc(')', out);
			break;

		case NODE_FUNC_DECL: {
			const char *name = dstring_get(&node->func_decl.func_name);
			fprintf(out, "(def %s", name ? name : "<null>");
			ast_dump_nodelist(&node->func_decl.param_windows, out);
			ast_dump_type_list(&node->func_decl.return_type, out);
			fputc(' ', out);
			ast_dump_sexpr(node->func_decl.body, out);
			fputc(')', out);
			break;
		}

		case NODE_EXT_DECL: {
			const char *name = dstring_get(&node->ext_decl.param_name);
			fprintf(out, "(extension %s", name ? name : "<null>");
			ast_dump_type_list(&node->ext_decl.type_name, out);
			ast_dump_nodelist(&node->ext_decl.funcs, out);
			fputc(')', out);
			break;
		}

		case NODE_FOR: {
			const char *id = dstring_get(&node->for_stmt.gen_id);
			fprintf(out, "(for %s ", id ? id : "<null>");
			ast_dump_sexpr(node->for_stmt.gen_start, out);
			fputc(' ', out);
			ast_dump_sexpr(node->for_stmt.gen_end, out);
			if (node->for_stmt.gen_step != NULL) {
				fputc(' ', out);
				ast_dump_sexpr(node->for_stmt.gen_step, out);
			}
			ast_dump_nodelist(&node->for_stmt.guards, out);
			fputc(' ', out);
			ast_dump_sexpr(node->for_stmt.body, out);
			fputc(')', out);
			break;
		}
	}
}

void ast_set_type(ASTNode *node, ValType type) {
	if (node != NULL) {
		node->expr_types = type;
	}
}

/* --- Recursive destructor --- */

void ast_destroy(ASTNode *node) {
	if (node == NULL) {
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
			ast_nodelist_destroy(&node->var_decl.type_name);
			ast_destroy(node->var_decl.init_expr);
			break;

		case NODE_ASSIGNMENT:
			break;

		case NODE_CALL:
			dstring_free(&node->call.func_name);
			ast_destroy(node->call.target);
			ast_nodelist_destroy(&node->call.args);
			break;

		case NODE_BLOCK:
			ast_nodelist_destroy(&node->block.stmts);
			break;

		case NODE_FUNC_WINDOW:
			ast_nodelist_destroy(&node->param_window.params);
			break;

		case NODE_FUNC_PARAM:
			ast_nodelist_destroy(&node->func_param.param_type);
			dstring_free(&node->func_param.param_name);
			break;

		case NODE_FUNC_DECL:
			dstring_free(&node->func_decl.func_name);
			ast_destroy(node->func_decl.body);
			ast_nodelist_destroy(&node->func_decl.return_type);
			ast_nodelist_destroy(&node->func_decl.param_windows);
			break;

		case NODE_EXT_DECL:
			dstring_free(&node->ext_decl.param_name);
			ast_nodelist_destroy(&node->ext_decl.type_name);
			ast_nodelist_destroy(&node->ext_decl.funcs);
			break;

		case NODE_CLASS_DECL:
			dstring_free(&node->data_class.class_name);
			ast_destroy(node->data_class.param_list);
			break;

		case NODE_FOR:
			dstring_free(&node->for_stmt.gen_id);
			ast_destroy(node->for_stmt.gen_start);
			ast_destroy(node->for_stmt.gen_end);
			ast_destroy(node->for_stmt.gen_step);
			ast_destroy(node->for_stmt.body);
			ast_nodelist_destroy(&node->for_stmt.guards);
			break;

		case NODE_TYPE_ORIG:
			break;
		case NODE_TYPE_USER:
			dstring_free(&node->data_type.defined_type);
			break;

		case NODE_MATCH:
			ast_destroy(node->match.switch_expr);
			ast_nodelist_destroy(&node->match.cases);
			break;

		case NODE_CASE:
			ast_destroy(node->case_node.case_member);
			ast_destroy(node->case_node.guard);
			ast_destroy(node->case_node.body_expr);
			ast_destroy(node->case_node.type);
			break;

		case NODE_UNIF:
			dstring_free(&node->type_unif.unif_name);
			ast_nodelist_destroy(&node->type_unif.types);
			break;

		case NODE_IF:
		case NODE_ELSE:
			ast_destroy(node->if_else_stmt.cond);
			ast_destroy(node->if_else_stmt.body);
			ast_nodelist_destroy(&node->if_else_stmt.next_conds);
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
