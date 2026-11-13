/**
 * @project IFJ26 Compiler
 * @file scope.c
 * @brief Scope management for semantic analysis (issue #21).
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11, Temporary linear-scan storage until symtable is finished.
 */

#include "scope.h"
#include "symtable.h"
#include <stdlib.h>
#include <string.h>

#define SCOPE_TABLE_CAPACITY 8

static const char *name_of_decl(const ASTNode *decl) {
	if (decl->type == NODE_VAR_DECL)
		return dstring_get(&decl->var_decl.var_name);
	if (decl->type == NODE_FUNC_PARAM)
		return dstring_get(&decl->func_param.param_name);
	return NULL;
}

static symtable *pop_table(ScopeStack *s) {
	return (symtable *)(void *)stack_pop(&s->scopes);
}

void scope_init(ScopeStack *s) {
	stack_init(&s->scopes);
	s->syms = NULL;
	s->count = 0;
	s->cap = 0;
}

void scope_free(ScopeStack *s) {
	while (s->scopes.top != NULL) {
		symtable_destroy(pop_table(s));
	}
	free(s->syms);
	s->syms = NULL;
	s->count = 0;
	s->cap = 0;
}

bool scope_push(ScopeStack *s) {
	symtable *t = symtable_init(SCOPE_TABLE_CAPACITY);
	if (!t)
		return false;
	stack_push(&s->scopes, t);
	return true;
}

void scope_pop(ScopeStack *s) {
	if (s->scopes.top == NULL)
		return;
	symtable_destroy(pop_table(s));
}

Symbol *scope_lookup(const ScopeStack *s, const char *name) {
	for (const StackElement *el = s->scopes.top; el != NULL; el = el->next) {
		const symbol *found = symtable_get_data((const symtable *)el->data, name);
		if (found != NULL) {
			size_t id = found->node->sym_id;
			if (id == 0 || id > s->count)
				return NULL;
			return &s->syms[id - 1];
		}
	}
	return NULL;
}

ScopeResult scope_insert(ScopeStack *s, ASTNode *decl, ValType type, Symbol **out) {
	const char *name = decl ? name_of_decl(decl) : NULL;
	if (!name || s->scopes.top == NULL)
		return SCOPE_NOMEM;
	symtable *cur = (symtable *)s->scopes.top->data;

	const symbol *existing = symtable_get_data(cur, name);
	if (existing != NULL) {
		if (out)
			*out = &s->syms[existing->node->sym_id - 1];
		return SCOPE_DUP;
	}

	if (s->count >= s->cap) {
		size_t ncap = s->cap ? s->cap * 2 : 16;
		Symbol *tmp = realloc(s->syms, ncap * sizeof(Symbol));
		if (!tmp)
			return SCOPE_NOMEM;
		s->syms = tmp;
		s->cap = ncap;
	}

	decl->sym_id = s->count + 1;
	if (!symtable_insert(cur, decl)) {
		decl->sym_id = 0;
		return SCOPE_NOMEM;
	}

	Symbol *sym = &s->syms[s->count++];
	sym->type = type;
	sym->is_const = (decl->type == NODE_FUNC_PARAM) ? true : decl->var_decl.is_const;
	sym->id = decl->sym_id;
	sym->node = decl;
	if (out)
		*out = sym;
	return SCOPE_OK;
}