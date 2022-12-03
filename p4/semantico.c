#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "semantico.h"
#include "assert.h"
#include "string.h"

#define MAX_SYMBOLS 100

extern int yylineno;

#define error_semantico(...) do {            \
	printf("[Línea %d] ", yylineno);         \
	printf("Error semántico: " __VA_ARGS__); \
	printf("\n");                            \
} while(0)

typedef enum {
	Var,
	Function,
	Mark,
	Param,
} TipoSimbolo;

typedef struct {
	const char* name;
	TipoSimbolo tipo;
	TipoVar tipo_var;
} Symbol;

static Symbol ts[MAX_SYMBOLS];
static int ts_size = 0;

int get_tipo_lista(TipoVar tipo_basico) {
	return tipo_basico + 4;
}

static const char* tipo_var_to_str(TipoVar tipo_var);
static const char* tipo_simbolo_to_str(TipoSimbolo tipo);
static void ts_dump(void);

// Devuelve indice si `lex` existe en la tabla de simbolos, o -1 en caso contrario.
// No incluye los parametros de una funcion del nivel actual.
static int ts_search(const char* lex) {
	bool found_mark = false;
	for (int i = ts_size - 1; i >= 0; i--) {
		if (ts[i].tipo == Mark) {
			found_mark = true;
			continue;
		}
		// Ignore params of the same-level functions
		if (ts[i].tipo == Param && !found_mark)
			continue;
		if (strcmp(ts[i].name, lex) == 0)
			return i;
	}
	return -1;
}

// Devuelve indice si `lex` existe en la tabla de simbolos en el frame actual,
// o -1 en caso contrario. Incluye los parámetros de la función en la que nos
// encontremos si nos encontramos en una.
static int ts_search_in_current_frame(const char* lex) {
	bool found_mark = false;
	for (int i = ts_size - 1; i >= 0; i--) {
		if (ts[i].tipo == Mark) {
			assert(!found_mark);
			found_mark = true;
			continue;
		}
		// Ignore params of the same-level functions
		if (ts[i].tipo == Param && !found_mark)
			continue;
		// Hemos pasado una marca en busca de parametros y no hay ninguno mas
		if (found_mark && ts[i].tipo != Param)
			return -1;
		assert(ts[i].tipo == Var || ts[i].tipo == Function || (ts[i].tipo == Param && found_mark));
		if (strcmp(ts[i].name, lex) == 0)
			return i;
	}

	// Si llegamos aqui debe ser porque estábamos en el primer frame y hemos
	// pasado la marca inicial
	assert(found_mark);
	return -1;
}

// Devuelve indice si `lex` es un parametro ya introducido para la funcion
// actual. Debe llamarse cuando se esté introduciendo parámetros: lee símbolos
// hasta encontrar un símbolo de tipo función, asumiendo que todos son parámetros.
static int ts_search_param(const char* lex) {
	for (int i = ts_size - 1; i >= 0; i--) {
		if (ts[i].tipo == Function)
			return -1;
		assert(ts[i].tipo == Param); // make sure we didnt find any mark
		if (strcmp(ts[i].name, lex) == 0)
			return i;
	}
	assert(false);
}

static void ts_insert(Symbol symbol){
	if (ts_size == MAX_SYMBOLS){
		error_semantico("tabla de símbolos llena");
		return;
	}

	ts[ts_size++] = symbol;
	return;
}

void ts_insert_var(const char* lex, TipoVar tipo_var) {
	if (ts_search_in_current_frame(lex) != -1) {
		error_semantico("ya existe la variable %s", lex);
		return;
	}
	Symbol symbol = {
		.name = lex,
		.tipo = Var,
		.tipo_var = tipo_var
	};
	printf("insertando variable %s de tipo %s\n", lex, tipo_var_to_str(tipo_var));
	ts_insert(symbol);
}

void ts_insert_func(const char* lex) {
	if (ts_search_in_current_frame(lex) != -1) {
		error_semantico("ya existe la función %s", lex);
		return;
	}
	Symbol symbol = {
		.name = lex,
		.tipo = Function,
		.tipo_var = -1
	};
	printf("insertando funcion %s\n", lex);
	ts_insert(symbol);
}

void ts_insert_param(const char* lex, TipoVar tipo_var) {
	if (ts_search_param(lex) != -1) {
		error_semantico("ya existe el parámetro %s", lex);
		return;
	}
	Symbol symbol = {
		.name = lex,
		.tipo = Param,
		.tipo_var = tipo_var
	};
	printf("insertando parametro %s de tipo %s\n", lex, tipo_var_to_str(tipo_var));
	ts_insert(symbol);
}

void ts_insert_mark(void) {
	Symbol symbol = {
		.name = "MARK",
		.tipo = Mark,
		.tipo_var = -1
	};
	ts_insert(symbol);
}

// Comprueba si el simbolo está y es del tipo dado
void ts_check(const char* lex) {
	printf("checking if symbol %s exists\n", lex);
	if (ts_search(lex) == -1)
		error_semantico("no existe la variable %s", lex);
}

void ts_remove_until_mark(void) {
	// Look for mark
	int i_mark = -1;
	for (int i = ts_size; i >= 0; i--) {
		if (ts[i].tipo == Mark) {
			i_mark = i;
			break;
		}
	}

	assert(i_mark != -1);   // Si no se encuentra marca - error
	ts_size = i_mark;
}

static const char* tipo_var_to_str(TipoVar tipo_var){
	switch(tipo_var){
		case Int:
			return "int";
		case Bool:
			return "bool";
		case Float:
			return "float";
		case Char:
			return "char";
		case ListInt:
			return "list int";
		case ListBool:
			return "list bool";
		case ListChar:
			return "list char";
		case ListFloat:
			return "list float";
		default:
			assert(false);
	}
}

static const char* tipo_simbolo_to_str(TipoSimbolo tipo) {
	switch (tipo) {
		case Var:
			return "var";
		case Function:
			return "function";
		case Param:
			return "param";
		case Mark:
			return "mark";
		default:
			assert(false);
	}
}

static void ts_dump(void) {
	printf("dumping ts of size %d\n", ts_size);
	for (int i = 0; i < ts_size; i++) {
		printf("[%d] %s ", i, tipo_simbolo_to_str(ts[i].tipo));
		switch (ts[i].tipo) {
			case Var:
				printf("%s, type %s\n", ts[i].name, tipo_var_to_str(ts[i].tipo_var));
				break;
			case Param:
				printf("%s, type %s\n", ts[i].name, tipo_var_to_str(ts[i].tipo_var));
				break;
			case Function:
				printf("%s\n", ts[i].name);
				break;
			case Mark:
				printf("\n");
				break;
			default:
				assert(false);
		}
	}
}