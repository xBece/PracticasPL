#include <stdio.h>
#include <stdlib.h>
#include "code_gen.h"

extern FILE* yyin;
int yyparse();

FILE *abrir_entrada(int argc, char *argv[]) {
    FILE *f = NULL;
    if (argc > 1) {
        f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "fichero '%s' no encontrado\n", argv[1]);
            exit(1);
        }
        else
            printf("leyendo fichero '%s'.\n", argv[1]);
    }
    else
        printf("leyendo entrada estándar.\n");
    return f;
}

int main(int argc, char *argv[]) {
    yyin = abrir_entrada(argc,argv) ;
    return yyparse() ;
}