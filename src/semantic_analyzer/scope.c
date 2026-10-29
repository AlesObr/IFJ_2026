/**
 * @project IFJ26 Compiler
 * @file scope.c
 * @brief Scope management for semantic analysis (issue #21).
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11, Temporary linear-scan storage until symtable is finished.
 */

#include "scope.h"
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

void scope_init(ScopeStack *s) {
	memset(s, 0, sizeof(*s));
}

void scope_free(ScopeStack *s) {
	for (size_t i = 0; i < s->count; i++) {
		free(s->syms[i].name);
	}
	free(s->syms);
	free(s->marks);
	memset(s, 0, sizeof(*s));
}

bool scope_push(ScopeStack *s) {
	if (s->nmarks == s->mcap) {
		size_t new_cap = s->mcap ? s->mcap * 2 : 8;
		size_t *p = realloc(s->marks, new_cap * sizeof(*p));
		if (!p)
			return false;
		s->marks = p;
		s->mcap = new_cap;
	}
	s->marks[s->nmarks++] = s->count;
	return true;
}

void scope_pop(ScopeStack *s) {
	if (s->nmarks == 0)
		return; // nothing to pop
	size_t start = s->marks[--s->nmarks];
	for (size_t i = start; i < s->count; i++) {
		free(s->syms[i].name);
	}
	s->count = start;
}

Symbol *scope_lookup(const ScopeStack *s, const char *name) {
	for (size_t i = s->count; i > 0; i--) { /* top -> bottom, first hit wins */
		if (strcmp(s->syms[i - 1].name, name) == 0) {
			return &s->syms[i - 1];
		}
	}
	return NULL;
}

ScopeResult scope_insert(
	ScopeStack *s, const char *name, ValType type, bool is_const, Symbol **out) {
	size_t start = s->nmarks ? s->marks[s->nmarks - 1] : 0;

	for (size_t i = s->count; i > start; i--) { /* stop at the scope boundary */
		if (strcmp(s->syms[i - 1].name, name) == 0) {
			if (out)
				*out = &s->syms[i - 1];
			return SCOPE_DUP;
		}
	}

	if (s->count == s->cap) {
		size_t new_cap = s->cap ? s->cap * 2 : 16;
		Symbol *p = realloc(s->syms, new_cap * sizeof(*p));
		if (!p)
			return SCOPE_NOMEM;
		s->syms = p;
		s->cap = new_cap;
	}

	char *copy = copy_str(name);
	if (!copy)
		return SCOPE_NOMEM;

	Symbol *sym = &s->syms[s->count++];
	sym->name = copy;
	sym->type = type;
	sym->is_const = is_const;
	sym->id = s->next_id++;
	if (out)
		*out = sym;
	return SCOPE_OK;
}