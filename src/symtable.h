/**
 * @project IFJ26 Compiler
 * @file symtable.h
 * @brief Symbol table (TRP/AVL) interface will be here.
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_SYMTABLE_H
#define IFJ26_SYMTABLE_H

#include "ast.h"
#include <stdbool.h>

/**
 *  @brief Element of symbol table
*/
typedef struct {
	char* key; /**< Name of symbol */
} symbol;

/**
 *	@brief Public definition of inner symbol table structure
*/
typedef struct symtable_t symtable;

/**
 *  @brief Initializes symbol table
 *  @param capacity Number of elements in symbol table
 *  @return Pointer of allocated symtable, NULL if problem with memory allocation
*/
symtable* symtable_init(int capacity);

/**
 *  @brief Insert symbol into symbol table
 *  @param htable Symbol table to insert into
 *  @param node AST node containing data to be saved as symbol
 *  @return 0 if success, 1 if insertion fails
 */
bool symtable_insert(symtable* htable, ASTNode* node);

/**
 *  @brief Checks whether symbol is already in symbol table
 *  @param htable Symbol table to find symbol in
 *  @param key Name of symbol to be found
 *  @return True if symbol found, false if not
 */
bool symtable_search(symtable* htable, char* key);

/**
 *  @brief Retrieves data of symbol from symbol table
 *  @param htable Symbol table to find symbol in
 *  @param key Name of symbol to be retrieved
 *  @return Pointer to symbol if found, NULL otherwise
 */
const symbol* symtable_get_data(symtable* htable, char* key);

/**
 *  @brief Frees memory allocated in symbol table
*/
void symtable_destroy(symtable* htable);

#endif /* IFJ26_SYMTABLE_H */
