/**
 * @project IFJ26 Compiler
 * @file semantic.c
 * @brief Semantic module implementation for IFJ26 compiler.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#include "semantic.h"
#include "fregister.h"
#include "type_rules.h"
#include <stdio.h>
#include <string.h>

static void set_type(ASTNode *n, ValType t) {
	ast_set_type(n, t);
}

static void mark_convert(ASTNode *n) {
	n->to_double = true;
}

static bool booleans_enabled(void) {
	return true; // : BOOLTHEN
}

Error sem_run(ASTNode *outermost_node) {
	if (!outermost_node || outermost_node->type != NODE_BLOCK)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "The top most node must be a block");

	SemanticContext ctx;
	sem_ctx_init(&ctx);
	Error err = sem_analyze(&ctx, &outermost_node->block.stmts);
	sem_ctx_free(&ctx);
	return err;
}

void sem_ctx_init(SemanticContext *ctx) {
	scope_init(&ctx->scope_stack);
	ftable_init(&ctx->func_table);
	ctx->current_function = NULL;
	ctx->current_return_type = UNDEF;
}

void sem_ctx_free(SemanticContext *ctx) {
	scope_free(&ctx->scope_stack);
	ftable_free(&ctx->func_table);
}

Error sem_analyze(SemanticContext *ctx, const ASTNodeList *top_nodes) {
	if (!register_builtin_function(&ctx->func_table))
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Failed to register built-in functions");

	// Pass 1
	Error err = register_functions(&ctx->func_table, top_nodes);
	if (ERROR_IS_ERR(err))
		return err;

	// Pass 2
	for (size_t i = 0; i < top_nodes->count; i++) {
		ASTNode *node = top_nodes->items[i];

		if (node->type != NODE_FUNC_DECL)
			continue; // EXTKW

		FunctionEntry *en =
			ftable_lookup(&ctx->func_table, dstring_get(&node->func_decl.func_name));

		if (en && en->state == FUNC_NEW) {
			err = analyze_function(ctx, en);
			if (ERROR_IS_ERR(err))
				return err;
		}
	}
	return ERROR_OK();
}

Error analyze_expression(SemanticContext *ctx, ASTNode *expr, ValType *out) {
	if (!ctx || !expr || !out) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_expression");
	}

	switch (expr->type) {
		case NODE_INT_LITERAL:
			*out = INT;
			break;
		case NODE_FLOAT_LITERAL:
			*out = DOUBLE;
			break;
		case NODE_STRING_LITERAL:
			*out = STRING;
			break;
		case NODE_NULL_LITERAL:
			*out = UNIT;
			break;
		case NODE_BOOL_LITERAL:
			*out = BOOLEAN;
			break;
		case NODE_IDENTIFIER: {
			Symbol *sym = scope_lookup(&ctx->scope_stack, dstring_get(&expr->str_val));
			if (!sym) {
				return ERROR_EMIT(ERR_SEM_UNDEF, expr->line, expr->col, "Undefined variable");
			}
			*out = sym->type;
			expr->sym_id = sym->id;
			break;
		}
		case NODE_BINARY_OP: {
			ValType left_type, right_type;
			bool conv_left, conv_right;

			Error err = analyze_expression(ctx, expr->op.left, &left_type);
			if (ERROR_IS_ERR(err))
				return err;

			err = analyze_expression(ctx, expr->op.right, &right_type);
			if (ERROR_IS_ERR(err))
				return err;

			*out = binop_result(expr->op.op, left_type, right_type, &conv_left, &conv_right);
			if (*out == UNDEF)
				return ERROR_EMIT(
					ERR_SEM_TYPE, expr->line, expr->col, "Type error in binary operation");

			// Handle conversions
			if (conv_left)
				mark_convert(expr->op.left);
			if (conv_right)
				mark_convert(expr->op.right);
			break;
		}
		case NODE_UNARY_OP: {
			ValType operand_type;
			Error err = analyze_expression(ctx, expr->op.left, &operand_type);
			if (ERROR_IS_ERR(err))
				return err;

			*out = unop_result(expr->op.op, operand_type);
			if (*out == UNDEF)
				return ERROR_EMIT(
					ERR_SEM_TYPE, expr->line, expr->col, "Type error in unary operation");

			break;
		}
		case NODE_CALL: {
			Error err = analyze_call(ctx, expr, out);
			if (ERROR_IS_ERR(err))
				return err;
			break;
		}
		case NODE_IF: {
			bool returns = false;
			Error err = analyze_if(ctx, expr, out, &returns);
			if (ERROR_IS_ERR(err))
				return err;

			size_t if_count = expr->if_else_stmt.next_conds.count;
			if (if_count == 0)
				return ERROR_EMIT(
					ERR_SEM_TYPE, expr->line, expr->col, "If expression has to have a case");
			ASTNode *last_node = expr->if_else_stmt.next_conds.items[if_count - 1];

			if (last_node->type != NODE_ELSE || last_node->if_else_stmt.cond != NULL)
				return ERROR_EMIT(ERR_SEM_TYPE, expr->line, expr->col,
					"The if expression has to end with an else with no conditions");

			if (*out == UNDEF)
				return ERROR_EMIT(ERR_SEM_TYPE, expr->line, expr->col,
					"All branches need a matching return type");
			break;
		}
		default: { // Temporary default case for unsupported expression types
			static char msg[64];
			snprintf(msg, sizeof(msg), "Unsupported expression node type %d", expr->type);
			return ERROR_EMIT(ERR_INTERNAL, expr->line, expr->col, msg);
		}
	}

	set_type(expr, *out); // Record the type of the expression node
	return ERROR_OK();
}

Error analyze_var_decl(SemanticContext *ctx, ASTNode *node) {
	if (!ctx || !node || node->type != NODE_VAR_DECL) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_var_decl");
	}

	ValType init_type;
	Error err = analyze_expression(ctx, node->var_decl.init_expr, &init_type);
	if (ERROR_IS_ERR(err))
		return err;

	if (init_type == BOOLEAN && !booleans_enabled()) {
		return ERROR_EMIT(
			ERR_SEM_TYPE, node->line, node->col, "Cannot assign BOOLEAN type to variable");
	}

	ValType final_type;

	err = type_from_nodes(&node->var_decl.type_name, node->line, node->col, &final_type);
	if (ERROR_IS_ERR(err))
		return err;

	if (final_type != UNDEF) {
		bool conv_needed;
		if (!type_compatible(final_type, init_type, &conv_needed))
			return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col,
				"Declared type is not compatible with initializer type");
		if (conv_needed)
			mark_convert(node->var_decl.init_expr);
	} else {
		final_type = init_type;
	}

	Symbol *sym = NULL;
	ScopeResult sr = scope_insert(&ctx->scope_stack, node, final_type, &sym);
	// make it write sym_id onto the node sym->id
	if (sr == SCOPE_DUP) {
		return ERROR_EMIT(ERR_SEM_REDEF, node->line, node->col,
			"Variable name already defined in the current scope");
	} else if (sr == SCOPE_NOMEM) {
		return ERROR_EMIT(ERR_INTERNAL, node->line, node->col,
			"Out of memory while inserting variable into scope");
	}

	return ERROR_OK();
}

Error analyze_assignment(SemanticContext *ctx, ASTNode *node) {
	if (!ctx || !node || node->type != NODE_BINARY_OP || node->op.op != OP_ASSIGN) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_assignment");
	}

	ASTNode *target = node->op.left;
	ASTNode *value = node->op.right;
	if (!target || !value)
		return ERROR_EMIT(
			ERR_INTERNAL, node->line, node->col, "Assignment without target or value");

	if (target->type != NODE_IDENTIFIER)
		return ERROR_EMIT(ERR_SEM_ASSIGN, target->line, target->col,
			"Left-hand side of assignment must be a variable identifier");

	Symbol *sym = scope_lookup(&ctx->scope_stack, dstring_get(&target->str_val));
	if (!sym)
		return ERROR_EMIT(ERR_SEM_UNDEF, node->line, node->col, "Undefined variable in assignment");

	bool is_const = sym->is_const;
	bool conv_needed;
	ValType var_type = sym->type;
	ValType expr_type;
	target->sym_id = sym->id;

	if (is_const)
		return ERROR_EMIT(
			ERR_SEM_ASSIGN, node->line, node->col, "Cannot assign to a constant variable");

	Error err = analyze_expression(ctx, value, &expr_type);
	if (ERROR_IS_ERR(err))
		return err;

	if (expr_type == BOOLEAN && !booleans_enabled())
		return ERROR_EMIT(
			ERR_SEM_TYPE, node->line, node->col, "Cannot assign BOOLEAN type to variable");

	if (!type_compatible(var_type, expr_type, &conv_needed))
		return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col,
			"Assigned value type is not compatible with the variable type");

	if (conv_needed)
		mark_convert(value);

	return ERROR_OK();
}

// (x.foo() will be rejected), VALMEM, EXTKW
Error analyze_call(SemanticContext *ctx, ASTNode *call, ValType *out) {
	if (!ctx || !call || !out || call->type != NODE_CALL) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_call");
	}

	if (call->call.target != NULL) {
		return ERROR_EMIT(
			ERR_SEM_CALL, call->line, call->col, "Member function calls are not supported");
	}

	FunctionEntry *entry = ftable_lookup(&ctx->func_table, dstring_get(&call->call.func_name));
	if (!entry) {
		return ERROR_EMIT(ERR_SEM_UNDEF, call->line, call->col, "Undefined function in call");
	}

	size_t param_count = entry->param_count;
	ValType return_type = entry->return_type;
	ValType *param_types = entry->param_types;
	ASTNodeList *args = NULL;
	size_t arg_count = 0;

	if (call->call.args.count > 0) {
		ASTNode *window = call->call.args.items[0];
		if (window->type != NODE_FUNC_WINDOW)
			return ERROR_EMIT(ERR_INTERNAL, call->line, call->col,
				"Parameters window is not of type NODE_FUNC_WINDOW");
		args = &window->param_window.params;
		arg_count = args->count;
	}

	if (param_count != arg_count) {
		return ERROR_EMIT(ERR_SEM_CALL, call->line, call->col,
			"Argument count does not match function parameter count");
	}

	for (size_t i = 0; i < param_count; i++) {
		ValType arg_type;
		Error err = analyze_expression(ctx, args->items[i], &arg_type);
		if (ERROR_IS_ERR(err))
			return err;

		bool conv_needed;
		if (!type_compatible(param_types[i], arg_type, &conv_needed)) {
			return ERROR_EMIT(ERR_SEM_CALL, args->items[i]->line, args->items[i]->col,
				"Argument type does not match function parameter type");
		}
		if (conv_needed) {
			mark_convert(args->items[i]);
		}
	}

	if (return_type == UNDEF) {
		// Check if call function is declared
		if (!entry->decl)
			return ERROR_EMIT(
				ERR_INTERNAL, call->line, call->col, "Function has undefined return type");

		// Check if it's calling it self
		if (entry->state == FUNC_RUNNING)
			return ERROR_EMIT(ERR_SEM_TYPE, call->line, call->col,
				"Funcrion that is recursive must have its return type declared");

		if (entry->state == FUNC_NEW) {
			Error err = analyze_function(ctx, entry);
			if (ERROR_IS_ERR(err))
				return err;
		}
	}

	*out = entry->return_type;
	set_type(call, *out); // Record the type of the function call node
	return ERROR_OK();
}

Error analyze_block(
	SemanticContext *ctx, ASTNode *block, bool new_scope, ValType *out, bool *returns) {
	if (!ctx || !block || !out || !returns || block->type != NODE_BLOCK) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_block");
	}

	if (new_scope && !scope_push(&ctx->scope_stack))
		return ERROR_EMIT(ERR_INTERNAL, block->line, block->col, "Failed to push scope");

	// Go through each statement and check it
	ValType last_type = UNIT;
	bool node_returns = false;
	for (size_t i = 0; i < block->block.stmts.count; i++) {
		ASTNode *stmt = block->block.stmts.items[i];
		Error err = analyze_stmt(ctx, stmt, &last_type, &node_returns);
		if (ERROR_IS_ERR(err)) {
			if (new_scope)
				scope_pop(&ctx->scope_stack);
			return err;
		}
	}

	*out = last_type;
	*returns = node_returns;

	if (new_scope) {
		scope_pop(&ctx->scope_stack);
	}

	return ERROR_OK();
}

static Error analyze_return(SemanticContext *ctx, ASTNode *node, ValType *out, bool *returns) {
	if (!ctx || !node || !out || !returns || node->type != NODE_RETURN)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_return");
	if (!ctx->current_function)
		return ERROR_EMIT(ERR_SEM_MAIN, node->line, node->col, "Return is outside of a function");

	ValType return_type = ctx->current_return_type;
	ASTNode *value = node->return_stmt.expr;

	// Check if the function has a return type
	if (return_type == UNDEF)
		return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col,
			"Function that has a return also needs declared return type");

	// Check if the return returns smth
	if (!value) {
		if (return_type != UNIT)
			return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col,
				"Empty return a function which return type is not UNIT");
	} else {
		ValType type;

		Error err = analyze_expression(ctx, value, &type);
		if (ERROR_IS_ERR(err))
			return err;

		if (type == BOOLEAN && !booleans_enabled())
			return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col, "Cannot return a BOOLEAN value");

		if (return_type != UNIT) {
			bool conv;

			if (!type_compatible(return_type, type, &conv))
				return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col,
					"Returned value does not match with functions return type");

			if (conv)
				mark_convert(value);
		}
	}

	*out = UNIT;
	*returns = true;

	return ERROR_OK();
}

static Error analyze_body(SemanticContext *ctx, ASTNode *body, ValType *out, bool *returns) {
	if (!ctx || !body || !out || !returns)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_body");

	if (body->type == NODE_BLOCK)
		return analyze_block(ctx, body, true, out, returns);

	if (!scope_push(&ctx->scope_stack))
		return ERROR_EMIT(ERR_INTERNAL, body->line, body->col, "Out of memory when opening a scop");

	Error err = analyze_stmt(ctx, body, out, returns);
	scope_pop(&ctx->scope_stack);

	return err;
}

static Error analyze_condition(SemanticContext *ctx, ASTNode *cond) {
	if (!ctx || !cond)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_condition");

	ValType type;

	Error err = analyze_expression(ctx, cond, &type);
	if (ERROR_IS_ERR(err))
		return err;

	if (type != BOOLEAN)
		return ERROR_EMIT(
			ERR_SEM_TYPE, cond->line, cond->col, "Condition is not of a BOOLEAN expression");

	return ERROR_OK();
}

static Error analyze_if(SemanticContext *ctx, ASTNode *node, ValType *out, bool *returns) {
	if (!ctx || !node || !out || !returns || node->type != NODE_IF)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_if");

	bool all_return = true; // Every branch returns
	bool all_unit = true;	// Every branch has a type UNIT
	bool has_else = false;	// If the 'if' has else, if not it will be like a branch with ()
	ValType common_type = UNDEF;

	bool first_non_return = true;
	size_t if_count = node->if_else_stmt.next_conds.count;
	for (size_t i = 0; i <= if_count; i++) {
		ASTNode *branch;
		if (i == 0) {
			branch = node;
		} // The 'if' itself
		else {
			branch = node->if_else_stmt.next_conds.items[i - 1];
		}

		if (!branch || (i > 0 && branch->type != NODE_ELSE))
			return ERROR_EMIT(ERR_INTERNAL, node->line, node->col, "Invalid branch");

		if (branch->if_else_stmt.cond != NULL) {
			Error err = analyze_condition(ctx, branch->if_else_stmt.cond);
			if (ERROR_IS_ERR(err))
				return err;
		} else {
			has_else = true;
		}

		ValType type;
		bool ret;

		Error err = analyze_body(ctx, branch->if_else_stmt.body, &type, &ret);
		if (ERROR_IS_ERR(err))
			return err;

		if (!ret) {
			if (first_non_return) {
				common_type = type;
				first_non_return = false;
			}

			if ((common_type == DOUBLE && type == INT) || (common_type == INT && type == DOUBLE)) {
				common_type = DOUBLE;
			} else if (common_type != type)
				common_type = UNDEF;
		}

		all_return = all_return && ret;
		all_unit = all_unit && (type == UNIT);
	}

	if (common_type == DOUBLE) {
		ASTNode *value;
		for (size_t i = 0; i <= if_count; i++) {
			ASTNode *branch;
			if (i == 0) {
				branch = node;
			} // The 'if' itself
			else {
				branch = node->if_else_stmt.next_conds.items[i - 1];
			}

			value = last_value_node(branch->if_else_stmt.body);
			if (value && value->expr_types == INT)
				mark_convert(value);
		}
	}

	*returns = has_else && all_return;
	if (*returns || all_unit) {
		*out = UNIT;
	} else {
		*out = common_type;
	}

	return ERROR_OK();
}

static Error analyze_for(SemanticContext *ctx, ASTNode *node, ValType *out, bool *returns) {
	if (!ctx || !node || !out || !returns || node->type != NODE_FOR)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Inavlid parameters to analyze_for");

	Error err = ERROR_OK();

	if (!node->for_stmt.gen_start)
		return ERROR_EMIT(
			ERR_INTERNAL, node->line, node->col, "Start, in for statement is not defined");
	if (!node->for_stmt.gen_end)
		return ERROR_EMIT(
			ERR_INTERNAL, node->line, node->col, "End, in for statement is not defined");

	ValType type_step;
	if (node->for_stmt.gen_step) {
		err = analyze_expression(ctx, node->for_stmt.gen_step, &type_step);
		if (ERROR_IS_ERR(err))
			return err;

		if (type_step != INT)
			return ERROR_EMIT(
				ERR_SEM_TYPE, node->line, node->col, "Step, in for statement is the wrong type");
		if (node->for_stmt.gen_step->type == NODE_INT_LITERAL &&
			node->for_stmt.gen_step->int_val == 0)
			return ERROR_EMIT(
				ERR_SEM_MAIN, node->line, node->col, "Step, in for statement can't be 0");
	}

	ValType type_start;
	err = analyze_expression(ctx, node->for_stmt.gen_start, &type_start);
	if (ERROR_IS_ERR(err))
		return err;
	if (type_start != INT)
		return ERROR_EMIT(ERR_SEM_TYPE, node->for_stmt.gen_start->line,
			node->for_stmt.gen_start->col, "Start, in for statement is not of type INT");

	ValType type_end;
	err = analyze_expression(ctx, node->for_stmt.gen_end, &type_end);
	if (ERROR_IS_ERR(err))
		return err;
	if (type_end != INT)
		return ERROR_EMIT(ERR_SEM_TYPE, node->for_stmt.gen_end->line, node->for_stmt.gen_end->col,
			"End, in for statement is not of type INT");

	if (!scope_push(&ctx->scope_stack))
		return ERROR_EMIT(
			ERR_INTERNAL, node->line, node->col, "Out of memory when opening a scope");

	ScopeResult sr = scope_insert(&ctx->scope_stack, node, INT, NULL);
	if (sr == SCOPE_DUP) {
		return ERROR_EMIT(ERR_SEM_REDEF, node->line, node->col,
			"For statement name already defined in the current scope");
	} else if (sr == SCOPE_NOMEM) {
		return ERROR_EMIT(ERR_INTERNAL, node->line, node->col,
			"Out of memory while inserting for statement into scope");
	}

	ASTNodeList *guards = &node->for_stmt.guards;
	for (size_t i = 0; i < guards->count; i++) {
		ASTNode *guard = guards->items[i];
		err = analyze_condition(ctx, guard);
		if (ERROR_IS_ERR(err))
			return err;
	}

	ValType type_body;
	bool ret;
	err = analyze_body(ctx, node->for_stmt.body, &type_body, &ret);
	if (ERROR_IS_ERR(err))
		return err;

	scope_pop(&ctx->scope_stack);
	*out = UNIT;
	*returns = false;

	return ERROR_OK();
}

static Error analyze_while(SemanticContext *ctx, ASTNode *node, ValType *out, bool *returns) {
	if (!ctx || !node || !out || !returns || node->type != NODE_WHILE)
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_while");

	Error err = analyze_condition(ctx, node->while_stmt.cond);
	if (ERROR_IS_ERR(err))
		return err;

	ValType type;
	bool ret;

	err = analyze_body(ctx, node->while_stmt.body, &type, &ret);
	if (ERROR_IS_ERR(err))
		return err;

	*out = UNIT;
	// The condition could be false all along,
	// which results in program not reaching the return
	*returns = false;

	return ERROR_OK();
}

Error analyze_stmt(SemanticContext *ctx, ASTNode *stmt, ValType *out, bool *returns) {
	if (!ctx || !stmt || !out || !returns) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_stmt");
	}
	*returns = false;

	switch (stmt->type) {
		case NODE_VAR_DECL: {
			Error err = analyze_var_decl(ctx, stmt);
			if (!ERROR_IS_ERR(err))
				*out = UNIT;
			return err;
		}
		case NODE_BINARY_OP:
			if (stmt->op.op == OP_ASSIGN) {
				Error err = analyze_assignment(ctx, stmt);
				if (!ERROR_IS_ERR(err))
					*out = UNIT;
				return err;
			}
			return analyze_expression(ctx, stmt, out);
		case NODE_CALL:
		case NODE_UNARY_OP:
		case NODE_INT_LITERAL:
		case NODE_FLOAT_LITERAL:
		case NODE_STRING_LITERAL:
		case NODE_NULL_LITERAL:
		case NODE_BOOL_LITERAL:
		case NODE_IDENTIFIER:
			return analyze_expression(ctx, stmt, out);
		case NODE_BLOCK:
			return analyze_block(ctx, stmt, true, out, returns);
		case NODE_RETURN:
			return analyze_return(ctx, stmt, out, returns);
		case NODE_IF:
			return analyze_if(ctx, stmt, out, returns);
		case NODE_WHILE:
			return analyze_while(ctx, stmt, out, returns);
		case NODE_FOR:
			return analyze_for(ctx, stmt, out, returns);
		default:
			return ERROR_EMIT(
				ERR_INTERNAL, stmt->line, stmt->col, "Unsupported statement type for analysis");
	}
}

static Error analyze_function_body(SemanticContext *ctx, FunctionEntry *entry) {
	// Analyze the function body
	ASTNode *func = entry->decl;

	// Insert function parameters into the new scope
	if (!scope_push(&ctx->scope_stack)) {
		return ERROR_EMIT(ERR_INTERNAL, func->line, func->col,
			"Out of memory while creating new scope for function parameters");
	}

	// Go through each parameter and push them on stack
	if (func->func_decl.param_windows.count > 0) {
		ASTNode *window = func->func_decl.param_windows.items[0];
		for (size_t i = 0; i < window->param_window.params.count; i++) {
			ASTNode *param = window->param_window.params.items[i];
			ValType param_type;

			Error type_err = type_from_nodes(
				&param->func_param.param_type, param->line, param->col, &param_type);
			if (ERROR_IS_ERR(type_err))
				return type_err;

			ScopeResult sr = scope_insert(&ctx->scope_stack, param, param_type, NULL);
			if (sr == SCOPE_DUP) {
				return ERROR_EMIT(ERR_SEM_REDEF, param->line, param->col,
					"Parameter name already defined in the current scope");
			} else if (sr == SCOPE_NOMEM) {
				return ERROR_EMIT(ERR_INTERNAL, param->line, param->col,
					"Out of memory while inserting parameter into scope");
			}
		}
	}

	ASTNode *body = func->func_decl.body;
	if (!body)
		return ERROR_EMIT(ERR_INTERNAL, func->line, func->col, "Function body is missing");

	ValType body_type = UNIT;
	ASTNode *body_node = body;
	bool returns = false;

	// Check what the body returns
	if (body_node->type != NODE_BLOCK) {
		Error err = analyze_stmt(ctx, body_node, &body_type, &returns);
		if (ERROR_IS_ERR(err))
			return err;
	} else {
		Error err = analyze_block(ctx, body_node, false, &body_type, &returns);
		if (body_node->block.stmts.count > 0)
			body_node = body->block.stmts.items[body->block.stmts.count - 1];
		if (ERROR_IS_ERR(err))
			return err;
	}

	ValType return_type;
	Error type_error = type_from_nodes(
		&entry->decl->func_decl.return_type, entry->decl->line, entry->decl->col, &return_type);
	if (ERROR_IS_ERR(type_error))
		return type_error;
	if (return_type == UNIT)
		return ERROR_OK();

	// Set the function return type if undef
	if (return_type == UNDEF) {
		// Omitted type
		if (body_type == UNDEF)
			return ERROR_EMIT(ERR_SEM_TYPE, func->line, func->col,
				"The branches of the last if do not have a common type");
		if (body_type == BOOLEAN && !booleans_enabled())
			return ERROR_EMIT(ERR_SEM_TYPE, func->line, func->col, "Cannot return a BOOLEAN value");
		entry->return_type = body_type;
		if (entry->is_main && body_type != UNIT)
			return ERROR_EMIT(
				ERR_SEM_TYPE, func->line, func->col, "@main function must return UNIT");
		return ERROR_OK();
	}

	// If there's no return in body, check last experssion's type against function's return type
	if (!returns) {
		if (body_type == UNDEF)
			return ERROR_EMIT(
				ERR_SEM_TYPE, func->line, func->col, "Last if must return in every branch");
		bool conv = false;
		if (!type_compatible(return_type, body_type, &conv))
			return ERROR_EMIT(ERR_SEM_TYPE, func->line, func->col,
				"Body is not compatible with declared return type");
		if (conv)
			mark_convert(body_node);
	}
	return ERROR_OK();
}

Error analyze_function(SemanticContext *ctx, FunctionEntry *entry) {
	if (!ctx || !entry || !entry->decl || entry->decl->type != NODE_FUNC_DECL) {
		return ERROR_EMIT(ERR_INTERNAL, 0, 0, "Invalid parameters to analyze_function");
	}

	ValType return_type;
	Error type_error = type_from_nodes(
		&entry->decl->func_decl.return_type, entry->decl->line, entry->decl->col, &return_type);
	if (ERROR_IS_ERR(type_error))
		return type_error;

	entry->state = FUNC_RUNNING;

	// Create new scope for the function, then restore
	ScopeStack saved_scopes = ctx->scope_stack;
	ASTNode *saved_func = ctx->current_function;
	ValType saved_return_type = ctx->current_return_type;
	scope_init(&ctx->scope_stack);
	ctx->current_function = entry->decl;
	ctx->current_return_type = return_type;

	Error err = analyze_function_body(ctx, entry);

	// Restore the previous state
	scope_free(&ctx->scope_stack);
	ctx->scope_stack = saved_scopes;
	ctx->current_function = saved_func;
	ctx->current_return_type = saved_return_type;

	if (!ERROR_IS_ERR(err)) {
		entry->state = FUNC_DONE;				   // Mark the function as analyzed
		set_type(entry->decl, entry->return_type); // Codegen needs to know if the function's last
												   // expression is returned or thrown away
	}

	return err;
}