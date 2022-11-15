%{
// Declaraciones

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Buffer usado para almacenamiento temporal de cadenas de texto
#define MAXLONBUFFER 200
char buffer[MAXLONBUFFER];

%}

%option yylineno
%option noyywrap

CADENA          \"[^\"]*\"
CARACTER        \'[^\']\'
LETRA           [a-zA-Z]
ALFANUMERICO_   [a-zA-Z0-9_]
DIGITO          [0-9]
IDENTIFICADOR   {LETRA}{ALFANUMERICO_}*
ENTERO          {DIGITO}+
REAL            {ENTERO}+.{ENTERO}+

%%
 /* Sección de reglas */

"main"  {
    return INICIO;
}

"var"   {
    return INI_VAR;
}

"endvar"    {
    return FIN_VAR;
}

"proc"  {
    return PROC;
}

"(" {
    return PAR_IZQ;
}

")" {
    return PAR_DER;
}

"[" {
    return COR_IZQ;
}

"]" {
    return COR_DER;
}

";" {
    return PYC;
}

"{" {
    return INI_BLOQUE;
}

"}" {
    return FIN_BLOQUE;
}

"+" {
    // fprintf(yyout, " attr: 0");
    return OP_BINARIO;
}

"/" {
    // fprintf(yyout, " attr: 1");
    return OP_BINARIO;
}

"*" {
    // fprintf(yyout, " attr: 2");
    return OP_BINARIO;
}

">" {
    // fprintf(yyout, " attr: 3");
    return OP_BINARIO;
}

"<" {
    // fprintf(yyout, " attr: 4");
    return OP_BINARIO;
}

">=" {
    // fprintf(yyout, " attr: 5");
    return OP_BINARIO;
}

"<=" {
    // fprintf(yyout, " attr: 6");
    return OP_BINARIO;
}

"!=" {
    // fprintf(yyout, " attr: 7");
    return OP_BINARIO;
}

"==" {
    // fprintf(yyout, " attr: 8");
    return OP_BINARIO;
}

"and" {
    // fprintf(yyout, " attr: 9");
    return OP_BINARIO;
}

"or" {
    // fprintf(yyout, " attr: 10");
    return OP_BINARIO;
}


"!" {
    // fprintf(yyout, " attr: 0");
    return OP_UNARIO;
}

"#" {
    // fprintf(yyout, " attr: 1");
    return OP_UNARIO;
}

"-" {
    return MENOS;
}

"false" {
    // fprintf(yyout, " attr: 0");
    return BOOL;
}

"true" {
    // fprintf(yyout, " attr: 1");
    return BOOL;
}

"if" {
    return IF;
}

"while" {
    return WHILE;
}

"do" {
    return DO;
}

"until" {
    return UNTIL;
}

"return" {
    return RETURN;
}

"read" {
    return READ;
}

"print" {
    return PRINT;
}

"mut" {
    return MUT;
}

{ENTERO} {
    return ENTERO;
}

{REAL} {
    return REAL;
}

"int" {
    // fprintf(yyout, " attr: 0");
    return TIPO_BASICO;
}

"bool" {
    // fprintf(yyout, " attr: 1");
    return TIPO_BASICO;
}

"float" {
    // fprintf(yyout, " attr: 2");
    return TIPO_BASICO;
}

"char" {
    // TODO poner atributos con yylval
    // fprintf(yyout, " attr: 3");
    return TIPO_BASICO;
}

"list" {
    return LIST;
}

"insert" {
    return INSERT;
}

"remove" {
    return REMOVE;
}

"," {
    return COMA;
}

"=" {
    return IGUAL;
}

"else" {
	return ELSE;
}

{CARACTER} {
	return CARACTER;
}


{IDENTIFICADOR} {
    // fprintf(yyout, " attr: %s", yytext);
    return IDENTIFICADOR;
}

{CADENA} {
    return CADENA;
}

[ \t] {
}

[\n\r] {
}

. {
	snprintf(buffer, sizeof(buffer),
	         "Error léxico: no se reconoce la palabra '%s'.", yytext);
	yyerror(buffer);
}

%%
