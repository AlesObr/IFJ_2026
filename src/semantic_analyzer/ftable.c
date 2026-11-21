/**
 * @project IFJ26 Compiler
 * @file ftable.c
 * @brief Function table management for semantic analysis.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#include "ftable.h"
#include <stdlib.h>
#include <string.h>

#define FTABLE_SYMTABLE_CAPACITY 32

static char *copy_str(const char *src) {
	size_t len = strlen(src);
	char *dst = malloc(len + 1);
	if (!dst)
		return NULL;
	memcpy(dst, src, len + 1);
	return dst;
}

static ASTNode *create_builtin_node(const char *name) {
	ASTNode *n = calloc(1, sizeof(ASTNode));
	if (!n)
		return NULL;

	n->type = NODE_FUNC_DECL;
	if (!dstring_init(&n->func_decl.func_name)) {
		free(n);
		return NULL;
	}

	if (!dstring_append_str(&n->func_decl.func_name, name)) {
		dstring_free(&n->func_decl.func_name);
		free(n);
		return NULL;
	}

	return n;
}

static void free_builtin_node(ASTNode *n) {
	if (!n)
		return;
	dstring_free(&n->func_decl.func_name);
	free(n);
}

static void free_entry(FunctionEntry *e) {
	if (!e->decl)
		free_builtin_node(e->symnode);
	free(e->name);
	free(e->param_types);
	free(e);
}

void ftable_init(FunctionTable *t) {
	memset(t, 0, sizeof(*t));
}

void ftable_free(FunctionTable *t) {
	for (size_t i = 0; i < t->count; i++) {
		free_entry(t->f_entries[i]);
	}
	free(t->f_entries);
	symtable_destroy(t->names);
	memset(t, 0, sizeof(*t));
}

FunctionEntry *ftable_lookup(const FunctionTable *t, const char *name) {
	if (!t->names)
		return NULL;

	const symbol *found = symtable_get_data(t->names, name);
	if (!found)
		return NULL;

	size_t id = found->node->sym_id;
	if (id == 0 || id > t->count)
		return NULL;
	return t->f_entries[id - 1];
}

bool ftable_insert(FunctionTable *t, const FunctionEntry *e) {
	if (!t->names) {
		t->names = symtable_init(FTABLE_SYMTABLE_CAPACITY);
		if (!t->names)
			return false;
	}

	// Check for duplicate
	if (symtable_search(t->names, e->name))
		return false;

	FunctionEntry *new_e = calloc(1, sizeof(FunctionEntry));
	if (!new_e)
		return false;
	new_e->return_type = e->return_type;
	new_e->param_count = e->param_count;
	new_e->is_main = e->is_main;
	new_e->is_builtin = e->is_builtin;
	new_e->decl = e->decl;
	new_e->state = e->state;
	new_e->name = copy_str(e->name);
	if (!new_e->name) {
		free(new_e);
		return false;
	}

	if (e->param_count > 0) {
		new_e->param_types = malloc(e->param_count * sizeof(ValType));
		if (!new_e->param_types) {
			free_entry(new_e);
			return false;
		}
		memcpy(new_e->param_types, e->param_types, e->param_count * sizeof(ValType));
	}

	new_e->symnode = e->decl;
	if (!new_e->symnode) {
		new_e->symnode = create_builtin_node(new_e->name);
		if (!new_e->symnode) {
			free_entry(new_e);
			return false;
		}
	}

	if (t->count >= t->capacity) {
		size_t ncap = t->capacity ? t->capacity * 2 : 16;
		void *temp = realloc(t->f_entries, ncap * sizeof(*t->f_entries));
		if (!temp) {
			free_entry(new_e);
			return false;
		}
		t->f_entries = temp;
		t->capacity = ncap;
	}

	new_e->symnode->sym_id = t->count + 1;
	if (!symtable_insert(t->names, new_e->symnode)) {
		free_entry(new_e);
		return false;
	}
	t->f_entries[t->count++] = new_e;
	return true;
}