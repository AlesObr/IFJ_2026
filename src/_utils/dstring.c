/**
 * @project IFJ26 Compiler
 * @file dstring.c
 * @brief Implementation of dynamic string library to be used for work with strings
 * @author Aleš Obr (xobrale00)
 * @note Standard: ISO C11
 */

#include "dstring.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DSTRING_INIT_CAPACITY 16

bool dstring_init(dstring_t *str) {
	if (str == NULL) {
		return false;
	}
	str->data = malloc(DSTRING_INIT_CAPACITY);
	if (str->data == NULL) {
		str->length = 0;
		str->capacity = 0;
		return false;
	}
	str->data[0] = '\0';
	str->length = 0;
	str->capacity = DSTRING_INIT_CAPACITY;
	return true;
}

void dstring_free(dstring_t *str) {
	if (str == NULL) {
		return;
	}
	if (str->data != NULL) {
		free(str->data);
		str->data = NULL;
	}
	str->length = 0;
	str->capacity = 0;
}

void dstring_clear(dstring_t *str) {
	if (str == NULL) {
		return;
	}
	if (str->data != NULL) {
		str->data[0] = '\0';
	}
	str->length = 0;
	// Note: We do not free the memory here, just reset the length and null-terminate the string.
}

bool dstring_append_char(dstring_t *str, char c) {
	if (str == NULL) {
		return false;
	}

	// Note: We need to account for the null terminator, so we add 2 to the length.
	if (str->length + 2 > str->capacity) {
		size_t new_capacity = (str->capacity == 0) ? DSTRING_INIT_CAPACITY : str->capacity * 2;
		char *new_data = realloc(str->data, new_capacity);
		if (new_data == NULL) {
			return false;
		}
		str->data = new_data;
		str->capacity = new_capacity;
	}

	str->data[str->length] = c;
	str->length++;
	str->data[str->length] = '\0';

	return true;
}

bool dstring_append_mem(dstring_t *str, const char *data, size_t length) {
	if (str == NULL || (data == NULL && length > 0)) {
		return false;
	}
	if (length == 0) {
		return true;
	}

	if (str->length + length + 1 > str->capacity) {
		size_t new_capacity = (str->capacity == 0) ? DSTRING_INIT_CAPACITY : str->capacity;
		while (new_capacity < str->length + length + 1) {
			new_capacity *= 2;
		}
		char *new_data = realloc(str->data, new_capacity);
		if (new_data == NULL) {
			return false;
		}
		str->data = new_data;
		str->capacity = new_capacity;
	}

	memcpy(str->data + str->length, data, length);
	str->length += length;
	str->data[str->length] = '\0';

	return true;
}

bool dstring_append_str(dstring_t *str, const char *text) {
	if (str == NULL || text == NULL) {
		return false;
	}
	return dstring_append_mem(str, text, strlen(text));
}

const char *dstring_get(const dstring_t *str) {
	if (str == NULL || str->data == NULL) {
		return "";
	}
	return str->data;
}

const char *dstring_get_raw(const dstring_t *str) {
	return dstring_get(str);
}

int dstring_cmp_str(const dstring_t *str, const char *text) {
	const char *str_val = (str != NULL && str->data != NULL) ? str->data : "";
	const char *text_val = (text != NULL) ? text : "";
	return strcmp(str_val, text_val);
}

bool dstring_copy(dstring_t *dest, const dstring_t *src) {
	if (dest == NULL) {
		return false;
	}

	// Source is NULL or completely empty
	if (src == NULL || src->data == NULL || src->length == 0) {
		if (dest->data == NULL) {
			return dstring_init(dest);
		}
		dstring_clear(dest);
		return true;
	}

	// Ensure capacity for string + null terminator
	if (src->length + 1 > dest->capacity) {
		size_t new_capacity = (dest->capacity == 0) ? DSTRING_INIT_CAPACITY : dest->capacity;
		while (new_capacity < src->length + 1) {
			new_capacity *= 2;
		}
		char *new_data = realloc(dest->data, new_capacity);
		if (new_data == NULL) {
			return false;
		}
		dest->data = new_data;
		dest->capacity = new_capacity;
	}

	// Copy the raw bytes and ensure null-termination
	memcpy(dest->data, src->data, src->length);
	dest->length = src->length;
	dest->data[dest->length] = '\0';

	return true;
}
