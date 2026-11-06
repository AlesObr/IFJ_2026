/**
 * @project IFJ26 Compiler
 * @file fregister.c
 * @brief Registers built-in and user-defined functions into the function table.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */
#include "fregister.h"
#include <stdlib.h>

bool register_builtin_function(FunctionTable *t) {
	static const struct {
		const char *name;
		ValType return_type;
		size_t param_count;
		ValType param_types[3];
	} builtins[] = {{"print", UNIT, 1, {ANY}}, {"readLine", STRING, 0, {0}},
		{"readInt", INT, 0, {0}}, {"readDouble", DOUBLE, 0, {0}}, {"toStr", STRING, 1, {ANY}},
		{"toInt", INT, 1, {DOUBLE}}, {"strLength", INT, 1, {STRING}},
		{"subStr", STRING, 3, {STRING, INT, INT}}, {"ord", INT, 2, {STRING, INT}},
		{"chr", STRING, 1, {INT}}};

	for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
		FunctionEntry entry;
		entry.name = (char *)builtins[i].name; // cast to non-const
		entry.return_type = builtins[i].return_type;
		entry.param_count = builtins[i].param_count;
		entry.param_types = (ValType *)builtins[i].param_types; // cast to non-const
		entry.is_main = false;
		entry.is_builtin = true;
		entry.decl = NULL;

		if (!ftable_insert(t, &entry)) {
			return false; // allocation failure or duplicate
		}
	}
	return true;
}

Error register_user_function(FunctionTable *t, ASTNode *func_node) {
	if (func_node->type != NODE_FUNC_DECL) {
		return ERROR_EMIT(ERR_INTERNAL, func_node->line, func_node->col,
			"Function is not a definition node"); // Not a function definition node
	}

	FunctionEntry entry;
	entry.name = (char *)dstring_get(&func_node->func_decl.func_name);
	entry.return_type = func_node->func_decl.return_type;
	entry.param_types = NULL; // Will be allocated below

	/* Build the parameter types by iterating through the parameter windows */
	entry.param_count = 0;
	if (func_node->func_decl.param_windows.count > 0) { // Check if there are parameter windows
		ASTNode *first_window = func_node->func_decl.param_windows.items[0];
		entry.param_count = first_window->param_window.params.count;
		if (entry.param_count > 0) { // Check if there are parameters to allocate
			entry.param_types = malloc(entry.param_count * sizeof(ValType));
			if (!entry.param_types) { // Check for allocation failure
				return ERROR_EMIT(ERR_INTERNAL, func_node->line, func_node->col,
					"Failed to allocate memory for function parameters"); // Memory allocation
																		  // failure
			}
			for (size_t i = 0; i < entry.param_count;
				i++) { // Go through each parameter in the first window
				ASTNode *param_node = first_window->param_window.params.items[i];
				if (param_node->type !=
					NODE_FUNC_PARAM) { // Check if the node is a function parameter
					free(entry.param_types);
					return ERROR_EMIT(ERR_INTERNAL, func_node->line, func_node->col,
						"Invalid parameter node type"); // Not a function parameter node
				}
				entry.param_types[i] = param_node->func_param.param_type;
			}
		}
	}

	entry.is_main = func_node->func_decl.is_main;
	entry.is_builtin = false;
	entry.decl = func_node;

	if (ftable_lookup(t, entry.name) != NULL) { // Check for duplicate function name
		free(entry.param_types);
		return ERROR_EMIT(ERR_SEM_REDEF, func_node->line, func_node->col,
			"Duplicate function name"); // Duplicate function name
	}

	if (!ftable_insert(t, &entry)) { // Insert and check for insertion failure
		free(entry.param_types);
		return ERROR_EMIT(ERR_INTERNAL, func_node->line, func_node->col,
			"Failed to register function"); // Internal error
	}
	free(entry.param_types);
	return ERROR_OK(); // Function registered successfully
}

Error register_functions(FunctionTable *t, const ASTNodeList *top_level_nodes) {
	size_t main_count = 0;
	for (size_t i = 0; i < top_level_nodes->count; i++) { // Iterate through top-level AST nodes
		ASTNode *node = top_level_nodes->items[i];
		if (node->type == NODE_FUNC_DECL) { // Check if the node is a function declaration
			Error err = register_user_function(t, node);
			if (ERROR_IS_ERR(err)) {
				return err; // Return the first error encountered
			}
			if (node->func_decl.is_main) { // Check if the function is marked as @main
				main_count++;
				// Validate @main function constraints
				if (node->func_decl.param_windows.count > 0 &&
					node->func_decl.param_windows.items[0]->param_window.params.count > 0) {
					return ERROR_EMIT(ERR_SEM_CALL, node->line, node->col,
						"@main function should not have parameters"); // @main should not have
																	  // parameters
				}
				// Check if multiple @main functions are defined
				if (main_count > 1) {
					return ERROR_EMIT(ERR_SEM_REDEF, node->line, node->col,
						"Multiple @main functions defined"); // Multiple @main functions
				}
				// Check if @main function has the correct return type
				if (!((node->func_decl.return_type == UNIT) ||
						(node->func_decl.return_type ==
							UNDEF))) { // UNDEF will be handled in the second pass
					return ERROR_EMIT(ERR_SEM_TYPE, node->line, node->col,
						"@main function must have return type Unit"); // @main must return Unit
				}
			}
		}
	}

	// Check if no @main function is defined
	if (main_count == 0) {
		return ERROR_EMIT(ERR_SEM_UNDEF, 0, 0, "No @main function defined"); // No @main function
	}

	return ERROR_OK(); // All functions registered successfully
}