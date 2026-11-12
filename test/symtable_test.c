/**
 * @file test_symtable.c
 * @brief Unit testy pro rozptylovací tabulku symbolů (implicitní řetězení / open addressing)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../src/symtable.h"

/* --- Lehký testovací harness --- */
static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(expr, msg) do { \
    if (!(expr)) { \
        printf("  [FAIL] Line %d: %s\n", __LINE__, msg); \
        return false; \
    } \
} while (0)

#define RUN_TEST(fn) do { \
    printf("[RUNNING] %s ...\n", #fn); \
    tests_run++; \
    if (fn()) { \
        printf("  [OK]\n"); \
    } else { \
        tests_failed++; \
    } \
} while (0)

/* --- Pomocné funkce pro vytvoření testovacích ASTNode --- */

/* Deklarace proměnné `val name` (skutečný uzel AST, klíčem je jeho jméno) */
static ASTNode *create_var_node(const char *name) {
    dstring_t dname;
    if (!dstring_init(&dname)) return NULL;
    ASTNode *node = NULL;
    if (dstring_append_str(&dname, name)) {
        node = ast_create_var_decl(&dname, true, 1, 1);
    }
    dstring_free(&dname);
    return node;
}

static ASTNode *create_func_node(const char *name) {
    dstring_t dname;
    if (!dstring_init(&dname)) return NULL;
    ASTNode *node = NULL;
    if (dstring_append_str(&dname, name)) {
        node = ast_create_func(&dname, false, 1, 1);
    }
    dstring_free(&dname);
    return node;
}

static ASTNode *create_param_node(const char *name) {
    dstring_t dname;
    if (!dstring_init(&dname)) return NULL;
    ASTNode *node = NULL;
    if (dstring_append_str(&dname, name)) {
        node = ast_create_func_param(&dname, 1, 1);
    }
    dstring_free(&dname);
    return node;
}

/* ========================================================================= */
/*                               TESTOVACÍ PŘÍPADY                           */
/* ========================================================================= */

/**
 * 1. Inicializace a uvolnění prázdné tabulky
 */
static bool test_init_and_destroy(void) {
    symtable* table = symtable_init(11);
    ASSERT(table != NULL, "symtable_init by mělo vrátit platný ukazatel");

    // Hledání v prázdné tabulce
    ASSERT(!symtable_search(table, "foo"), "Prázdná tabulka nesmí najít symbol");
    ASSERT(symtable_get_data(table, "foo") == NULL, "Data prázdné tabulky musí být NULL");

    symtable_destroy(table);
    symtable_destroy(NULL); // Nesmí spadnout
    return true;
}

/**
 * 2. Neplatné argumenty
 */
static bool test_invalid_arguments(void) {
    ASSERT(symtable_init(0) == NULL, "Kapacita 0 musí být odmítnuta");
    ASSERT(symtable_init(1) == NULL, "Kapacita 1 nenechá volný slot, musí být odmítnuta");

    symtable *table = symtable_init(5);
    ASSERT(table != NULL, "Chyba alokace tabulky");
    ASSERT(!symtable_insert(table, NULL), "Vložení NULL uzlu musí selhat");
    ASSERT(!symtable_insert(NULL, NULL), "Vložení do NULL tabulky musí selhat");
    ASSERT(!symtable_search(table, NULL), "Hledání NULL klíče musí vrátit false");
    ASSERT(symtable_get_data(NULL, "x") == NULL, "Hledání v NULL tabulce musí vrátit NULL");

    // Uzel, který nic nedeklaruje (blok), nemá jméno a nesmí se vložit
    ASTNode *block = ast_create_block(1, 1);
    ASSERT(block != NULL, "Chyba alokace uzlu");
    ASSERT(!symtable_insert(table, block), "Uzel bez jména nesmí jít vložit");

    ast_destroy(block);
    symtable_destroy(table);
    return true;
}

/**
 * 3. Vložení a vyhledání jednoho i více základních prvků
 */
static bool test_insert_and_lookup_basic(void) {
    symtable* table = symtable_init(17);
    ASSERT(table != NULL, "Chyba alokace tabulky");

    ASTNode* node_a = create_var_node("var_a");
    ASTNode* node_b = create_var_node("var_b");
    ASSERT(node_a != NULL && node_b != NULL, "Chyba alokace uzlů");

    ASSERT(symtable_insert(table, node_a) == true, "Vložení prvku var_a selhalo");
    ASSERT(symtable_insert(table, node_b) == true, "Vložení prvku var_b selhalo");

    // Ověření přítomnosti
    ASSERT(symtable_search(table, "var_a"), "Nenalezen vložený prvek var_a");
    ASSERT(symtable_search(table, "var_b"), "Nenalezen vložený prvek var_b");
    ASSERT(!symtable_search(table, "nonexistent"), "Falešná detekce nevloženého prvku");
    ASSERT(!symtable_search(table, "var_"), "Prefix klíče nesmí být nalezen");

    // Kontrola vrácených dat
    const symbol* s = symtable_get_data(table, "var_a");
    ASSERT(s != NULL, "symtable_get_data vrátilo NULL pro existující prvek");
    ASSERT(strcmp(s->key, "var_a") == 0, "Identifikátor symbolu se neshoduje");
    ASSERT(s->node == node_a, "Symbol musí ukazovat na deklarující uzel");
    // Tabulka si klíč kopíruje, nesmí sdílet paměť s uzlem
    ASSERT(s->key != dstring_get(&node_a->var_decl.var_name), "Klíč musí být kopie jména");

    symtable_destroy(table);
    ast_destroy(node_a);
    ast_destroy(node_b);
    return true;
}

/**
 * 4. Různé druhy deklarací (proměnná, funkce, parametr)
 */
static bool test_node_kinds(void) {
    symtable *table = symtable_init(7);
    ASSERT(table != NULL, "Chyba alokace tabulky");

    ASTNode *var = create_var_node("x");
    ASTNode *func = create_func_node("foo");
    ASTNode *param = create_param_node("p");
    ASSERT(var != NULL && func != NULL && param != NULL, "Chyba alokace uzlů");

    ASSERT(symtable_insert(table, var), "Vložení proměnné selhalo");
    ASSERT(symtable_insert(table, func), "Vložení funkce selhalo");
    ASSERT(symtable_insert(table, param), "Vložení parametru selhalo");

    const symbol *s = symtable_get_data(table, "foo");
    ASSERT(s != NULL && s->node == func && s->node->type == NODE_FUNC_DECL, "Funkce nenalezena");
    s = symtable_get_data(table, "p");
    ASSERT(s != NULL && s->node == param && s->node->type == NODE_FUNC_PARAM, "Parametr nenalezen");

    symtable_destroy(table);
    ast_destroy(var);
    ast_destroy(func);
    ast_destroy(param);
    return true;
}

/**
 * 5. Test kolizí (Collision Handling v otevřeném adresování)
 * Malá kapacita vynutí kolize i opakované zvětšování tabulky.
 */
static bool test_collisions(void) {
    symtable* table = symtable_init(2);
    ASSERT(table != NULL, "Chyba alokace");

    enum { COUNT = 200 };
    ASTNode* nodes[COUNT];
    char key_buf[16];

    for (int i = 0; i < COUNT; i++) {
        snprintf(key_buf, sizeof(key_buf), "k%d", i);
        nodes[i] = create_var_node(key_buf);
        ASSERT(nodes[i] != NULL, "Chyba alokace uzlu");
        ASSERT(symtable_insert(table, nodes[i]) == true, "Vložení při kolizi selhalo");
    }

    // Ověření, že všechny kolidující prvky jsou stále k nalezení (i po rehashi)
    for (int i = 0; i < COUNT; i++) {
        snprintf(key_buf, sizeof(key_buf), "k%d", i);
        const symbol* s = symtable_get_data(table, key_buf);
        ASSERT(s != NULL && strcmp(s->key, key_buf) == 0, "Kolidující klíč nebyl nalezen");
        ASSERT(s->node == nodes[i], "Data kolidujícího klíče jsou neplatná");
    }
    ASSERT(!symtable_search(table, "k200"), "Falešná detekce nevloženého prvku");

    symtable_destroy(table);
    for (int i = 0; i < COUNT; i++) {
        ast_destroy(nodes[i]);
    }
    return true;
}

/**
 * 6. Duplicitní vkládání stejného identifikátoru
 */
static bool test_duplicate_keys(void) {
    symtable* table = symtable_init(13);
    ASSERT(table != NULL, "Chyba alokace");

    ASTNode* node1 = create_var_node("shadow_var");
    ASTNode* node2 = create_func_node("shadow_var");
    ASSERT(node1 != NULL && node2 != NULL, "Chyba alokace uzlů");

    ASSERT(symtable_insert(table, node1) == true, "První vložení shadow_var selhalo");

    // Duplicitní deklarace v témže scope musí selhat (i když jde o jiný druh uzlu)
    ASSERT(symtable_insert(table, node2) == false,
           "Vložení duplicitního identifikátoru do stejné tabulky mělo selhat");
    const symbol *s = symtable_get_data(table, "shadow_var");
    ASSERT(s != NULL && s->node == node1, "Duplikát nesmí přepsat původní symbol");

    symtable_destroy(table);
    ast_destroy(node1);
    ast_destroy(node2);
    return true;
}

/**
 * 7. Mezní stav: tabulka se zaplní nad počáteční kapacitu
 * Tabulka se při 75 % zaplnění sama zvětší, vložení tedy nesmí selhat.
 */
static bool test_table_grows(void) {
    const int capacity = 5;
    symtable* table = symtable_init(capacity);
    ASSERT(table != NULL, "Chyba alokace");

    enum { COUNT = 6 };
    ASTNode* nodes[COUNT];
    char key_buf[16];

    for (int i = 0; i < COUNT; i++) {
        snprintf(key_buf, sizeof(key_buf), "id_%d", i);
        nodes[i] = create_var_node(key_buf);
        ASSERT(nodes[i] != NULL, "Chyba alokace uzlu");
        ASSERT(symtable_insert(table, nodes[i]) == true, "Vložení nad počáteční kapacitu selhalo");
    }

    // Zkontrolujeme, že existující prvky zůstaly nepoškozené
    for (int i = 0; i < COUNT; i++) {
        snprintf(key_buf, sizeof(key_buf), "id_%d", i);
        ASSERT(symtable_search(table, key_buf), "Původní klíč po zvětšení tabulky zmizel");
    }

    symtable_destroy(table);
    for (int i = 0; i < COUNT; i++) {
        ast_destroy(nodes[i]);
    }
    return true;
}

/* ========================================================================= */
/*                               HLAVNÍ SPOUŠTĚČ                             */
/* ========================================================================= */

int main(void) {
    printf("=== SPUŠTĚNÍ TESTŮ PRO SYMTABLE (TRP / OPEN ADDRESSING) ===\n\n");

    RUN_TEST(test_init_and_destroy);
    RUN_TEST(test_invalid_arguments);
    RUN_TEST(test_insert_and_lookup_basic);
    RUN_TEST(test_node_kinds);
    RUN_TEST(test_collisions);
    RUN_TEST(test_duplicate_keys);
    RUN_TEST(test_table_grows);

    printf("\n--------------------------------------------------\n");
    printf("Souhrn: %d testů spuštěno, %d selhalo.\n", tests_run, tests_failed);
    printf("--------------------------------------------------\n");

    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
