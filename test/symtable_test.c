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

/* --- Pomocná funkce pro vytvoření testovacího ASTNode --- */
static ASTNode* create_mock_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;

    // Upravte podle vaší reálné struktury ASTNode:
    // Předpokládáme například: node->identifier = strdup(name);
    // Pokud ASTNode obsahuje char identif[...], přizpůsobte:
    #ifdef MOCK_AST_FIELD
    node->name = strdup(name);
    #endif

    return node;
}

static void free_mock_node(ASTNode* node) {
    if (!node) return;
    #ifdef MOCK_AST_FIELD
    free(node->name);
    #endif
    free(node);
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
    return true;
}

/**
 * 2. Vložení a vyhledání jednoho i více základních prvků
 */
static bool test_insert_and_lookup_basic(void) {
    symtable* table = symtable_init(17);
    ASSERT(table != NULL, "Chyba alokace tabulky");

    ASTNode* node_a = create_mock_node("var_a");
    ASTNode* node_b = create_mock_node("var_b");

    ASSERT(symtable_insert(table, node_a) == true, "Vložení prvku var_a selhalo");
    ASSERT(symtable_insert(table, node_b) == true, "Vložení prvku var_b selhalo");

    // Ověření přítomnosti
    ASSERT(symtable_search(table, "var_a"), "Nenalezen vložený prvek var_a");
    ASSERT(symtable_search(table, "var_b"), "Nenalezen vložený prvek var_b");
    ASSERT(!symtable_search(table, "nonexistent"), "Falešná detekce nevloženého prvku");

    // Kontrola vrácených dat
    const symbol* s = symtable_get_data(table, "var_a");
    ASSERT(s != NULL, "symtable_get_data vrátilo NULL pro existující prvek");
    ASSERT(strcmp(s->identif, "var_a") == 0, "Identifikátor symbolu se neshoduje");

    symtable_destroy(table);
    free_mock_node(node_a);
    free_mock_node(node_b);
    return true;
}

/**
 * 3. Test kolizí (Collision Handling v otevřeném adresování)
 * Vkládáme dostatek prvků, aby nevyhnutelně nastaly kolize indexů.
 */
static bool test_collisions(void) {
    // Malá prvočíselná kapacita pro vynucení kolizí
    const int capacity = 7;
    symtable* table = symtable_init(capacity);
    ASSERT(table != NULL, "Chyba alokace");

    const char* keys[] = {"k1", "k2", "k3", "k4", "k5"};
    const int count = sizeof(keys) / sizeof(keys[0]);
    ASTNode* nodes[5];

    for (int i = 0; i < count; i++) {
        nodes[i] = create_mock_node(keys[i]);
        ASSERT(symtable_insert(table, nodes[i]) == true, "Vložení při kolizi selhalo");
    }

    // Ověření, že všechny kolidující prvky jsou stále k nalezení
    for (int i = 0; i < count; i++) {
        ASSERT(symtable_search(table, (char*)keys[i]), "Kolidující klíč nebyl nalezen");
        const symbol* s = symtable_get_data(table, (char*)keys[i]);
        ASSERT(s != NULL && strcmp(s->identif, keys[i]) == 0, "Data kolidujícího klíče jsou neplatná");
    }

    symtable_destroy(table);
    for (int i = 0; i < count; i++) {
        free_mock_node(nodes[i]);
    }
    return true;
}

/**
 * 4. Duplicitní vkládání stejného identifikátoru
 */
static bool test_duplicate_keys(void) {
    symtable* table = symtable_init(13);
    ASSERT(table != NULL, "Chyba alokace");

    ASTNode* node1 = create_mock_node("shadow_var");
    ASTNode* node2 = create_mock_node("shadow_var");

    ASSERT(symtable_insert(table, node1) == true, "První vložení shadow_var selhalo");

    // Záleží na specifikaci IFJ: buď se vložení duplikátu odmítne (vrátí false),
    // nebo se aktualizuje. V kompilátoru obvykle duplicitní deklarace v témže scope selže:
    bool res2 = symtable_insert(table, node2);
    ASSERT(res2 == false, "Vložení duplicitního identifikátoru do stejné tabulky mělo selhat");

    symtable_destroy(table);
    free_mock_node(node1);
    free_mock_node(node2);
    return true;
}

/**
 * 5. Mezní stav: Zcela zaplněná tabulka (Boundary condition)
 * U implicitního zřetězení nelze překročit kapacitu pole (load factor = 1.0).
 */
static bool test_table_full(void) {
    const int capacity = 5;
    symtable* table = symtable_init(capacity);
    ASSERT(table != NULL, "Chyba alokace");

    ASTNode* nodes[5];
    char key_buf[16];

    for (int i = 0; i < capacity; i++) {
        snprintf(key_buf, sizeof(key_buf), "id_%d", i);
        nodes[i] = create_mock_node(key_buf);
        ASSERT(symtable_insert(table, nodes[i]) == true, "Zaplnění tabulky selhalo");
    }

    // Pokus o vložení prvku nad kapacitu
    ASTNode* overflow_node = create_mock_node("overflow_id");
    bool overflow_res = symtable_insert(table, overflow_node);
    ASSERT(overflow_res == false, "Vložení do plné tabulky s otevřenou adresací musí selhat");

    // Zkontrolujeme, že existující prvky zůstaly nepoškozené
    for (int i = 0; i < capacity; i++) {
        snprintf(key_buf, sizeof(key_buf), "id_%d", i);
        ASSERT(symtable_search(table, key_buf), "Původní klíč po pokusu o přetečení zmizel");
    }

    symtable_destroy(table);
    for (int i = 0; i < capacity; i++) {
        free_mock_node(nodes[i]);
    }
    free_mock_node(overflow_node);
    return true;
}

/* ========================================================================= */
/*                               HLAVNÍ SPOUŠTĚČ                             */
/* ========================================================================= */

int main(void) {
    printf("=== SPUŠTĚNÍ TESTŮ PRO SYMTABLE (TRP / OPEN ADDRESSING) ===\n\n");

    RUN_TEST(test_init_and_destroy);
    RUN_TEST(test_insert_and_lookup_basic);
    RUN_TEST(test_collisions);
    RUN_TEST(test_duplicate_keys);
    RUN_TEST(test_table_full);

    printf("\n--------------------------------------------------\n");
    printf("Souhrn: %d testů spuštěno, %d selhalo.\n", tests_run, tests_failed);
    printf("--------------------------------------------------\n");

    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
