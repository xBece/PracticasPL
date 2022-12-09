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
REAL            {ENTERO}+\.{ENTERO}+

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
    yylval.tipo = Suma;
    return OP_BINARIO_SUM;
}

"/" {
    yylval.tipo = Div;
    return OP_BINARIO_PROD_DIV;
}

"*" {
    yylval.tipo = Mult;
    return OP_BINARIO_PROD_DIV;
}

">" {
    yylval.tipo = Greater;
    return OP_BINARIO_REL;
}

"<" {
    yylval.tipo = Less;
    return OP_BINARIO_REL;
}

">=" {
    yylval.tipo = GreaterEq;
    return OP_BINARIO_REL;
}

"<=" {
    yylval.tipo = LessEq;
    return OP_BINARIO_REL;
}

"!=" {
    yylval.tipo = NotEq;
    return OP_BINARIO_REL;
}

"==" {
    yylval.tipo = Eq;
    return OP_BINARIO_REL;
}

"and" {
    yylval.tipo = And;
    return OP_BINARIO_BOOL;
}

"or" {
    yylval.tipo = Or;
    return OP_BINARIO_BOOL;
}


"!" {
    yylval.tipo = Not;
    return OP_UNARIO;
}

"#" {
    yylval.tipo = LengthOf;
    return OP_UNARIO;
}

"-" {
    return MENOS;
}

"false" {
    yylval.tipo = False;
    return BOOL;
}

"true" {
    yylval.tipo = True;
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
    yylval.lex = strdup(yytext);
    return ENTERO;
}

{REAL} {
    yylval.lex = strdup(yytext);
    return REAL;
}

"int" {
    yylval.tipo = Int;
    return TIPO_BASICO;
}

"bool" {
    yylval.tipo = Bool;
    return TIPO_BASICO;
}

"float" {
    yylval.tipo = Float;
    return TIPO_BASICO;
}

"char" {
    yylval.tipo = Char;
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
    yylval.lex = strdup(yytext);
	return CARACTER;
}


{IDENTIFICADOR} {
    yylval.lex = strdup(yytext);
    return IDENTIFICADOR;
}

{CADENA} {
    yylval.lex = strdup(yytext);
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
