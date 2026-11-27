/**
 * @project IFJ26 Compiler
 * @file symtable.c
 * @brief Symbol table implemented as a hash table with implicit chaining.
 * @author Štěpán Hořenek (xhorens00)
 * @author Karel Kříž (xkrizka00)
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "symtable.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MIN_TABLE_SIZE 2
/** Maximum load factor in percent; above it the table grows (long probe sequences otherwise) */
#define MAX_LOAD_PERCENT 75

/**
 * @brief Inner symbol table structure
 */
struct symtable_t {
	symbol **htable; /**< Array of slots, NULL marks an empty slot */
	int size;		 /**< Number of symbols currently in table */
	int capacity;	 /**< Number of slots in the array */
};

/** --- Symbol table inner methods --- */

/**
 * @brief Compares 2 keys of symbol table
 * @param key1 1st key string to compare
 * @param key2 2nd key string to compare
 * @return true if keys are equal, false otherwise
 */
static bool key_compare(const char *key1, const char *key2) {
	const char *key1_val = (key1 != NULL) ? key1 : "";
	const char *key2_val = (key2 != NULL) ? key2 : "";
	return (strcmp(key1_val, key2_val) == 0);
}

/**
 * @brief Hashes a key onto a home slot of the table (sdbm string hash)
 * @param capacity Number of slots in the table
 * @param key Key to be hashed
 * @return Index of the slot where probing for the key starts
 */
static int hash_function(int capacity, const char *key) {
	unsigned long hash = 0;
	for (const unsigned char *c = (const unsigned char *)key; *c != '\0'; c++) {
		hash = *c + (hash << 6) + (hash << 16) - hash;
	}
	return (int)(hash % (unsigned long)capacity);
}

/**
 * @brief Find symbol containing key in symbol table (linear probing)
 * @param htable Symbol table to search in
 * @param key Key to be found
 * @return Pointer to the slot holding the key if present, otherwise pointer to the empty slot
 * that ends its probe sequence (where the key would be inserted)
 */
static symbol **find_symbol(const symtable *htable, const char *key) {
	int idx = hash_function(htable->capacity, key);
	while (htable->htable[idx] != NULL && !key_compare(htable->htable[idx]->key, key)) {
		idx = (idx + 1) % htable->capacity;
	}
	return &(htable->htable[idx]);
}

/**
 * @brief Returns the name declared by an AST node
 * @param node Declaring node
 * @return Name of the declared symbol, NULL if the node does not declare a name
 */
static const char *declared_name(const ASTNode *node) {
	switch (node->type) {
		case NODE_VAR_DECL:
			return dstring_get(&node->var_decl.var_name);
		case NODE_FUNC_PARAM:
			return dstring_get(&node->func_param.param_name);
		case NODE_FUNC_DECL:
			return dstring_get(&node->func_decl.func_name);
		case NODE_CLASS_DECL:
			return dstring_get(&node->data_class.class_name);
		case NODE_UNIF:
			return dstring_get(&node->type_unif.unif_name);
		case NODE_FOR:
			return dstring_get(&node->for_stmt.gen_id);
		default:
			return NULL;
	}
}

/**
 * @brief Creates a symbol for a key
 * @param key Name of the symbol (copied)
 * @param node Declaring AST node
 * @return Newly allocated symbol, NULL on memory allocation failure
 */
static symbol *create_symbol(const char *key, const ASTNode *node) {
	symbol *item = malloc(sizeof(symbol));
	if (item == NULL) {
		return NULL;
	}
	size_t length = strlen(key) + 1;
	item->key = malloc(length);
	if (item->key == NULL) {
		free(item);
		return NULL;
	}
	memcpy(item->key, key, length);
	item->node = node;
	return item;
}

/**
 * @brief Moves all symbols into a larger array (keys are rehashed for the new capacity)
 * @param htable Symbol table to grow
 * @return True if success, false on memory allocation failure (table stays unchanged)
 */
static bool grow(symtable *htable) {
	int new_capacity = htable->capacity * 2;
	symbol **new_slots = calloc((size_t)new_capacity, sizeof(symbol *));
	if (new_slots == NULL) {
		return false;
	}
	symtable bigger = {.htable = new_slots, .size = htable->size, .capacity = new_capacity};
	for (int idx = 0; idx < htable->capacity; idx++) {
		if (htable->htable[idx] != NULL) {
			*find_symbol(&bigger, htable->htable[idx]->key) = htable->htable[idx];
		}
	}
	free(htable->htable);
	htable->htable = new_slots;
	htable->capacity = new_capacity;
	return true;
}

/** --- Symbol table public methods --- */

symtable *symtable_init(int capacity) {
	if (capacity < MIN_TABLE_SIZE) {
		return NULL;
	}
	symtable *htable = malloc(sizeof(symtable));
	if (htable == NULL) {
		return NULL;
	}
	htable->size = 0;
	htable->capacity = capacity;
	htable->htable = calloc((size_t)capacity, sizeof(symbol *));
	if (htable->htable == NULL) {
		free(htable);
		return NULL;
	}
	return htable;
}

bool symtable_insert(symtable *htable, const ASTNode *node) {
	if (htable == NULL || node == NULL) {
		return false;
	}
	const char *key = declared_name(node);
	if (key == NULL) {
		return false;
	}
	symbol **item = find_symbol(htable, key);
	if (*item != NULL) {
		return false; // Redefinition in the same table
	}
	// Grow before the load factor is exceeded; this also keeps at least one slot empty
	if ((htable->size + 1) * 100 > htable->capacity * MAX_LOAD_PERCENT) {
		if (!grow(htable)) {
			return false;
		}
		item = find_symbol(htable, key);
	}
	*item = create_symbol(key, node);
	if (*item == NULL) {
		return false;
	}
	htable->size += 1;
	return true;
}

bool symtable_search(const symtable *htable, const char *key) {
	return symtable_get_data(htable, key) != NULL;
}

const symbol *symtable_get_data(const symtable *htable, const char *key) {
	if (htable == NULL || key == NULL) {
		return NULL;
	}
	return *find_symbol(htable, key);
}

void symtable_destroy(symtable *htable) {
	if (htable == NULL) {
		return;
	}
	for (int idx = 0; idx < htable->capacity; idx++) {
		if (htable->htable[idx] != NULL) {
			free(htable->htable[idx]->key);
			free(htable->htable[idx]);
		}
	}
	free(htable->htable);
	free(htable);
}
