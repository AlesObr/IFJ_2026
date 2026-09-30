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

/** --- Symbol table inner methods --- */

/**
 * @brief Hashes entry onto symbol table index
 * @return calculated index of synonym list
*/
int hash_function(symtable* htable, AST_node* node){
	return 0;
}

/**
 * @brief Find symbol containing key in symbol table
 * @param entry Entrance to synonym list
 * @param key Key to be found
 * @return Pointer to symbol containing key if found, pointer to end of synonym list if not present
*/
symbol** find_idx(symtable* htable, int entry, char* key){
	return NULL;
}

/**
 * @brief Converts input node into symbol
 * @param mem Memory to save symbol into
 * @param node AST node to be converted into symbol
 * @return 0 if success, 1 if memmory allocation problem or missing node members
*/
int convert_to_symbol(symbol* mem, AST_node* node){
	return 1;
}

/** --- Symbol table public methods --- */

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

bool symtable_insert(symtable* htable, AST_node* node){
	if (htable == NULL || node == NULL){
		return false;
	}
	if (htable->size == htable->capacity){
		return false;
	}
	int idx = hash_function(htable, node);
	symbol* item = find_idx(htable, idx);
	if (item != NULL){
		return false;
	}
	convert_to_symbol(item, node);
	htable->size += 1;
	return true;
}

bool symtable_search(symtable* htable, char* identif){
	return false;
}

const symbol* symtable_get_data(symtable* htable, char* identif){
	return NULL;
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