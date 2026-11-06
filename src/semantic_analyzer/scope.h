/**
 * @project IFJ26 Compiler
 * @file scope.h
 * @brief Scope management for semantic analysis (issue #21).
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11, Temporary linear-scan storage until symtable is finished.
 */

#ifndef IFJ26_SCOPE_H
#define IFJ26_SCOPE_H

#include "../parser/ast.h"
#include <stdbool.h>
#include <stddef.h>

/** @brief A symbol in the scope */
typedef struct {
	char *name;	   /**< Owned copy of the identifier */
	ValType type;  /**< Type of the symbol */
	bool is_const; /**< Whether the symbol is a constant */
	size_t id;	   /**< Unique identifier for the symbol */
} Symbol;

/** @brief Result of scope operations */
typedef enum {
	SCOPE_OK,	/**< Operation successful */
	SCOPE_DUP,	/**< Name already exists in the current scope (error 4) */
	SCOPE_NOMEM /**< Memory allocation failed */
} ScopeResult;

/** @brief A stack of scopes */
typedef struct {
	Symbol *syms;	/**< Array of symbols, oldest first */
	size_t count;	/**< Number of symbols in the scope */
	size_t cap;		/**< Capacity of the symbols array */
	size_t *marks;	/**< marks[i] = index in syms where scope i begins*/
	size_t nmarks;	/**< Current nesting depth */
	size_t mcap;	/**< Capacity of the marks array */
	size_t next_id; /**< Next unique identifier for symbols */
} ScopeStack;

/**
 * @brief Initializes a new scope stack
 * @param stack The scope stack to initialize
 */
void scope_init(ScopeStack *stack);

/**
 * @brief Frees everything the stack owns
 * @param stack The scope stack to free
 */
void scope_free(ScopeStack *stack);

/**
 * @brief Opens a new innermost scope.
 * @param stack The scope stack to push a new scope onto
 * @returns False on allocation failure
 */
bool scope_push(ScopeStack *stack);

/**
 * @brief Closes the innermost scope and forgets its symbols
 * @param stack The scope stack to pop the innermost scope from
 */
void scope_pop(ScopeStack *stack);

/**
 * @brief Searches all scopes, innermost first
 * @param s The scope stack to search
 * @param name The name of the symbol to look up
 * @return Symbol or NULL if not found (error 3).
 * @warning The pointer is invalidated by the next scope_insert (the array may move).
 */
Symbol *scope_lookup(const ScopeStack *s, const char *name);

/**
 * @brief Declares a name in the current scope only
 * @param s The scope stack to insert into
 * @param name The name of the symbol to insert
 * @param type The type of the symbol to insert
 * @param is_const Whether the symbol is a constant
 * @param out Optional; recieves the new symbol, or the existing one on SCOPE_DUP.
 * @return SCOPE_OK on success, SCOPE_DUP if the name already exists in the current scope,
 */
ScopeResult scope_insert(
	ScopeStack *s, const char *name, ValType type, bool is_const, Symbol **out);

#endif // IFJ26_SCOPE_H