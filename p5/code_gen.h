#ifndef _CODE_GEN_H
#define _CODE_GEN_H

#include "semantico.h"

void gen_start(void);
void gen_end(void);
void gen_add_code_no_indent(const char* format, ...);
void gen_add_code(const char* format, ...);
const char* gen_new_temp(TipoVar tipo_var);
const char* gen_new_etiqueta(void);
void gen_start_sentencia(void);
void gen_end_sentencia(void);
void gen_new_var(const char* name, TipoVar tipo);

const char* tipo_var_to_str(TipoVar tipo_var);
const char* tipo_var_to_format(TipoVar tipo_var);
const char* tipo_bool_to_str(TipoBool tipo_bool);
char* string_add(const char* s, const char* add);
char* string_add_fmt(const char* s, const char* format, ...);
// void string_add_in_place(char** s, const char* add);


const char* op_binario_to_symbol(TipoOpBinario tipo_op);

#endif
