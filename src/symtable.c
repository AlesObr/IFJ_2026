/**
 * @project IFJ26 Compiler
 * @file symtable.c
 * @brief Symbol table (TRP/AVL) implementation will be here.
 * @author Štěpán Hořenek (xhorens00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

 #include "symtable.h"
 #include <stdbool.h>

 /**
  * @brief Inner symbol table structure
 */
 struct symtable_t{
	symbol** htable; /**< Array of symbols*/
	int size;       /**< Number of symbols currently in table */
	int capacity;   /**< Size of array of symbols */
 };

 symtable* symtable_init(int capacity){
	if (capacity <= 0){
		return NULL;
	}
	symtable* htable = malloc(sizeof(symtable));
	if (htable == NULL){
		return NULL;
	}
	htable->size = 0;
	htable->capacity = capacity;
	htable->htable = calloc(capacity, sizeof(symbol*));
	if (htable->htable == NULL){
		free(htable);
		return NULL;
	}
	return table;
 }

 void symtable_destroy(symtable* htable){
	if (htable == NULL){
		return;
	}
	for (int idx = 0; idx <htable->capacity; idx++){
		if (htable->htable[idx] != NULL){
			free(htable->htable[idx]);
		}
	}
	free(htable->htable);
	free(htable);
	return;
 }

 /** --- Symbol table inner methods --- */

 /**
  * @brief Hashes entry onto symbol table index
 */
 int symtable_hash(symtable* htable, AST_node* node){
	return 0;
 }