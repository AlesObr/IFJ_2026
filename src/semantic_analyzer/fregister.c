/**
 * @project IFJ26 Compiler
 * @file fregister.c
 * @brief Registers built-in and user-defined functions into the function table.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */
#include "fregister.h"
#include "type_rules.h"
#include <stdlib.h>

bool register_builtin_function(FunctionTable *t) {
	static const struct {
		const char *name;
		ValType return_type;
		size_t param_count;
		ValType param_types[3];
		// if changed, change it in code_gen.c as well
	} builtins[] = {{"print", UNIT, 1, {ANY}}, {"readLine", STRING, 0, {0}},
		{"readInt", INT, 0, {0}}, {"readDouble", DOUBLE, 0, {0}}, {"toStr", STRING, 1, {ANY}},
		{"toInt", INT, 1, {DOUBLE}}, {"strLength", INT, 1, {STRING}},
		{"subStr", STRING, 3, {STRING, INT, INT}}, {"ord", INT, 2, {STRING, INT}},
		{"chr", STRING, 1, {INT}}, {"readBoolean", BOOLEAN, 0, {0}}};

	for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
		FunctionEntry entry;
		entry.name = (char *)builtins[i].name; // cast to non-const
		entry.return_type = builtins[i].return_type;
		entry.param_count = builtins[i].param_count;
		entry.param_types = (ValType *)builtins[i].param_types; // cast to non-const
		entry.is_main = false;
		entry.is_builtin = true;
		entry.decl = NULL;
		entry.state = FUNC_DONE;

		if (!ftable_insert(t, &entry))
			return false; // allocation failure or duplicate
	}
	return true;
}

Error register_user_function(FunctionTable *t, ASTNode *func_node) {
	if (func_node->type != NODE_FUNC_DECL)
		return ERROR_EMIT(
			ERR_INTERNAL, func_node->line, func_node->col, "Function is not a definition node");

	// Make a new entry
	FunctionEntry entry;
	entry.name = (char *)dstring_get(&func_node->func_decl.func_name);
	entry.param_types = NULL; // Will be allocated below
	Error type_error = type_from_nodes(
		&func_node->func_decl.return_type, func_node->line, func_node->col, &entry.return_type);
	if (ERROR_IS_ERR(type_error))
		return type_error;

	// Build the parameter types by iterating through the parameter windows
	entry.param_count = 0;
	if (func_node->func_decl.param_windows.count > 0) { // Check if there are parameter windows
		ASTNode *first_window = func_node->func_decl.param_windows.items[0];
		entry.param_count = first_window->param_window.params.count;

		// Check if there are parameters to allocate
		if (entry.param_count > 0) {
			entry.param_types = malloc(entry.param_count * sizeof(ValType));

			// Check for allocation failure
			if (!entry.param_types)
				return ERROR_EMIT(ERR_INTERNAL, func_node->line, func_node->col,
					"Failed to allocate memory for function parameters");

			// Go through each parameter in the first window
			for (size_t i = 0; i < entry.param_count; i++) {
				ASTNode *param_node = first_window->param_window.params.items[i];

				// Check if the node is a function parameter
				if (param_node->type != NODE_FUNC_PARAM) {
					free(entry.param_types);
					return ERROR_EMIT(ERR_INTERNAL, func_node->line, func_node->col,
						"Invalid parameter node type");
				}

				type_error = type_from_nodes(&param_node->func_param.param_type, param_node->line,
					param_node->col, &entry.param_types[i]);

				if (ERROR_IS_ERR(type_error) || entry.param_types[i] == UNDEF) {
					free(entry.param_types);
					if (!ERROR_IS_ERR(type_error))
						type_error = ERROR_EMIT(ERR_SEM_TYPE, param_node->line, param_node->col,
							"Parameter without a type");
					return type_error;
				}
			}
		}
	}

	entry.is_main = func_node->func_decl.is_main;
	entry.is_builtin = false;
	entry.decl = func_node;
	entry.state = FUNC_NEW;

	// Check for duplicate function name
	if (ftable_lookup(t, entry.name) != NULL) {
		free(entry.param_types);
		return ERROR_EMIT(
			ERR_SEM_REDEF, func_node->line, func_node->col, "Duplicate function name");
	}

	// Insert and check for insertion failure
	if (!ftable_insert(t, &entry)) {
		free(entry.param_types);
		return ERROR_EMIT(
			ERR_INTERNAL, func_node->line, func_node->col, "Failed to register function");
	}

	free(entry.param_types);

	return ERROR_OK(); // Function registered successfully
}

Error register_functions(FunctionTable *t, const ASTNodeList *top_level_nodes) {
	size_t main_count = 0;

	// Iterate through top-level AST nodes
	for (size_t i = 0; i < top_level_nodes->count; i++) {
		ASTNode *node = top_level_nodes->items[i];

		// Check if the node is a function declaration
		if (node->type == NODE_FUNC_DECL) {
			Error err = register_user_function(t, node);

			if (ERROR_IS_ERR(err))
				return err; // Return the first error encountered

			// Check if the function is marked as @main
			if (node->func_decl.is_main) {
				main_count++;

				// Validate @main function constraints
				if (node->func_decl.param_windows.count > 0 &&
					node->func_decl.param_windows.items[0]->param_window.params.count > 0) {
					return ERROR_EMIT(ERR_SEM_CALL, node->line, node->col,
						"@main function should not have parameters");
				}

				// Check if multiple @main functions are defined
				if (main_count > 1) {
					return ERROR_EMIT(
						ERR_SEM_REDEF, node->line, node->col, "Multiple @main functions defined");
				}

				// Check if @main function has the correct return type
				FunctionEntry *en = ftable_lookup(t, dstring_get(&node->func_decl.func_name));

				// UNDEF will be handled in the second pass
				if (en && !(en->return_type == UNIT || en->return_type == UNDEF))
					return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col,
						"@main function must have return type Unit");
			}
		}
	}

	// Check if no @main function is defined
	if (main_count == 0) {
		return ERROR_EMIT(ERR_SEM_UNDEF, 0, 0, "No @main function defined");
	}

	return ERROR_OK();
}