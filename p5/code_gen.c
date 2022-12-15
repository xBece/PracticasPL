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

// Crea una nueva cadena con la concatenacion de `s` y `add`.
char* string_add(const char* s, const char* add) {
	size_t add_len = strlen(add);
	assert(add_len);
	size_t curr_len = (s ? strlen(s) : 0);
	size_t new_len = curr_len + add_len + 1;
	char* result = malloc(new_len);
	if (s)
		strcpy(result, s);
	strcat(result, add);
	return result;
}

char* string_add_fmt(const char* s, const char* format, ...) {
	char* add;
	va_list arg_list;
	va_start(arg_list, format);
	int ret = vasprintf(&add, format, arg_list);
	va_end(arg_list);
	assert(ret != -1);

	char* result = string_add(s, add);
	free(add);
	return result;
}

// Añade a `s` la cadena `add`
void string_add_in_place(char** s, const char* add) {
	char* prev_s = *s;
	*s = string_add(*s, add);
	free(prev_s);
}

void gen_start(void) {
	gen_add_code(
		"#include <stdio.h>\n"
		"#include <stdbool.h>\n"
		"#include \"listas.c\"\n\n"
		"int main() {\n"
	);
	g_curr_indent_level += 1;
}

void gen_end(void) {
	assert(g_curr_indent_level == 1);
	g_curr_indent_level -= 1;
	gen_add_code("}\n");

	FILE* f = fopen("generated.c", "w");
	if (!f) {
		fprintf(stderr, "error al abrir archivo generated.c como escritura");
		return;
	}
	fprintf(f, "%s\n", g_all_code);
	fclose(f);
}

static void gen_add_code_aux(const char* format, va_list arg_list) {
	// Parse format string to get code
	char* code;
	int ret = vasprintf(&code, format, arg_list);
	assert(ret != -1);

	// Add code
	string_add_in_place(&g_all_code, code);

	// Free resources
	free(code);
}

// Añade el código dado a la variable global `g_all_code` sin indentación.
void gen_add_code_no_indent(const char* format, ...) {
	va_list arg_list;
	va_start(arg_list, format);
	gen_add_code_aux(format, arg_list);
	va_end(arg_list);
}

// Añade el código dado a la variable global `g_all_code`.
void gen_add_code(const char* format, ...) {
	// Add indentation
	if (g_curr_indent_level > 0) {
		size_t indent_size = g_curr_indent_level * TAB_SIZE;
		char indent[indent_size + 1];
		memset(indent, ' ', indent_size);
		indent[indent_size] = 0;
		string_add_in_place(&g_all_code, indent);
	}

	va_list arg_list;
	va_start(arg_list, format);
	gen_add_code_aux(format, arg_list);
	va_end(arg_list);
}

const char* tipo_var_to_str(TipoVar tipo_var) {
	switch (tipo_var) {
		case Int:
			return "int";
		case Bool:
			return "bool";
		case Float:
			return "float";
		case Char:
			return "char";
		case ListBool:
		case ListChar:
		case ListFloat:
		case ListInt:
			return "List";
		case Desconocido:
			return "DESCONOCIDO";
		default:
			assert(false);
	}
}

const char* tipo_bool_to_str(TipoBool tipo_bool) {
	switch (tipo_bool) {
		case True:
			return "true";
		case False:
			return "false";
		default:
			assert(false);
	}
};

const char* tipo_var_to_format(TipoVar tipo_var) {
	switch (tipo_var) {
		case Int:
			return "%d";
		case Bool:
			return "%d";
		case Float:
			return "%f";
		case Char:
			return "%c";
		default:
			assert(false);
	}
}

const char* op_binario_to_symbol(TipoOpBinario tipo_op) {
	switch (tipo_op) {
		case Suma:
			return "+";
		case Div:
			return "/";
		case Mult:
			return "*";
		case Greater:
			return ">";
		case Less:
			return "<";
		case GreaterEq:
			return ">=";
		case LessEq:
			return "<=";
		case NotEq:
			return "!=";
		case Eq:
			return "==";
		case And:
			return "&&";
		case Or:
			return "||";
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

	gen_new_var(result, tipo);

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
	gen_add_code("{\n");
	g_curr_indent_level += 1;
}

void gen_end_sentencia(void) {
	g_curr_indent_level -= 1;
	gen_add_code("}\n");
}

void gen_new_var(const char* name, TipoVar tipo) {
	if (is_list_type(tipo))
		gen_add_code("%s %s = list_create(NULL, 0, sizeof(%s));\n",
		             tipo_var_to_str(tipo), name, tipo_var_to_str(get_tipo_basico(tipo)));
	else
		gen_add_code("%s %s;\n", tipo_var_to_str(tipo), name);
}
