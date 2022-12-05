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

int get_tipo_basico(TipoVar tipo_lista) {
	return tipo_lista - 4;
}

bool is_op_rel(TipoOpBinario tipo_op) {
	return tipo_op == Greater || tipo_op == Less || tipo_op == GreaterEq ||
	       tipo_op == LessEq || tipo_op == NotEq || tipo_op == Eq;
}

static const char* tipo_var_to_str(TipoVar tipo_var);
static const char* tipo_simbolo_to_str(TipoSimbolo tipo);
static const char* tipo_op_bin_to_str(TipoOpBinario tipo);
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
	// printf("insertando variable %s de tipo %s\n", lex, tipo_var_to_str(tipo_var));
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
	// printf("insertando funcion %s\n", lex);
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
	// printf("insertando parametro %s de tipo %s\n", lex, tipo_var_to_str(tipo_var));
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

// Comprueba si el simbolo está
void ts_check(const char* lex) {
	// printf("checking if symbol %s exists\n", lex);
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

// #define ts_check_types(tipo1, tipo2, ...)
// 	if (tipo1 != tipo2) {
// 		error_semantico("")
// 	}
void ts_check_types(TipoVar tipo1, TipoVar tipo2, const char* msg){
	if(tipo1 != tipo2){
		error_semantico("%s: %s, %s", msg, tipo_var_to_str(tipo1), tipo_var_to_str(tipo2));
	}
}

static int ts_get_num_args(int i_proc) {
	// Loop over parameters, counting them
	int n_args = 0;
	for (int i = i_proc + 1; i < ts_size && ts[i].tipo == Param; i++) 
		n_args++;
	return n_args;
}

void ts_check_arg_type(const char* func_name, int arg_index, TipoVar tipo) {
	int i_proc = ts_search(func_name);
	int n_args = ts_get_num_args(i_proc);

	// Check if we are in bounds. If not, error will be given by `ts_check_num_args`.
	if (arg_index >= n_args) {
		return;
	}

	// Get argument and check its type
	assert(i_proc + n_args - arg_index < ts_size);
	Symbol arg = ts[i_proc + n_args - arg_index];
	assert(arg.tipo == Param);
	if (arg.tipo_var != tipo) {
		error_semantico("parámetro %d en la llamada a %s es de tipo %s, pero se esperaba %s",
		                arg_index+1, func_name, tipo_var_to_str(tipo), tipo_var_to_str(arg.tipo_var));
	}
}

void ts_check_num_args(const char* func_name, int n_given_args) {
	int i_proc = ts_search(func_name);
	int n_args = ts_get_num_args(i_proc);
	if (n_given_args != n_args) {
		error_semantico("llamada a %s con %d argumentos, se esperaban %d",
		                func_name, n_given_args, n_args);
	}
}

bool ts_is_var(const char* var_name) {
	int i = ts_search(var_name);
	if (i == -1)
		return false;
	return ts[i].tipo == Var || ts[i].tipo == Param;
}

TipoVar ts_get_var_type(const char* var_name) {
	int i = ts_search(var_name);
	if (i == -1 || (ts[i].tipo != Var && ts[i].tipo != Param))
		return Desconocido;
	return ts[i].tipo_var;
}

void ts_check_op_un(TipoVar tipo_var, TipoOpUnario tipo_op) {
	if(tipo_op == LengthOf && tipo_var != ListBool && tipo_var != ListChar &&
	   tipo_var != ListFloat && tipo_var != ListInt)
		error_semantico("Tipo de operando en operador de longitud erróneo: %s,"
		                "se esperaba una lista", tipo_var_to_str(tipo_var));
	if(tipo_op == Not && tipo_var != Bool)
		error_semantico("Tipo de operando en operador de negación erróneo: %s, se esperaba bool",
		                tipo_var_to_str(tipo_var));
}

void ts_check_menos_un(TipoVar tipo) {
	if(tipo != Int && tipo != Float)
		error_semantico("Tipo de operando en operador unario menos erróneo: %s, se esperaba un int o float", tipo_var_to_str(tipo));
}

static void ts_check_op_bin_operands(TipoVar tipo_var1, TipoVar tipo_var2, const char* tipo_op_Str) {
	if(tipo_var1 != Int && tipo_var1 != Float)
		error_semantico("Tipo de primer operando en operador binario %s erróneo: %s, "
						"se esperaba un int o float", tipo_op_Str, tipo_var_to_str(tipo_var1));
	if(tipo_var2 != Int && tipo_var2 != Float)
		error_semantico("Tipo de segundo operando en operador binario %s erróneo: %s, "
						"se esperaba un int o float", tipo_op_Str, tipo_var_to_str(tipo_var2));
}

void ts_check_op_bin(TipoVar tipo_var1, TipoVar tipo_var2, TipoOpBinario tipo_op) {
	if(tipo_var1 != tipo_var2)
		error_semantico("Tipos de operandos en operador binario %s distintos: %s, %s",
		    tipo_op_bin_to_str(tipo_op), tipo_var_to_str(tipo_var1), tipo_var_to_str(tipo_var2));

	if(tipo_op == Suma || tipo_op == Div || tipo_op == Mult || tipo_op == Greater ||
	   tipo_op == Less || tipo_op == GreaterEq || tipo_op == LessEq)
	{
		ts_check_op_bin_operands(tipo_var1, tipo_var2, tipo_op_bin_to_str(tipo_op));
	}
	if(tipo_op == And || tipo_op == Or) {
		if(tipo_var1 != Bool)
			error_semantico("Tipo de primer operando en operador binario %s erróneo: %s, "
			                "se esperaba un bool", tipo_op_bin_to_str(tipo_op), tipo_var_to_str(tipo_var1));
		if(tipo_var2 != Bool)
			error_semantico("Tipo de segundo operando en operador binario %s erróneo: %s, "
			                "se esperaba un bool", tipo_op_bin_to_str(tipo_op), tipo_var_to_str(tipo_var2));
	}
}

void ts_check_menos_bin(TipoVar tipo1, TipoVar tipo2) {
	ts_check_types(tipo1, tipo2, "Tipos de operandos en operador binario menos distintos");
	ts_check_op_bin_operands(tipo1, tipo2, "Menos");
}

static bool is_list_type(TipoVar tipo_lista) {
	return tipo_lista == ListBool || tipo_lista == ListChar || tipo_lista == ListFloat || tipo_lista == ListInt;
}

void ts_check_list_insert(TipoVar tipo_lista, TipoVar tipo_indice, TipoVar tipo_dato) {
	if (!is_list_type(tipo_lista))
		error_semantico("Tipo de identificador en inserción en lista erróneo: %s, se esperaba una lista", tipo_var_to_str(tipo_lista));

	if(tipo_indice != Int)
		error_semantico("Tipo de índice en inserción en lista erróneo: %s, se esperaba int", tipo_var_to_str(tipo_indice));

	if(get_tipo_lista(tipo_dato) != tipo_lista)
		error_semantico("Tipo de dato en inserción en lista erróneo: %s, se esperaba %s", tipo_var_to_str(tipo_dato), tipo_var_to_str(get_tipo_basico(tipo_lista)));
}

void ts_check_list_remove(TipoVar tipo_lista, TipoVar tipo_indice) {
	if (!is_list_type(tipo_lista))
		error_semantico("Tipo de identificador en remove en lista erróneo: %s, se esperaba una lista", tipo_var_to_str(tipo_lista));
	if (tipo_indice != Int)
		error_semantico("Tipo de índice en remove en lista erróneo: %s, se esperaba int", tipo_var_to_str(tipo_indice));
}

void ts_check_list_access(TipoVar tipo_lista, TipoVar tipo_indice) {
	if (!is_list_type(tipo_lista))
		error_semantico("Tipo de identificador en acceso a lista erróneo: %s, se esperaba una lista", tipo_var_to_str(tipo_lista));
	if (tipo_indice != Int)
		error_semantico("Tipo de índice en acceso a lista erróneo: %s, se esperaba int", tipo_var_to_str(tipo_indice));
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
		case Desconocido:
			return "desconocido";
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

static const char* tipo_op_bin_to_str(TipoOpBinario tipo) {
	switch (tipo) {
		case Suma:
			return "Suma";
		case Div:
			return "Div";
		case Mult:
			return "Mult";
		case Greater:
			return "Greater";
		case Less:
			return "Less";
		case GreaterEq:
			return "GreaterEq";
		case LessEq:
			return "LessEq";
		case NotEq:
			return "NotEq";
		case Eq:
			return "Eq";
		case And:
			return "And";
		case Or:
			return "Or";
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