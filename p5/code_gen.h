#ifndef _CODE_GEN_H
#define _CODE_GEN_H

#include "semantico.h"

void gen_start(void);
void gen_end(void);
void gen_add_code(const char* format, ...);
const char* gen_new_temp(TipoVar tipo_var);
const char* gen_new_etiqueta(void);
void gen_start_sentencia(void);
void gen_end_sentencia(void);
void gen_new_var(const char* name, TipoVar tipo);

#endif
