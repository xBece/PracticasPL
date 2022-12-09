#ifndef _CODE_GEN_H
#define _CODE_GEN_H

#include <stddef.h>
#include <stdarg.h>

typedef struct {
	const char* ptr;
	size_t size;
} string;

typedef struct {
	const char* lugar;
	string code;
} CodeGen;

void add_code(CodeGen* code_gen, ...) {
}

	add_code($$.code_gen, "%s; %s = %s", $3.code_gen.code, $1.code_gen.lugar, $3.code_gen.lugar);

#endif
