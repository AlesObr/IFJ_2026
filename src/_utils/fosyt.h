/**
 * @project IFJ26 Compiler
 * @file fosyt.h
 * @brief Fosyt module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_FOSYT_H
#define IFJ26_FOSYT_H

#include "../parser/ast.h"
#include "../symtable.h"

/**
 * @brief Structure representing a node in the function scope tree (FOSYT).
 */
typedef struct SymtableNode {
	Symtable *symtable;
	int scope_level;
	ASTNode *ast_node;

	struct SymtableNode *parent;
	struct SymtableNode **children;
	int num_children;
	int capacity;
} SymtableNode;

/**
 * @brief Structure representing the function scope tree (FOSYT).
 */
typedef struct Fosyt {
	SymtableNode *root;
	SymtableNode *current;
} Fosyt;

/**
 * @brief Initializes the function scope tree (FOSYT).
 * @param fosyt Pointer to the FOSYT structure to initialize.
 */
void fosyt_init(Fosyt *fosyt);

/**
 * @brief Destroys the function scope tree (FOSYT).
 * @param fosyt Pointer to the FOSYT structure to destroy.
 */
void fosyt_destroy(Fosyt *fosyt);

/**
 * @brief Creates a new node in the function scope tree (FOSYT).
 * @param symtable Pointer to the symbol table associated with the node.
 * @param scope_level The scope level of the node.
 * @param ast_node Pointer to the AST node associated with the node.
 * @return Pointer to the newly created SymtableNode.
 */
SymtableNode *fosyt_create_node(Symtable *symtable, int scope_level, ASTNode *ast_node);

/**
 * @brief Finds a node in the function scope tree (FOSYT) by its AST node.
 * @param fosyt Pointer to the FOSYT structure.
 * @param ast_node Pointer to the AST node to search for.
 * @return Pointer to the found SymtableNode, or NULL if not found.
 */
SymtableNode *fosyt_find_node(Fosyt *fosyt, ASTNode *ast_node);

/**
 * @brief Adds a child node to a parent node in the function scope tree (FOSYT).
 * @param parent Pointer to the parent SymtableNode.
 * @param child Pointer to the child SymtableNode.
 */
void fosyt_add_child(SymtableNode *parent, SymtableNode *child);

/**
 * @brief Enters a new scope in the function scope tree (FOSYT).
 * @param fosyt Pointer to the FOSYT structure.
 * @param symtable Pointer to the symbol table for the new scope.
 * @param ast_node Pointer to the AST node for the new scope.
 */
void fosyt_enter_scope(Fosyt *fosyt, Symtable *symtable, ASTNode *ast_node);

/**
 * @brief Exits the current scope in the function scope tree (FOSYT).
 * @param fosyt Pointer to the FOSYT structure.
 */
void fosyt_exit_scope(Fosyt *fosyt);

#endif /* IFJ26_FOSYT_H */
