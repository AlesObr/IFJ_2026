/**
 * @file test_dstring.c
 * @brief Unit tests for dstring_t implementation verifying memory safety,
 *        null-termination invariants, growth policy, and API contracts.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/_utils/dstring.h"

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("[ RUN      ] test_" #name "\n"); \
    test_##name(); \
    printf("[       OK ] test_" #name "\n"); \
} while (0)

TEST(init_and_invariants) {
    dstring_t s;
    assert(dstring_init(&s) == true);
    assert(s.length == 0);
    assert(s.capacity > 0);
    assert(s.data != NULL);
    assert(s.data[0] == '\0');
    assert(strcmp(dstring_get(&s), "") == 0);
    dstring_free(&s);
}

TEST(append_char_and_null_terminator) {
    dstring_t s;
    assert(dstring_init(&s) == true);

    assert(dstring_append_char(&s, 'x') == true);
    assert(dstring_append_char(&s, 'y') == true);
    assert(dstring_append_char(&s, 'z') == true);

    assert(s.length == 3);
    assert(s.data[3] == '\0');
    assert(strcmp(dstring_get(&s), "xyz") == 0);

    dstring_free(&s);
}

TEST(append_str_and_empty_edge_cases) {
    dstring_t s;
    assert(dstring_init(&s) == true);

    // Prázdný řetězec nesmí rozbít stav
    assert(dstring_append_str(&s, "") == true);
    assert(s.length == 0);
    assert(s.data[0] == '\0');

    assert(dstring_append_str(&s, "Hello, ") == true);
    assert(dstring_append_str(&s, "IFJ26!") == true);

    assert(s.length == strlen("Hello, IFJ26!"));
    assert(strcmp(dstring_get(&s), "Hello, IFJ26!") == 0);
    assert(s.data[s.length] == '\0');

    dstring_free(&s);
}

TEST(clear_and_reuse_preserves_capacity) {
    dstring_t s;
    assert(dstring_init(&s) == true);

    assert(dstring_append_str(&s, "token_to_discard") == true);
    size_t allocated_capacity = s.capacity;

    dstring_clear(&s);
    assert(s.length == 0);
    assert(s.data[0] == '\0');
    assert(s.capacity == allocated_capacity); // Nemá dealokovat, jen resetovat

    // Znovupoužití po vyčištění (scanner token buffer)
    assert(dstring_append_char(&s, '4') == true);
    assert(dstring_append_char(&s, '2') == true);
    assert(strcmp(dstring_get(&s), "42") == 0);

    dstring_free(&s);
}

TEST(cmp_str_lexicographical) {
    dstring_t s;
    assert(dstring_init(&s) == true);

    dstring_append_str(&s, "apple");
    assert(dstring_cmp_str(&s, "apple") == 0);
    assert(dstring_cmp_str(&s, "banana") < 0);
    assert(dstring_cmp_str(&s, "aardvark") > 0);

    // Prefix edge case
    assert(dstring_cmp_str(&s, "apples") < 0);
    assert(dstring_cmp_str(&s, "app") > 0);

    dstring_free(&s);
}

TEST(copy_independence) {
    dstring_t src, dest;
    assert(dstring_init(&src) == true);
    assert(dstring_init(&dest) == true);

    assert(dstring_append_str(&src, "payload_original") == true);
    assert(dstring_copy(&dest, &src) == true);

    assert(dest.length == src.length);
    assert(strcmp(dstring_get(&dest), "payload_original") == 0);

    // Ověření deep copy (nezávislost bufferů)
    assert(dest.data != src.data);
    dstring_append_char(&dest, '_');
    dstring_append_char(&dest, '2');

    assert(strcmp(dstring_get(&src), "payload_original") == 0);
    assert(strcmp(dstring_get(&dest), "payload_original_2") == 0);

    dstring_free(&src);
    dstring_free(&dest);
}

TEST(geometric_realloc_stress) {
    dstring_t s;
    assert(dstring_init(&s) == true);

    // 128 KB dat znak po znaku – prověří správnost reallocu a výkon
    const size_t STRESS_SIZE = 128 * 1024;
    for (size_t i = 0; i < STRESS_SIZE; ++i) {
        char c = (char)('a' + (i % 26));
        assert(dstring_append_char(&s, c) == true);
    }

    assert(s.length == STRESS_SIZE);
    assert(s.capacity >= STRESS_SIZE + 1);
    assert(s.data[STRESS_SIZE] == '\0');

    // Náhodná kontrola vzorku
    assert(s.data[0] == 'a');
    assert(s.data[25] == 'z');
    assert(s.data[26] == 'a');

    dstring_free(&s);
}

int main(void) {
    printf("=== RUNNING DSTRING TEST SUITE ===\n");
    RUN_TEST(init_and_invariants);
    RUN_TEST(append_char_and_null_terminator);
    RUN_TEST(append_str_and_empty_edge_cases);
    RUN_TEST(clear_and_reuse_preserves_capacity);
    RUN_TEST(cmp_str_lexicographical);
    RUN_TEST(copy_independence);
    RUN_TEST(geometric_realloc_stress);
    printf("=== ALL DSTRING TESTS PASSED ===\n");
    return 0;
}
