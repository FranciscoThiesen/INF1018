#ifndef CRIA_FUNC_H
#define CRIA_FUNC_H

typedef enum {INT_PAR, PTR_PAR} TipoValor;
typedef enum {PARAM, FIX, IND} OrigemValor;

typedef struct {
   TipoValor    tipo_val; /* indica o tipo do parametro */
   OrigemValor  orig_val;  /* indica a origem do valor do parametro */
   union {
     int v_int;
     void* v_ptr;
   } valor;         /* define o valor ou endereÃ§o do parametro (amarrado) */
} DescParam;

void* cria_func (void* f, DescParam params[], int n);
void libera_func (void* func);

#endif
