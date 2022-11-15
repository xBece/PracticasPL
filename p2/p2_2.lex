%{

// Declaraciones

#include <stdlib.h>
#include <string.h>
#include "tokens.h"


%}

%option yylineno
%option noyywrap

CADENA          \"[^\"]*\"
LETRA           [a-zA-Z]
ALFANUMERICO_   [a-zA-Z0-9_]
DIGITO          [0-9]
IDENTIFICADOR   {LETRA}{ALFANUMERICO_}*
ENTERO          {DIGITO}+
REAL            {ENTERO}+.{ENTERO}+

%%
 /* Sección de reglas */

"main"  {
    ECHO;
    return INICIO;
}

"var"   {
    ECHO;
    return INI_VAR;
}

"endvar"    {
    ECHO;
    return FIN_VAR;
}

"proc"  {
    ECHO;
    return PROC;
}

"(" {
    ECHO;
    return PAR_IZQ;
}

")" {
    ECHO;
    return PAR_DER;
}

"[" {
    ECHO;
    return COR_IZQ;
}

"]" {
    ECHO;
    return COR_DER;
}

";" {
    ECHO;
    return PyC;
}

"{" {
    ECHO;
    return INI_BLOQUE;
}

"}" {
    ECHO;
    return FIN_BLOQUE;
}

"+" {
    ECHO;
    fprintf(yyout, " attr: 0");
    return OP_BINARIO;
}

"/" {
    ECHO;
    fprintf(yyout, " attr: 1");
    return OP_BINARIO;
}

"*" {
    ECHO;
    fprintf(yyout, " attr: 2");
    return OP_BINARIO;
}

">" {
    ECHO;
    fprintf(yyout, " attr: 3");
    return OP_BINARIO;
}

"<" {
    ECHO;
    fprintf(yyout, " attr: 4");
    return OP_BINARIO;
}

">=" {
    ECHO;
    fprintf(yyout, " attr: 5");
    return OP_BINARIO;
}

"<=" {
    ECHO;
    fprintf(yyout, " attr: 6");
    return OP_BINARIO;
}

"!=" {
    ECHO;
    fprintf(yyout, " attr: 7");
    return OP_BINARIO;
}

"==" {
    ECHO;
    fprintf(yyout, " attr: 8");
    return OP_BINARIO;
}

"and" {
    ECHO;
    fprintf(yyout, " attr: 9");
    return OP_BINARIO;
}

"or" {
    ECHO;
    fprintf(yyout, " attr: 10");
    return OP_BINARIO;
}


"!" {
    ECHO;
    fprintf(yyout, " attr: 0");
    return OP_UNARIO;
}

"#" {
    ECHO;
    fprintf(yyout, " attr: 1");
    return OP_UNARIO;
}

"-" {
    ECHO;
    return MENOS;
}

"false" {
    ECHO;
    fprintf(yyout, " attr: 0");
    return BOOL;
}

"true" {
    ECHO;
    fprintf(yyout, " attr: 1");
    return BOOL;
}

"if" {
    ECHO;
    return IF;
}

"while" {
    ECHO;
    return WHILE;
}

"do" {
    ECHO;
    return DO;
}

"until" {
    ECHO;
    return UNTIL;
}

"return" {
    ECHO;
    return RETURN;
}

"read" {
    ECHO;
    return READ;
}

"print" {
    ECHO;
    return PRINT;
}

"mut" {
    ECHO;
    return MUT;
}

{ENTERO} {
    ECHO;
    return ENTERO;
}

{REAL} {
    ECHO;
    return REAL;
}

"int" {
    ECHO;
    fprintf(yyout, " attr: 0");
    return TIPO_BASICO;
}

"bool" {
    ECHO;
    fprintf(yyout, " attr: 1");
    return TIPO_BASICO;
}

"float" {
    ECHO;
    fprintf(yyout, " attr: 2");
    return TIPO_BASICO;
}

"char" {
    ECHO;
    fprintf(yyout, " attr: 3");
    return TIPO_BASICO;
}

"list" {
    ECHO;
    return LIST;
}

"insert" {
    ECHO;
    return INSERT;
}

"remove" {
    ECHO;
    return REMOVE;
}

"," {
    ECHO;
    return COMA;
}

"=" {
    ECHO;
    return IGUAL;
}

{IDENTIFICADOR} {
    ECHO;
    fprintf(yyout, " attr: %s", yytext);
    return IDENTIFICADOR;
}

{CADENA} {
    ECHO;
    return CADENA;
}

[ \t] {
    // ECHO;
}

[\n\r] {
    // ECHO;
}

. {
    printf(" \nError Léxico en Linea: %d. No se reconoce la palabra '%s'. ", yylineno, yytext);
}

%%

/* Sección de procedimientos */

int main (int argc, char** argv) {

    // Se comprueba que se recibe 1 argumento (nombre del fichero fuente)
    if (argc <= 1) {

        printf("\nError al ejecutar la aplicación...\n");
        printf("Uso: %s nombre_fichero_fuente\n", argv[0]);

        exit(-1);

    }

    // Se abre el fichero recibido por parámetro
    yyin = fopen(argv[1], "r");

    // Si "yyin" es nulo no se ha podido abrir el fichero
    if (yyin == NULL) {

        printf ("\nError al abrir el fichero %s\n", argv[1]);

        exit (-2);

    }

    // Invocamos al analizador léxico, que seguirá hasta que termine el fichero
    int val;
    val= yylex() ;
    while (val != 0) {
        printf (" %d\n", val);
        val= yylex() ;
    }

    exit(0);
}