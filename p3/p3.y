%{
/*********************************************************
**
** Fichero: p3.y
** Función: archivo YACC para análisis sintáctico
**
********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** La siguiente declaracion permite que 'yyerror' se pueda invocar desde el
*** fuente de lex (prueba.l)
**/
void yyerror( const char * msg ) ;

%}

%define parse.error verbose

/** A continuación declaramos los nombres simbólicos de los tokens.
*** byacc se encarga de asociar a cada uno un código
**/
%token INICIO
%token INI_VAR
%token FIN_VAR
%token PROC
%token PAR_IZQ
%token PAR_DER
%token COR_IZQ
%token COR_DER
%token PYC
%token INI_BLOQUE
%token FIN_BLOQUE
%token OP_BINARIO
%token OP_UNARIO
%token MENOS
%token BOOL
%token IF
%token WHILE
%token DO
%token UNTIL
%token RETURN
%token READ
%token PRINT
%token MUT
%token ENTERO
%token REAL
%token IDENTIFICADOR
%token TIPO_BASICO
%token LIST
%token INSERT
%token REMOVE
%token CADENA
%token COMA
%token IGUAL
%token CARACTER
%token ELSE

%left OP_BINARIO
%left MENOS
%left OP_UNARIO

%start programa

%%
// PRODUCCIONES
programa                    : INICIO bloque;
bloque                      : inicio_de_bloque 
                              declar_variables_locales
                              declar_de_subprogs
                              sentencias
                              fin_de_bloque
                            ;
declar_de_subprogs          : declar_de_subprogs declar_subprog
                            |
                            ;
declar_subprog              : cabecera_subprograma bloque;
declar_variables_locales    : marca_ini_declar_variables
                              variables_locales
                              marca_fin_declar_variables
                            |
                            ;
marca_ini_declar_variables  : INI_VAR;
marca_fin_declar_variables  : FIN_VAR;
inicio_de_bloque            : INI_BLOQUE;
fin_de_bloque               : FIN_BLOQUE;
variables_locales           : variables_locales cuerpo_declar_variables
                            | cuerpo_declar_variables
                            ;
cuerpo_declar_variables     : tipo lista_variables PYC
                            | error
                            ;
cabecera_subprograma        : PROC IDENTIFICADOR PAR_IZQ lista_parametros PAR_DER
                            ;
sentencias                  : sentencias sentencia
                            |
                            ;
sentencia                   : bloque
                            | sentencia_asignacion PYC
                            | sentencia_if
                            | sentencia_while
                            | sentencia_entrada PYC
                            | sentencia_salida PYC
                            | sentencia_llamada_proc PYC
                            | sentencia_do_until PYC
                            | sentencia_anadir_elemento PYC
                            | sentencia_eliminar_elemento PYC
                            | sentencia_return PYC
                            | error
                            ;

sentencia_asignacion        : IDENTIFICADOR IGUAL expresion
                            | acceso_lista IGUAL expresion
                            ;

sentencia_if                : IF PAR_IZQ expresion PAR_DER sentencia
                            | IF PAR_IZQ expresion PAR_DER sentencia ELSE sentencia
                            ;

sentencia_while             : WHILE PAR_IZQ expresion PAR_DER sentencia;

sentencia_entrada           : READ lista_variables;

sentencia_salida            : PRINT lista_expresiones_o_cadena;

sentencia_do_until          : DO PAR_IZQ sentencia PAR_DER UNTIL PAR_IZQ expresion PAR_DER;

sentencia_return            : RETURN;

expresion                   : PAR_IZQ expresion PAR_DER
                            | OP_UNARIO expresion
                            | MENOS expresion
                            | expresion OP_BINARIO expresion
                            | expresion MENOS expresion
                            | IDENTIFICADOR
                            | constante
                            | acceso_lista
                            ;

acceso_lista                : IDENTIFICADOR COR_IZQ expresion COR_DER;
sentencia_anadir_elemento   : INSERT PAR_IZQ IDENTIFICADOR COMA expresion COMA expresion PAR_DER;
sentencia_eliminar_elemento : REMOVE PAR_IZQ IDENTIFICADOR COMA expresion PAR_DER;
sentencia_llamada_proc      : IDENTIFICADOR PAR_IZQ lista_expresiones PAR_DER
                            ;
lista_expresiones_o_cadena  : lista_expresiones_o_cadena COMA expresion
                            | lista_expresiones_o_cadena COMA CADENA
                            | CADENA
                            | expresion
                            ;
lista_expresiones           : lista_expresiones COMA expresion
                            | expresion
                            |
                            ;
lista_variables             : IDENTIFICADOR COMA lista_variables
                            | IDENTIFICADOR
                            ;
lista_parametros            : parametro COMA lista_parametros
                            | parametro
                            | error
                            |
                            ;

parametro                   : MUT tipo IDENTIFICADOR
                            | tipo IDENTIFICADOR
                            ;
tipo                        : TIPO_BASICO
                            | LIST TIPO_BASICO
                            ;
constante                   : ENTERO
                            | REAL
                            | CARACTER
                            | BOOL
                            | lista
                            ;

lista                       : COR_IZQ lista_expresiones COR_DER;



%%

#include "lex.yy.c"

void yyerror(const char *msg) {
	fprintf(stderr, "[Linea %d]: %s\n", yylineno, msg);
}