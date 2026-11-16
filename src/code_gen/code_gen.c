/**
 * @project IFJ26 Compiler
 * @file code_gen.c
 * @brief Code gen module implementation for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#include "code_gen.h"
#include "code_gen_buildins.h"
#include <stdio.h>
#include <string.h>

static size_t label_id = 0;

static void print_escaped(const char *str) {
	for (const char *c = (const char *)str; *c; c++) {
		if (*c <= 32 || *c == 35 || *c == 92)
			printf("\\%03d", *c);
		else
			putchar(*c);
	}
}

static size_t bigger_size(const size_t x, const size_t y) {
	return x > y ? x : y;
}

static size_t max_sym_id(const ASTNode *node) {
	if (!node)
		return 0;

	size_t biggest = 0;
	switch (node->type) {
		case NODE_BLOCK:
			for (size_t i = 0; i < node->block.stmts.count; i++) {
				const ASTNode *stmt = node->block.stmts.items[i];
				biggest = bigger_size(max_sym_id(stmt), biggest);
			}
			return biggest;
		case NODE_VAR_DECL:
			return node->sym_id;
		case NODE_IF:
		case NODE_ELSE:
			biggest = bigger_size(max_sym_id(node->if_else_stmt.body), biggest);
			for (size_t i = 0; i < node->if_else_stmt.next_conds.count; i++) {
				const ASTNode *branch = node->if_else_stmt.next_conds.items[i];
				biggest = bigger_size(max_sym_id(branch), biggest);
			}
			return biggest;
		case NODE_WHILE:
			biggest = bigger_size(max_sym_id(node->while_stmt.body), biggest);
			return biggest;
		default:
			return 0;
	}
}

static bool is_expression(const ASTNode *node) {
	switch (node->type) {
		case NODE_BINARY_OP:
			if (node->op.op == OP_ASSIGN)
				return false;
		case NODE_INT_LITERAL:
		case NODE_FLOAT_LITERAL:
		case NODE_STRING_LITERAL:
		case NODE_NULL_LITERAL:
		case NODE_CALL:
		case NODE_UNARY_OP:
		case NODE_IDENTIFIER:
			return true;
		default:
			return false;
	}
	return false;
}

static bool is_builtin(const char *name) {
	// builtin_names from semantic register_builtin_function in fregister.c
	static const char *builtin_names[] = {"print", "readLine", "readInt", "readDouble", "toStr",
		"toInt", "strLength", "subStr", "ord", "chr"};

	for (size_t i = 0; i < sizeof(builtin_names) / sizeof(builtin_names[0]); i++) {
		if (strcmp(name, builtin_names[i]) == 0)
			return true;
	}

	return false;
}

Error gen_run(const ASTNode *outermost_node) {
	if (!outermost_node || outermost_node->type != NODE_BLOCK)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "The top most node must be a block");

	printf(".IFJcode26\n");
	printf("DEFVAR GF@throwaway\n");
	printf("JUMP $main\n");

	read_line();
	read_int();
	read_double();
	print();
	to_str();
	to_int();
	str_length();
	sub_str();
	ord();
	chr();

	const ASTNodeList *stmts = &outermost_node->block.stmts;
	ASTNode *main_func = NULL;
	for (size_t i = 0; i < stmts->count; i++) {
		ASTNode *node = stmts->items[i];

		if (node->type != NODE_FUNC_DECL)
			continue; // EXTKW

		if (node->func_decl.is_main) {
			if (!main_func) // Semantic can't return AST with more than one main,
							// I'm just making sure ':D
				main_func = node;
			else
				return ERROR_EMIT(
					ERR_INTERNAL, node->line, node->col, "Multiple main functions defined");
		}

		if (!gen_function(node))
			return ERROR_EMIT(ERR_INTERNAL, node->line, node->col, "Function gen_function failed");
	}

	// Again, I'm just making sure that semantic didn't mess up somewhere
	if (!main_func)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "No main function");

	printf("LABEL $main\n");
	printf("CREATEFRAME\n");
	printf("CALL $func_%s\n", dstring_get(&main_func->func_decl.func_name));
	printf("EXIT int@0\n");
	return ERROR_OK();
}

bool gen_function(ASTNode *func) {
	printf("LABEL $func_%s\n", dstring_get(&func->func_decl.func_name));
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("MOVE LF@%%retval nil@nil\n");

	size_t window_count = func->func_decl.param_windows.count;
	ASTNode *window = (window_count > 0) ? func->func_decl.param_windows.items[0] : NULL;
	size_t param_count = window ? window->param_window.params.count : 0;
	size_t var_count = bigger_size(param_count, max_sym_id(func->func_decl.body));

	for (size_t i = 1; i <= var_count; i++)
		printf("DEFVAR LF@v%zu\n", i);

	for (size_t i = 0; i < param_count; i++) {
		ASTNode *param = window->param_window.params.items[i];
		printf("MOVE LF@v%zu LF@%%arg%zu\n", param->sym_id, i + 1);
	}

	ASTNode *last_stmt = NULL;
	if (func->func_decl.body->type == NODE_BLOCK) {
		ASTNodeList *stmts = &func->func_decl.body->block.stmts;
		if (stmts->count > 0) {
			// Skip the last stmt and then save it
			for (size_t i = 0; i + 1 < stmts->count; i++) {
				ASTNode *stmt = stmts->items[i];
				if (!gen_stmt(stmt))
					return false;
			}
			last_stmt = stmts->items[stmts->count - 1];
		}
	} else
		last_stmt = func->func_decl.body;

	if (last_stmt) {
		if (is_expression(last_stmt) && func->expr_types != UNIT) {
			if (!gen_expr(last_stmt))
				return false;
			printf("POPS LF@%%retval\n");
		} else if (!gen_stmt(last_stmt))
			return false;
	}
	printf("POPFRAME\n");
	printf("RETURN\n");
	return true;
}

bool gen_binary_op_stack(OpType op, ValType result_type) {
	switch (op) {
		case OP_ADD:
			printf("ADDS\n");
			break;
		case OP_SUB:
			printf("SUBS\n");
			break;
		case OP_MUL:
			printf("MULS\n");
			break;
		case OP_DIV:
			if (result_type == INT)
				printf("IDIVS\n");
			else if (result_type == DOUBLE)
				printf("DIVS\n");
			else
				return false;
			break;
		case OP_LT:
			printf("LTS\n");
			break;
		case OP_GT:
			printf("GTS\n");
			break;
		case OP_EQ:
			printf("EQS\n");
			break;
		case OP_AND:
			printf("ANDS\n");
			break;
		case OP_OR:
			printf("ORS\n");
			break;
		case OP_NOT:
			printf("NOTS\n");
			break;
		default: // OP_CONCAT, OP_ASSIGN, OP_NEQ, OP_LTE, OP_GTE, OP_UNARY_MINUS, OP_UNARY_PLUS
			return false;
	}
	return true;
}

bool gen_expr(const ASTNode *exp) {
	if (!exp)
		return false;

	bool passed = true;
	switch (exp->type) {
		case NODE_BINARY_OP:
			if (!gen_expr(exp->op.left))
				return false;
			if (!gen_expr(exp->op.right))
				return false;
			if (!gen_binary_op_stack(exp->op.op, exp->expr_types))
				passed = false;
			break;
		case NODE_INT_LITERAL:
			printf("PUSHS int@%ld\n", exp->int_val);
			break;
		case NODE_FLOAT_LITERAL:
			printf("PUSHS float@%a\n", exp->float_val);
			break;
		case NODE_STRING_LITERAL:
			printf("PUSHS string@");
			print_escaped(dstring_get(&exp->str_val));
			printf("\n");
			break;
		case NODE_NULL_LITERAL:
			printf("PUSHS nil@nil\n");
			break;
		case NODE_IDENTIFIER:
			printf("PUSHS LF@v%zu\n", exp->sym_id);
			break;
		case NODE_CALL: {
			const char *func_name = dstring_get(&exp->call.func_name);
			const ASTNodeList *params = NULL;
			size_t params_count = 0;

			if (exp->call.args.count > 0) {
				params = &exp->call.args.items[0]->param_window.params;
				params_count = params->count;

				for (size_t i = 0; i < params_count; i++) {
					ASTNode *param = params->items[i];
					if (!gen_expr(param))
						return false;
				}
			}

			printf("CREATEFRAME\n");
			for (size_t i = 1; i <= params_count; i++)
				printf("DEFVAR TF@%%arg%zu\n", i);
			for (size_t i = params_count; i > 0; i--)
				printf("POPS TF@%%arg%zu\n", i);

			if (is_builtin(func_name))
				printf("CALL $built_in_%s\n", func_name);
			else
				printf("CALL $func_%s\n", func_name);

			printf("PUSHS TF@%%retval\n");
			break;
		}
		default:
			passed = false;
			break;
	}

	if (exp->to_double)
		printf("INT2FLOATS\n");

	return passed;
}

static bool gen_if(const ASTNode *node) {
	if (!node || node->type != NODE_IF)
		return false;

	size_t id = label_id++;
	size_t if_count = node->if_else_stmt.next_conds.count;
	for (size_t i = 0; i <= if_count; i++) {
		const ASTNode *branch;
		if (i == 0)
			branch = node;
		else
			branch = node->if_else_stmt.next_conds.items[i - 1];

		// Double check (same in semantic)
		if (!branch || (i > 0 && branch->type != NODE_ELSE))
			return false;

		if (branch->if_else_stmt.cond != NULL) {
			if (!gen_expr(branch->if_else_stmt.cond))
				return false;
			printf("PUSHS bool@false\n");
			printf("JUMPIFEQS $if_%zu_%zu\n", id, i + 1);
		}

		if (!gen_stmt(branch->if_else_stmt.body))
			return false;

		if (branch->if_else_stmt.cond != NULL) {
			printf("JUMP $if_%zu_end\n", id);
			printf("LABEL $if_%zu_%zu\n", id, i + 1);
		}
	}

	printf("LABEL $if_%zu_end\n", id);

	return true;
}

bool gen_stmt(const ASTNode *stmt) {
	if (!stmt)
		return false;

	switch (stmt->type) {
		case NODE_VAR_DECL:
			if (!gen_expr(stmt->var_decl.init_expr))
				return false;
			printf("POPS LF@v%zu\n", stmt->sym_id);
			return true;
		case NODE_BINARY_OP:
			if (stmt->op.op == OP_ASSIGN) {
				if (!gen_expr(stmt->op.right))
					return false;
				printf("POPS LF@v%zu\n", stmt->op.left->sym_id);
				return true;
			}
			// expressions like 1 + 5 without assignment
			if (!gen_expr(stmt))
				return false;
			printf("POPS GF@throwaway\n");
			return true;
		case NODE_INT_LITERAL:
		case NODE_FLOAT_LITERAL:
		case NODE_STRING_LITERAL:
		case NODE_NULL_LITERAL:
		case NODE_CALL:
		case NODE_UNARY_OP:
		case NODE_IDENTIFIER:
			if (!gen_expr(stmt))
				return false;
			printf("POPS GF@throwaway\n");
			break;
		case NODE_BLOCK:
			for (size_t i = 0; i < stmt->block.stmts.count; i++) {
				ASTNode *block_stmt = stmt->block.stmts.items[i];
				if (!gen_stmt(block_stmt))
					return false;
			}
			break;
		case NODE_RETURN:
			if (stmt->return_stmt.expr) {
				if (!gen_expr(stmt->return_stmt.expr))
					return false;
				printf("POPS LF@%%retval\n");
			}
			printf("POPFRAME\n");
			printf("RETURN\n");
			return true;
		case NODE_IF:
			return gen_if(stmt);
		default:
			return false; // if, while, return, for
	}
	return true;
}