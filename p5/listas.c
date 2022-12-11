#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "listas.h"

static size_t mul_check(size_t a, size_t b) {
	// https://stackoverflow.com/questions/1815367/catch-and-compute-overflow-during-multiplication-of-two-large-integers
	size_t x = a*b;
	if (a != 0 && x/a != b) {
		printf("Runtime error: multiplication overflow\n");
		exit(EXIT_FAILURE);
	}
	return x;
}

List list_create(void* buffer, size_t length, size_t elem_size) {
	assert(elem_size > 0);
	size_t sz = mul_check(length, elem_size);
	void* list_buf = malloc(sz);
	memcpy(list_buf, buffer, sz);
	return (List){
		.buffer = list_buf,
		.length = length,
		.elem_size = elem_size,
	};
};

void list_insert(List* list, size_t i, const void* elem) {
	if (i > list->length) {
		printf("Runtime error: intentando insertar en indice %lu de lista de tamaño %lu\n", i, list->length);
		exit(EXIT_FAILURE);
	}

	// Allocate space for one more element
	list->length++;
	list->buffer = realloc(list->buffer, mul_check(list->length, list->elem_size));

	// Move every element from position i onwards one to the right, making space
	// for the new element
	memmove(
		list->buffer + (i+1)*list->elem_size,
		list->buffer + i*list->elem_size,
		(list->length-1 - i)*list->elem_size // CHECK ME
	);

	// Copy the new element
	memcpy(list->buffer + i*list->elem_size, elem, list->elem_size);
}

void* list_ptr_at(List* list, size_t i) {
	if (i >= list->length) {
		printf("Runtime error: accediendo indice %lu de lista de tamaño %lu\n", i, list->length);
		exit(EXIT_FAILURE);
	}
	size_t offset = mul_check(i, list->elem_size);
	return list->buffer + offset;
}

void list_remove(List* list, size_t i) {
	if (i >= list->length) {
		printf("Runtime error: accediendo indice %lu de lista de tamaño %lu\n", i, list->length);
		exit(EXIT_FAILURE);
	}

	// Move every element from position i+1 onwards one to the left, removing
	// element i
	memmove(
		list->buffer + i*list->elem_size,
		list->buffer + (i+1)*list->elem_size,
		(list->length - (i+1))*list->elem_size // CHECK ME
	);

	// Update length. No need to realloc here.
	list->length--;
}

size_t list_length(List* list) {
	return list->length;
}