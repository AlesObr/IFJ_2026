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

static char *copy_str(const char *src) {
	size_t len = strlen(src);
	char *dst = malloc(len + 1);
	if (!dst)
		return NULL;
	memcpy(dst, src, len + 1);
	return dst;
}

void ftable_init(FunctionTable *t) {
	memset(t, 0, sizeof(*t));
}

void ftable_free(FunctionTable *t) {
	for (size_t i = 0; i < t->count; i++) {
		free(t->f_entries[i]->name);
		free(t->f_entries[i]->param_types);
		free(t->f_entries[i]);
	}
	free(t->f_entries);
	memset(t, 0, sizeof(*t));
}

FunctionEntry *ftable_lookup(const FunctionTable *t, const char *name) {
	for (size_t i = 0; i < t->count; i++) {
		if (strcmp(t->f_entries[i]->name, name) == 0) {
			return t->f_entries[i];
		}
	}
	return NULL;
}

bool ftable_insert(FunctionTable *t, const FunctionEntry *e) {
	FunctionEntry *new_entry = malloc(sizeof(FunctionEntry));
	if (!new_entry)
		return false;
	new_entry->return_type = e->return_type;
	new_entry->param_count = e->param_count;
	new_entry->is_main = e->is_main;
	new_entry->is_builtin = e->is_builtin;
	new_entry->decl = e->decl;
	new_entry->name = copy_str(e->name);
	if (!new_entry->name) {
		free(new_entry);
		return false;
	}
	if (e->param_count > 0) {
		new_entry->param_types = malloc(e->param_count * sizeof(ValType));
		if (!new_entry->param_types) {
			free(new_entry->name);
			free(new_entry);
			return false;
		}
		memcpy(new_entry->param_types, e->param_types, e->param_count * sizeof(ValType));
	} else {
		new_entry->param_types = NULL;
	}

	if (ftable_lookup(t, new_entry->name) != NULL) {
		free(new_entry->name);
		free(new_entry->param_types);
		free(new_entry);
		return false;
	}
	if (t->count >= t->capacity) {
		size_t new_capacity = t->capacity ? t->capacity * 2 : 16;
		void *temp = realloc(t->f_entries, new_capacity * sizeof(*t->f_entries));
		if (!temp) {
			free(new_entry->name);
			free(new_entry->param_types);
			free(new_entry);
			return false;
		}
		t->capacity = new_capacity;
		t->f_entries = temp;
	}
	t->f_entries[t->count++] = new_entry;
	return true;
}