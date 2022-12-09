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
#include <stdbool.h>
#include "semantico.h"
#include "code_gen.h"

/** La siguiente declaracion permite que 'yyerror' se pueda invocar desde el
*** fuente de lex (prueba.l)
**/
void yyerror( const char * msg ) ;

extern char* yytext;
int yylex();

bool inside_dec_var;
int current_declaring_type;

bool inside_cabecera_proc;

const char* inside_llamada_proc;
int arg_idx;

bool inside_list_declaration;

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
%token OP_BINARIO_REL
%token OP_BINARIO_PROD_DIV
%token OP_BINARIO_SUM
%token OP_BINARIO_BOOL
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

// menor precedencia
%left OP_BINARIO_BOOL
%left OP_BINARIO_REL
%left OP_BINARIO_SUM MENOS
%left OP_BINARIO_PROD_DIV
%left OP_UNARIO
// mayor precedencia

%start programa

%%
// PRODUCCIONES
programa                    : { gen_start(); } INICIO bloque { gen_end(); };
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
declar_variables_locales    : marca_ini_declar_variables {inside_dec_var = true;}
                              variables_locales
                              marca_fin_declar_variables {inside_dec_var = false;}
                            |
                            ;
marca_ini_declar_variables  : INI_VAR;
marca_fin_declar_variables  : FIN_VAR;
inicio_de_bloque            : INI_BLOQUE {ts_insert_mark();};
fin_de_bloque               : FIN_BLOQUE {ts_remove_until_mark();};
variables_locales           : variables_locales cuerpo_declar_variables
                            | cuerpo_declar_variables
                            ;
cuerpo_declar_variables     : tipo { current_declaring_type = $1.tipo;} lista_variables {current_declaring_type = -1;} PYC
                            | error
                            ;
cabecera_subprograma        : PROC { inside_cabecera_proc = true; }
                              identificador { ts_insert_func($3.lex); }
                              PAR_IZQ lista_parametros PAR_DER
                              { inside_cabecera_proc = false; }
                            ;
sentencias                  : sentencias sentencia
                            |
                            ;
sentencia                   : { gen_start_sentencia(); } sentencia_ { gen_end_sentencia(); };
sentencia_                  : bloque
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

sentencia_asignacion        : identificador IGUAL expresion {
                                ts_check_types($1.tipo, $3.tipo, "Tipos en operador de asignación erróneos"); 
                                if ($3.lugar != NULL) // TODO: quitar esto cuando este hecho todo para las expresiones
                                  gen_add_code("%s = %s;\n", $1.lugar, $3.lugar);

                            }
                            | acceso_lista IGUAL expresion { ts_check_types($1.tipo, $3.tipo, "Tipos en operador de asignación de listas erróneos"); }
                            ;

sentencia_if                : IF PAR_IZQ expresion PAR_DER sentencia
                            | IF PAR_IZQ expresion PAR_DER sentencia ELSE sentencia
                            ;

sentencia_while             : WHILE PAR_IZQ expresion PAR_DER sentencia;

sentencia_entrada           : READ lista_variables;

sentencia_salida            : PRINT lista_expresiones_o_cadena;

sentencia_do_until          : DO PAR_IZQ sentencia PAR_DER UNTIL PAR_IZQ expresion PAR_DER;

sentencia_return            : RETURN;

expresion                   : PAR_IZQ expresion PAR_DER { $$.tipo = $2.tipo; }
                            | OP_UNARIO expresion { ts_check_op_un($2.tipo, $1.tipo); $$.tipo = ($1.tipo == LengthOf ? Int : $2.tipo); }
                            | MENOS expresion %prec OP_UNARIO { ts_check_menos_un($2.tipo); $$.tipo = $2.tipo; }
                            | expresion OP_BINARIO_BOOL expresion { ts_check_op_bin($1.tipo, $3.tipo, $2.tipo); $$.tipo = $1.tipo; }
                            | expresion OP_BINARIO_PROD_DIV expresion { ts_check_op_bin($1.tipo, $3.tipo, $2.tipo); $$.tipo = $1.tipo; }
                            | expresion OP_BINARIO_REL expresion { ts_check_op_bin($1.tipo, $3.tipo, $2.tipo); $$.tipo = Bool; }
                            | expresion OP_BINARIO_SUM expresion { ts_check_op_bin($1.tipo, $3.tipo, $2.tipo); $$.tipo = $1.tipo; 
                                $$.lugar = gen_new_temp($$.tipo);
                                gen_add_code("%s = %s + %s;\n", $$.lugar, $1.lugar, $3.lugar);
                            }
                            | expresion MENOS expresion { ts_check_menos_bin($1.tipo, $3.tipo); $$.tipo = $1.tipo; }
                            | identificador { $$.tipo = $1.tipo; }
                            | constante { $$.tipo = $1.tipo; }
                            | acceso_lista { $$.tipo = $1.tipo; }
                            ;

acceso_lista                : identificador COR_IZQ expresion COR_DER { ts_check_list_access($1.tipo, $3.tipo); $$.tipo = get_tipo_basico($1.tipo); };
sentencia_anadir_elemento   : INSERT PAR_IZQ identificador COMA expresion COMA expresion PAR_DER { ts_check_list_insert($3.tipo, $5.tipo, $7.tipo); } ;
sentencia_eliminar_elemento : REMOVE PAR_IZQ identificador COMA expresion PAR_DER { ts_check_list_remove($3.tipo, $5.tipo); };
sentencia_llamada_proc      : identificador PAR_IZQ
                              { inside_llamada_proc = $1.lex; arg_idx = 0; }
                              lista_expresiones
                              { ts_check_num_args(inside_llamada_proc, arg_idx); inside_llamada_proc = NULL; arg_idx = -1; }
                              PAR_DER
                            ;
lista_expresiones_o_cadena  : lista_expresiones_o_cadena COMA expresion
                            | lista_expresiones_o_cadena COMA CADENA
                            | CADENA
                            | expresion
                            ;
lista_expresiones           : lista_expresiones COMA expresion_ 
                              { 
                              if(inside_list_declaration) {
                                if (!ts_check_types($1.tipo, $3.tipo, "Tipos de constantes de lista erróneos") && $1.tipo != Desconocido )
                                  $$.tipo = Desconocido;
                                else
                                  $$.tipo = $1.tipo; 
                              }
                              }
                            | expresion_ { $$.tipo = $1.tipo; }
                            |
                            ;
expresion_                  : expresion {
                              if (inside_llamada_proc) {
                                ts_check_arg_type(inside_llamada_proc, arg_idx, $1.tipo);
                                arg_idx++;
                              }
                            };

lista_variables             : identificador COMA lista_variables
                            | identificador
                            ;
identificador               : IDENTIFICADOR
  // NOTAS: no sabemos muy bien si al identificador de la izqd le tenemos que asignar
  // lexema, tipo o ambos.
                              {
                                $$.lex = $1.lex;
                                $$.lugar = $1.lex;
                                if (inside_dec_var) {
                                  ts_insert_var($1.lex, current_declaring_type);
                                  gen_new_var($1.lex, current_declaring_type);
                                } else if (!inside_cabecera_proc){
                                  ts_check($1.lex);
                                  $$.tipo = ts_get_var_type($1.lex);
                                }
                              };
lista_parametros            : parametro COMA lista_parametros {ts_insert_param($1.lex, $1.tipo);}
                            | parametro {ts_insert_param($1.lex, $1.tipo);}
                            | error
                            |
                            ;

parametro                   : MUT tipo identificador {$$.lex = $3.lex; $$.tipo = $2.tipo;}
                            | tipo identificador {$$.lex = $2.lex; $$.tipo = $1.tipo;}
                            ;
tipo                        : TIPO_BASICO { $$.tipo = $1.tipo; }
                            | LIST TIPO_BASICO { $$.tipo = get_tipo_lista($2.tipo); }
                            ;
constante                   : ENTERO { $$.tipo = Int; $$.lugar = $$.lex; }
                            | REAL { $$.tipo = Float; }
                            | CARACTER { $$.tipo = Char; }
                            | BOOL { $$.tipo = Bool; }
                            | lista { $$.tipo = $1.tipo; }
                            ;

lista                       : COR_IZQ { inside_list_declaration = true; }
                              lista_expresiones COR_DER
                              { inside_list_declaration = false; $$.tipo = get_tipo_lista($3.tipo); };



%%

#include "lex.yy.c"

void yyerror(const char *msg) {
	fprintf(stderr, "[Linea %d]: %s\n", yylineno, msg);
}