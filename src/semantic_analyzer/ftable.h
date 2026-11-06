/**
 * @project IFJ26 Compiler
 * @file ftable.h
 * @brief Function table management for semantic analysis.
 * @author Karel Kříž (xkrizka00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_FTABLE_H
#define IFJ26_FTABLE_H

#include "../parser/ast.h"
#include <stdbool.h>
#include <stddef.h>

/** @brief A function in the table */
typedef struct {
	char *name;			  /**< Function name */
	ValType return_type;  /**< What it returns */
	size_t param_count;	  /**< How many parameters the function takes */
	ValType *param_types; /**< Types of the parameters */
	bool is_main;		  /**< If the function is main */
	bool is_builtin;	  /**< If the function is builtin */
	ASTNode *decl;		  /**< The function node */
} FunctionEntry;

/** @brief Table of all inserted functions */
typedef struct {
	FunctionEntry **f_entries; /**< Array of pointers to functions */
	size_t count;			   /**< Number of functions in the table */
	size_t capacity;		   /**< Capacity of the functions array */
} FunctionTable;

/**
 * @brief Initializes function table
 * @param t The function table to initialize
 */
void ftable_init(FunctionTable *t);

/**
 * @brief Frees the whole table and each entry
 * @param t The function table to free
 */
void ftable_free(FunctionTable *t);

/**
 * @brief Function table lookup
 * @return Pointer to the function entry if found, NULL otherwise
 * @param t The function table to search
 * @param name The name of the function to look up
 */
FunctionEntry *ftable_lookup(const FunctionTable *t, const char *name);

/**
 * @brief Adds an entry into the function table
 * @param t The function table to insert into
 * @param e The function entry to insert
 * @return True on success, false on failure
 */
bool ftable_insert(FunctionTable *t, const FunctionEntry *e);

#endif // IFJ26_FTABLE_H