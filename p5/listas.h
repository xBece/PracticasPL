#include <stddef.h>

typedef struct {
	void* buffer;
	size_t length;
	size_t elem_size;
} List;

List list_create(void* buffer, size_t length, size_t elem_size);
void list_insert(List* list, size_t i, const void* elem);
void* list_ptr_at(List* list, size_t i);
#define list_at(list, i, type) *(type*)list_ptr_at(list, i)
void list_remove(List* list, size_t i);
size_t list_length(List* list);
