/**
 * @project IFJ26 Compiler
 * @file dstring.h
 * @brief Dstring module interface for IFJ26 compiler.
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#ifndef IFJ26_DSTRING_H
#define IFJ26_DSTRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


/**
 * @brief Dynamic string structure.
 */
typedef struct {
	char *data;
	size_t length;
	size_t capacity;
} dstring_t;


/**
 * @brief Initialize a dstring.
 * @param str The dstring to initialize.
 * @return true if successful, false otherwise.
 */
bool dstring_init(dstring_t *str);

/**
 * @brief Free the memory allocated for a dstring.
 * @param str The dstring to free.
 */
void dstring_free(dstring_t *str);

/**
 * @brief Clear the contents of a dstring.
 * @param str The dstring to clear.
 */
void dstring_clear(dstring_t *str);

/**
 * @brief Append a character to a dstring.
 * @param str The dstring to append to.
 * @param c The character to append.
 * @return true if successful, false otherwise.
 */
bool dstring_append_char(dstring_t *str, char c);

/**
 * @brief Append a string to a dstring.
 * @param str The dstring to append to.
 * @param text The string to append.
 * @return true if successful, false otherwise.
 */
bool dstring_append_str(dstring_t *str, const char *text);

/**
 * @brief Get the C-style string from a dstring.
 * @param str The dstring to get the string from.
 * @return The C-style string.
 */
const char *dstring_get(const dstring_t *str);

/**
 * @brief Get the raw C-style string from a dstring (same as dstring_get).
 * @param str The dstring to get the string from.
 * @return The raw C-style string.
 */
const char *dstring_get_raw(const dstring_t *str);

/**
 * @brief Append a block of memory to a dstring.
 * @param str The dstring to append to.
 * @param data The memory block to append.
 * @param length The length of the memory block.
 * @return true if successful, false otherwise.
 */
bool dstring_append_mem(dstring_t *str, const char *data, size_t length);

/**
 * @brief Compare a dstring with a C-style string.
 * @param str The dstring to compare.
 * @param text The C-style string to compare with.
 * @return 0 if equal, negative if str is less than text, positive if str is greater than text.
 */
int dstring_cmp_str(const dstring_t *str, const char *text);

/**
 * @brief Copy the contents of one dstring to another.
 * @param dest The dstring to copy to.
 * @param src The dstring to copy from.
 * @return true if successful, false otherwise.
 */
bool dstring_copy(dstring_t *dest, const dstring_t *src);

#endif /* IFJ26_DSTRING_H */
