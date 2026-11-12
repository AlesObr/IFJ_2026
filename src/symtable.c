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
#include <string.h>

#define MIN_TABLE_SIZE 2

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
 * @brief Compares 2 keys of symbol table
 * @param key1 1st key string to compare
 * @param key2 2nd key string to compare
 * @return true if keys are equal, false otherwise
*/
bool key_compare(const char* key1, const char* key2){
	const char* key1_val = (key1 != NULL) ? key1 : "";
	const char* key2_val = (key2 != NULL) ? key2 : "";
	return (strcmp(key1_val, key2_val) == 0);
}

/**
 * @brief Hashes entry onto symbol table index
 * @return calculated index of synonym list
*/
int hash_function(symtable* htable, AST_node* node){
	return 0;
}

/**
 * @brief Find symbol containing key in symbol table
 * @param key Key to be found
 * @return Pointer to symbol containing key if found, pointer to end of synonym list if not present
*/
symbol** find_symbol(symtable* htable, char* key){
	int idx = hash_function(htable, key);
	while(true){
		if ((htable->htable[idx] == NULL) || key_compare(htable->htable[idx]->key, key)){
			return &(htable->htable[idx]);
		}
		idx = (idx + 1) % (htable->capacity + 1);
	}
}

/**
 * @brief Converts input node into symbol
 * @param mem Memory to save symbol into
 * @param node AST node to be converted into symbol
 * @return 0 if success, 1 if memmory allocation problem or missing node members
*/
int convert_to_symbol(symbol** mem, AST_node* node){
	return 1;
}

/** --- Symbol table public methods --- */

symtable* symtable_init(int capacity){
	if (capacity < MIN_TABLE_SIZE){
		return NULL;
	}
	symtable* htable = malloc(sizeof(symtable));
	if (htable == NULL){
		return NULL;
	}
	htable->size = 0;
	htable->capacity = capacity-1;
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
	symbol** item = find_symbol(htable, idx);
	if (item == NULL || *item != NULL){
		return false;
	}
	if (!(convert_to_symbol(item, node))){
		return false;
	}
	htable->size += 1;
	return true;
}

bool symtable_search(symtable* htable, char* key){
	symbol** item = find_symbol(htable, key);
	return !(item == NULL || *item == NULL || !key_compare((*item)->key, key));
}

const symbol* symtable_get_data(symtable* htable, char* key){
	symbol** item = find_symbol(htable, key);
	if (item != NULL && *item != NULL && key_compare((*item)->key, key)){
		return *item;
	}
	return NULL;
}

void symtable_destroy(symtable* htable){
	if (htable == NULL){
		return;
	}
	for (int idx = 0; idx < htable->capacity; idx++){
		if (htable->htable[idx] != NULL){
			free(htable->htable[idx]);
		}
	}
	free(htable->htable);
	free(htable);
	return;
}