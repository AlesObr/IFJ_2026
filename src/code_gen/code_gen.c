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
	for (const unsigned char *c = (const unsigned char *)str; *c; c++) {
		if (*c <= 32 || *c == 35 || *c == 92)
			printf("\\%03d", *c);
		else
			putchar(*c);
	}
}

static size_t bigger_size(const size_t x, const size_t y) {
	return x > y ? x : y;
}

static size_t max_sym_id(const ASTNode *node, bool emit) {
	if (!node)
		return 0;

	size_t biggest = 0;
	switch (node->type) {
		case NODE_BLOCK:
			for (size_t i = 0; i < node->block.stmts.count; i++) {
				const ASTNode *stmt = node->block.stmts.items[i];
				biggest = bigger_size(max_sym_id(stmt, emit), biggest);
			}
			return biggest;
		case NODE_VAR_DECL:
			biggest = bigger_size(node->sym_id, max_sym_id(node->var_decl.init_expr, emit));
			return biggest;
		case NODE_IF:
		case NODE_ELSE:
			biggest = bigger_size(max_sym_id(node->if_else_stmt.body, emit), biggest);
			for (size_t i = 0; i < node->if_else_stmt.next_conds.count; i++) {
				const ASTNode *branch = node->if_else_stmt.next_conds.items[i];
				biggest = bigger_size(max_sym_id(branch, emit), biggest);
			}
			biggest = bigger_size(max_sym_id(node->if_else_stmt.cond, emit), biggest);
			return biggest;
		case NODE_WHILE:
			biggest = bigger_size(max_sym_id(node->while_stmt.body, emit), biggest);
			biggest = bigger_size(max_sym_id(node->while_stmt.cond, emit), biggest);
			return biggest;
		case NODE_RETURN:
			biggest = bigger_size(max_sym_id(node->return_stmt.expr, emit), biggest);
			return biggest;
		case NODE_BINARY_OP:
			biggest = bigger_size(max_sym_id(node->op.left, emit), biggest);
			biggest = bigger_size(max_sym_id(node->op.right, emit), biggest);
			return biggest;
		case NODE_UNARY_OP:
			biggest = bigger_size(max_sym_id(node->op.left, emit), biggest);
			return biggest;
		case NODE_CALL: {
			if (node->call.args.count == 0)
				return 0;
			const ASTNodeList *args = &node->call.args.items[0]->param_window.params;
			for (size_t i = 0; i < args->count; i++) {
				const ASTNode *arg = args->items[i];
				biggest = bigger_size(max_sym_id(arg, emit), biggest);
			}
			return biggest;
		}
		case NODE_FOR: {
			if (emit) {
				printf("DEFVAR LF@v%zu_end\n", node->sym_id);
				printf("DEFVAR LF@v%zu_step\n", node->sym_id);
			}

			biggest = node->sym_id;
			biggest = bigger_size(max_sym_id(node->for_stmt.gen_start, emit), biggest);
			biggest = bigger_size(max_sym_id(node->for_stmt.gen_end, emit), biggest);
			if (node->for_stmt.gen_step)
				biggest = bigger_size(max_sym_id(node->for_stmt.gen_step, emit), biggest);
			biggest = bigger_size(max_sym_id(node->for_stmt.body, emit), biggest);

			const ASTNodeList *guards = &node->for_stmt.guards;
			for (size_t i = 0; i < guards->count; i++) {
				const ASTNode *guard = guards->items[i];
				biggest = bigger_size(max_sym_id(guard, emit), biggest);
			}
			return biggest;
		}
		default:
			return 0;
	}
	return 0;
}

static bool is_expression(const ASTNode *node) {
	switch (node->type) {
		case NODE_BINARY_OP:
			// An assignment is a statement, any other binary operation is an expression
			return node->op.op != OP_ASSIGN;
		case NODE_INT_LITERAL:
		case NODE_FLOAT_LITERAL:
		case NODE_STRING_LITERAL:
		case NODE_NULL_LITERAL:
		case NODE_BOOL_LITERAL:
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
		"toInt", "strLength", "subStr", "ord", "chr", "readBoolean"};

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
	printf("DEFVAR GF@%%t1\n");
	printf("DEFVAR GF@%%t2\n");
	printf("JUMP $main\n");

	read_line();
	read_int();
	read_double();
	read_boolean();
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

		Error err = gen_function(node);
		if (ERROR_IS_ERR(err))
			return err;
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

static Error gen_val(const ASTNode *node) {
	if (node == NULL)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters for gen_val");

	switch (node->type) {
		case NODE_BLOCK: {
			const ASTNodeList *stmts = &node->block.stmts;
			Error err = ERROR_OK();

			if (stmts->count == 0) {
				printf("PUSHS nil@nil\n");
				break;
			}

			for (size_t i = 0; i < stmts->count - 1; i++) {
				err = gen_stmt(stmts->items[i]);
				if (ERROR_IS_ERR(err))
					return err;
			}

			err = gen_val(stmts->items[stmts->count - 1]);
			if (ERROR_IS_ERR(err))
				return err;
			break;
		}
		case NODE_IF: {
			Error err = gen_if(node, true);
			if (ERROR_IS_ERR(err))
				return err;
			if (node->to_double)
				printf("INT2FLOATS\n");
			break;
		}
		case NODE_VAR_DECL: {
			Error err = gen_stmt(node);
			if (ERROR_IS_ERR(err))
				return err;
			printf("PUSHS nil@nil\n");
			break;
		}
		default: { // expression and assignments
			if (is_expression(node)) {
				Error err = gen_expr(node);
				if (ERROR_IS_ERR(err))
					return err;
				break;
			}

			// assignment and the rest
			Error err = gen_stmt(node);
			if (ERROR_IS_ERR(err))
				return err;
			printf("PUSHS nil@nil\n");
			break;
		}
	}

	return ERROR_OK();
}

Error gen_if(const ASTNode *node, const bool as_val) {
	if (!node || node->type != NODE_IF)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to gen_if");

	size_t id = label_id++;
	size_t if_count = node->if_else_stmt.next_conds.count;
	Error err = ERROR_OK();
	for (size_t i = 0; i <= if_count; i++) {
		const ASTNode *branch;
		if (i == 0)
			branch = node;
		else
			branch = node->if_else_stmt.next_conds.items[i - 1];

		// Double check (same in semantic)
		if (!branch || (i > 0 && branch->type != NODE_ELSE))
			return ERROR_EMIT(ERR_INTERNAL, node->line, node->col, "Invalid branch");

		if (branch->if_else_stmt.cond != NULL) {
			err = gen_expr(branch->if_else_stmt.cond);
			if (ERROR_IS_ERR(err))
				return err;
			printf("PUSHS bool@false\n");
			printf("JUMPIFEQS $if_%zu_%zu\n", id, i + 1);
		}

		if (as_val) {
			err = gen_val(branch->if_else_stmt.body);
			if (ERROR_IS_ERR(err))
				return err;
		} else {
			err = gen_stmt(branch->if_else_stmt.body);
			if (ERROR_IS_ERR(err))
				return err;
		}

		if (branch->if_else_stmt.cond != NULL) {
			printf("JUMP $if_%zu_end\n", id);
			printf("LABEL $if_%zu_%zu\n", id, i + 1);
		}
	}

	printf("LABEL $if_%zu_end\n", id);

	return ERROR_OK();
}

Error gen_function(ASTNode *func) {
	printf("LABEL $func_%s\n", dstring_get(&func->func_decl.func_name));
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@%%retval\n");
	printf("MOVE LF@%%retval nil@nil\n");

	size_t window_count = func->func_decl.param_windows.count;
	ASTNode *window = (window_count > 0) ? func->func_decl.param_windows.items[0] : NULL;
	size_t param_count = window ? window->param_window.params.count : 0;
	size_t var_count = bigger_size(param_count, max_sym_id(func->func_decl.body, false));
	Error err = ERROR_OK();

	for (size_t i = 1; i <= var_count; i++)
		printf("DEFVAR LF@v%zu\n", i);

	(void)max_sym_id(func->func_decl.body, true);

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
				err = gen_stmt(stmt);
				if (ERROR_IS_ERR(err))
					return err;
			}
			last_stmt = stmts->items[stmts->count - 1];
		}
	} else
		last_stmt = func->func_decl.body;

	if (last_stmt) {
		if (is_expression(last_stmt) && func->expr_types != UNIT) {
			err = gen_expr(last_stmt);
			if (ERROR_IS_ERR(err))
				return err;
			printf("POPS LF@%%retval\n");
		} else if (last_stmt->type == NODE_IF && func->expr_types != UNIT) {
			err = gen_if(last_stmt, true);
			if (ERROR_IS_ERR(err))
				return err;
			printf("POPS LF@%%retval\n");
		} else {
			err = gen_stmt(last_stmt);
			if (ERROR_IS_ERR(err))
				return err;
		}
	}

	printf("POPFRAME\n");
	printf("RETURN\n");
	return ERROR_OK();
}

Error gen_binary_op_stack(const ASTNode *exp) {
	switch (exp->op.op) {
		case OP_ADD:
			if (exp->expr_types == STRING) {
				printf("POPS GF@%%t2\n");
				printf("POPS GF@%%t1\n");
				printf("CONCAT GF@%%t1 GF@%%t1 GF@%%t2\n");
				printf("PUSHS GF@%%t1\n");
				break;
			}
			printf("ADDS\n");
			break;
		case OP_SUB:
			printf("SUBS\n");
			break;
		case OP_MUL:
			printf("MULS\n");
			break;
		case OP_DIV:
			if (exp->expr_types == INT)
				printf("IDIVS\n");
			else if (exp->expr_types == DOUBLE)
				printf("DIVS\n");
			else
				return ERROR_EMIT(ERR_INTERNAL, exp->line, exp->col,
					"result_type is not the correct type for division, expected INT or DOUBLE");
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
		case OP_NEQ:
			printf("EQS\n");
			printf("NOTS\n");
			break;
		case OP_LTE:
			printf("GTS\n");
			printf("NOTS\n");
			break;
		case OP_GTE:
			printf("LTS\n");
			printf("NOTS\n");
			break;
		default: { // OP_CONCAT, OP_ASSIGN, OP_NEQ, OP_LTE, OP_GTE, OP_UNARY_MINUS, OP_UNARY_PLUS
			static char msg[64];
			snprintf(msg, sizeof(msg), "Unsupported operand node type %d", exp->op.op);
			return ERROR_EMIT(ERR_INTERNAL, exp->line, exp->col, msg);
		}
	}

	return ERROR_OK();
}

Error gen_expr(const ASTNode *exp) {
	if (!exp)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to gen_expr");

	Error err = ERROR_OK();
	switch (exp->type) {
		case NODE_BINARY_OP: {
			if (exp->op.op == OP_AND) {
				size_t id = label_id++;
				err = gen_expr(exp->op.left);
				if (ERROR_IS_ERR(err))
					return err;
				printf("PUSHS bool@false\n");
				printf("JUMPIFEQS $and_%zu_short\n", id);

				err = gen_expr(exp->op.right);
				if (ERROR_IS_ERR(err))
					return err;
				printf("JUMP $and_%zu_end\n", id);
				printf("LABEL $and_%zu_short\n", id);
				printf("PUSHS bool@false\n");
				printf("LABEL $and_%zu_end\n", id);
				break;
			}
			if (exp->op.op == OP_OR) {
				size_t id = label_id++;
				err = gen_expr(exp->op.left);
				if (ERROR_IS_ERR(err))
					return err;
				printf("PUSHS bool@true\n");
				printf("JUMPIFEQS $or_%zu_short\n", id);

				err = gen_expr(exp->op.right);
				if (ERROR_IS_ERR(err))
					return err;
				printf("JUMP $or_%zu_end\n", id);
				printf("LABEL $or_%zu_short\n", id);
				printf("PUSHS bool@true\n");
				printf("LABEL $or_%zu_end\n", id);
				break;
			}
			err = gen_expr(exp->op.left);
			if (ERROR_IS_ERR(err))
				return err;
			err = gen_expr(exp->op.right);
			if (ERROR_IS_ERR(err))
				return err;
			err = gen_binary_op_stack(exp);
			if (ERROR_IS_ERR(err))
				return err;
			break;
		}
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
		case NODE_BOOL_LITERAL:
			if (exp->bool_val)
				printf("PUSHS bool@true\n");
			else
				printf("PUSHS bool@false\n");
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
					err = gen_expr(param);
					if (ERROR_IS_ERR(err))
						return err;
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
		case NODE_UNARY_OP: {
			switch (exp->op.op) {
				case OP_UNARY_MINUS:
					if (exp->expr_types == INT)
						printf("PUSHS int@0\n");
					else if (exp->expr_types == DOUBLE)
						printf("PUSHS float@0x0p+0\n");
					else
						return ERROR_EMIT(ERR_INTERNAL, exp->line, exp->col,
							"Unsupported opperatins"); // Double check ;P

					err = gen_expr(exp->op.left);
					if (ERROR_IS_ERR(err))
						return err;

					printf("SUBS\n");
					break;
				case OP_NOT:
					err = gen_expr(exp->op.left);
					if (ERROR_IS_ERR(err))
						return err;

					printf("NOTS\n");
					break;
				case OP_UNARY_PLUS:
					err = gen_expr(exp->op.left);
					if (ERROR_IS_ERR(err))
						return err;
					break;
				default:
					return ERROR_EMIT(
						ERR_INTERNAL, exp->line, exp->col, "Unsupported NODE_UNARY_OP node");
			}
			break;
		}
		case NODE_IF:
			err = gen_if(exp, true);
			if (ERROR_IS_ERR(err))
				return err;
			break;
		default: {
			static char msg[64];
			snprintf(msg, sizeof(msg), "Unsupported expression node type %d", exp->type);
			return ERROR_EMIT(ERR_INTERNAL, exp->line, exp->col, msg);
		}
	}

	if (exp->to_double)
		printf("INT2FLOATS\n");

	return ERROR_OK();
}

static Error gen_while(const ASTNode *node) {
	if (!node || node->type != NODE_WHILE)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to gen_while");

	size_t id = label_id++;

	printf("LABEL $while_%zu_start\n", id);

	Error err = gen_expr(node->while_stmt.cond);
	if (ERROR_IS_ERR(err))
		return err;

	printf("PUSHS bool@false\n");
	printf("JUMPIFEQS $while_%zu_end\n", id);

	err = gen_stmt(node->while_stmt.body);
	if (ERROR_IS_ERR(err))
		return err;

	printf("JUMP $while_%zu_start\n", id);
	printf("LABEL $while_%zu_end\n", id);

	return ERROR_OK();
}

static Error gen_for(const ASTNode *node) {
	if (!node || node->type != NODE_FOR)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters for gen_for");
	size_t id = label_id++;

	Error err = gen_expr(node->for_stmt.gen_start);
	if (ERROR_IS_ERR(err))
		return err;
	printf("POPS LF@v%zu\n", node->sym_id);

	err = gen_expr(node->for_stmt.gen_end);
	if (ERROR_IS_ERR(err))
		return err;
	printf("POPS LF@v%zu_end\n", node->sym_id);

	if (node->for_stmt.gen_step) {
		err = gen_expr(node->for_stmt.gen_step);
		if (ERROR_IS_ERR(err))
			return err;

		printf("POPS LF@v%zu_step\n", node->sym_id);
		if (node->for_stmt.gen_step->type != NODE_INT_LITERAL) {
			printf("JUMPIFNEQ $for_%zu_ok LF@v%zu_step int@0\n", id, node->sym_id);
			printf("EXIT int@40\n");
			printf("LABEL $for_%zu_ok\n", id);
		}
	} else
		printf("MOVE LF@v%zu_step int@1\n", node->sym_id);

	printf("LABEL $for_%zu_cond\n", id);

	printf("PUSHS LF@v%zu_step\n", node->sym_id);
	printf("PUSHS int@0\n");
	printf("GTS\n");
	printf("PUSHS bool@false\n");

	printf("JUMPIFEQS $for_%zu_neg\n", id);

	printf("PUSHS LF@v%zu\n", node->sym_id);
	printf("PUSHS LF@v%zu_end\n", node->sym_id);
	if (node->for_stmt.last_in) {
		printf("GTS\n");
		printf("NOTS\n");
	} else
		printf("LTS\n");
	printf("JUMP $for_%zu_chk\n", id);

	printf("LABEL $for_%zu_neg\n", id);
	printf("PUSHS LF@v%zu\n", node->sym_id);
	printf("PUSHS LF@v%zu_end\n", node->sym_id);
	if (node->for_stmt.last_in) {
		printf("LTS\n");
		printf("NOTS\n");
	} else
		printf("GTS\n");
	printf("LABEL $for_%zu_chk\n", id);

	printf("PUSHS bool@false\n");
	printf("JUMPIFEQS $for_%zu_end\n", id);

	// Guard
	const ASTNodeList *guards = &node->for_stmt.guards;
	for (size_t i = 0; i < guards->count; i++) {
		const ASTNode *guard = guards->items[i];
		err = gen_expr(guard);
		if (ERROR_IS_ERR(err))
			return err;

		printf("PUSHS bool@false\n");
		printf("JUMPIFEQS $for_%zu_next\n", id);
	}

	// Body
	err = gen_stmt(node->for_stmt.body);
	if (ERROR_IS_ERR(err))
		return err;

	printf("LABEL $for_%zu_next\n", id);
	printf("PUSHS LF@v%zu\n", node->sym_id);
	printf("PUSHS LF@v%zu_step\n", node->sym_id);
	printf("ADDS\n");
	printf("POPS LF@v%zu\n", node->sym_id);
	printf("JUMP $for_%zu_cond\n", id);

	printf("LABEL $for_%zu_end\n", id);

	return ERROR_OK();
}

Error gen_stmt(const ASTNode *stmt) {
	if (!stmt)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameter to gen_stmt");

	Error err = ERROR_OK();

	switch (stmt->type) {
		case NODE_VAR_DECL:
			err = gen_expr(stmt->var_decl.init_expr);
			if (ERROR_IS_ERR(err))
				return err;
			printf("POPS LF@v%zu\n", stmt->sym_id);
			break;
		case NODE_BINARY_OP:
			if (stmt->op.op == OP_ASSIGN) {
				err = gen_expr(stmt->op.right);
				if (ERROR_IS_ERR(err))
					return err;
				printf("POPS LF@v%zu\n", stmt->op.left->sym_id);
				break;
			}
			// expressions like 1 + 5 without assignment
			err = gen_expr(stmt);
			if (ERROR_IS_ERR(err))
				return err;
			printf("POPS GF@throwaway\n");
			break;
		case NODE_INT_LITERAL:
		case NODE_FLOAT_LITERAL:
		case NODE_STRING_LITERAL:
		case NODE_NULL_LITERAL:
		case NODE_BOOL_LITERAL:
		case NODE_CALL:
		case NODE_UNARY_OP:
		case NODE_IDENTIFIER:
			err = gen_expr(stmt);
			if (ERROR_IS_ERR(err))
				return err;
			printf("POPS GF@throwaway\n");
			break;
		case NODE_BLOCK:
			for (size_t i = 0; i < stmt->block.stmts.count; i++) {
				ASTNode *block_stmt = stmt->block.stmts.items[i];
				err = gen_stmt(block_stmt);
				if (ERROR_IS_ERR(err))
					return err;
			}
			break;
		case NODE_RETURN:
			if (stmt->return_stmt.expr) {
				err = gen_expr(stmt->return_stmt.expr);
				if (ERROR_IS_ERR(err))
					return err;
				printf("POPS LF@%%retval\n");
			}
			printf("POPFRAME\n");
			printf("RETURN\n");
			break;
		case NODE_IF:
			return gen_if(stmt, false);
		case NODE_WHILE:
			return gen_while(stmt);
		case NODE_FOR:
			return gen_for(stmt);
		default: {
			static char msg[64];
			snprintf(msg, sizeof(msg), "Unsupported statement node type %d", stmt->type);
			return ERROR_EMIT(ERR_INTERNAL, stmt->line, stmt->col, msg);
		}
	}
	return ERROR_OK();
}
