#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "code_gen.h"

#define TAB_SIZE 4

size_t g_curr_indent_level = 0;
char* g_all_code;

static void string_add(char** s, const char* add) {
	size_t add_len = strlen(add);
	if (add_len == 0)
		return;

	size_t curr_len = (*s ? strlen(*s) : 0);
	*s = realloc(*s, curr_len + add_len + 1);
	strcat(*s, add);
}

void gen_start(void) {
	gen_add_code("int main() {");
	g_curr_indent_level += 1;
}

void gen_end(void) {
	assert(g_curr_indent_level == 1);
	g_curr_indent_level -= 1;
	gen_add_code("}");
	printf("%s\n", g_all_code);
	// TODO: guardar archivo
}

// Añade el código dado a la variable global `g_all_code`.
void gen_add_code(const char* format, ...) {
	// Add indentation
	if (g_curr_indent_level > 0) {
		size_t indent_size = g_curr_indent_level * TAB_SIZE;
		char indent[indent_size + 1];
		memset(indent, ' ', indent_size);
		indent[indent_size] = 0;
		string_add(&g_all_code, indent);
	}

	// Parse format string to get code
	char* code;
	va_list arg_list;
	va_start(arg_list, format);
	int ret = vasprintf(&code, format, arg_list);
	assert(ret != -1);

	// Add code
	string_add(&g_all_code, code);
	string_add(&g_all_code, "\n");

	// Free resources
	va_end(arg_list);
	free(code);
}

static const char* tipo_var_to_str(TipoVar tipo_var) {
	switch (tipo_var) {
		case Int:
			return "int";
		case Bool:
			return "bool";
		case Float:
			return "float";
		case Char:
			return "char";
		default:
			assert(false);
	}
}

// Devuelve una cadena tempN, con N un número único empezando en 0.
// Además, genera el código asociado a una variable temporal con ese nombre y el
// tipo dado.
const char* gen_new_temp(TipoVar tipo) {
	static int next_temp = 0;
	int temp = next_temp++;

	char* result;
	int ret = asprintf(&result, "temp%d", temp);
	assert(ret != -1);

	gen_add_code("%s %s;", tipo_var_to_str(tipo), result);

	return result;
}

// Devuelve una cadena etiquetaN, con N un número único empezando en 0
const char* gen_new_etiqueta(void) {
	static int next_etiqueta = 0;
	int etiqueta = next_etiqueta++;

	char* result;
	int ret = asprintf(&result, "etiqueta%d", etiqueta);
	assert(ret != -1);

	return result;
}

void gen_start_sentencia(void) {
	gen_add_code("{");
	g_curr_indent_level += 1;
}

void gen_end_sentencia(void) {
	g_curr_indent_level -= 1;
	gen_add_code("}");
}

void gen_new_var(const char* name, TipoVar tipo) {
	gen_add_code("%s %s;", tipo_var_to_str(tipo), name);
}
