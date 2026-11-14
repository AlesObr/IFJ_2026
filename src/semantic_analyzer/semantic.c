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

static void set_type(ASTNode *n, ValType t) {
	(void)n;
	(void)t; // Fields are in ast.h, just have to implement it
}
static void mark_convert(ASTNode *n) {
	(void)n; // Fields are in ast.h, just have to implement it
}

static bool booleans_enabled(void) {
	return false; // : BOOLTHEN
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
		default: // Temporary default case for unsupported expression types
			return ERROR_EMIT(
				ERR_INTERNAL, expr->line, expr->col, "Unsupported expression type for analysis");
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

	if (init_type == BOOLEAN) {
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

	if (expr_type == BOOLEAN)
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

	if (param_count != call->call.args.count) {
		return ERROR_EMIT(ERR_SEM_CALL, call->line, call->col,
			"Argument count does not match function parameter count");
	}

	for (size_t i = 0; i < param_count; i++) {
		ValType arg_type;
		Error err = analyze_expression(ctx, call->call.args.items[i], &arg_type);
		if (ERROR_IS_ERR(err))
			return err;

		bool conv_needed;
		if (!type_compatible(param_types[i], arg_type, &conv_needed)) {
			return ERROR_EMIT(ERR_SEM_CALL, call->call.args.items[i]->line,
				call->call.args.items[i]->col,
				"Argument type does not match function parameter type");
		}
		if (conv_needed) {
			mark_convert(call->call.args.items[i]);
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
		case NODE_IDENTIFIER: {
			return analyze_expression(ctx, stmt, out);
		}
		case NODE_BLOCK: {
			return analyze_block(ctx, stmt, true, out, returns);
		}
		default: // NODE_IF, NODE_WHILE, NODE_FOR, NODE_RETURN
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

	if (!ERROR_IS_ERR(err))
		entry->state = FUNC_DONE; // Mark the function as analyzed

	return err;
}