#include <stdio.h>
#include <string.h>
#include "cria_func.h"

typedef int (*func_ptr) (void* candidata, size_t n);

char fixa[] = "quero saber se a outra string é um prefixo dessa";

int main (void) {
  DescParam params[3];
  func_ptr mesmo_prefixo;
  char s[] = "quero saber tudo";
  int tam;

  params[0].tipo_val = PTR_PAR; /* o primeiro parâmetro de memcmp é um ponteiro para char */
  params[0].orig_val = FIX;     /* a nova função passa para memcmp o endereço da string "fixa" */
  params[0].valor.v_ptr = fixa;

  params[1].tipo_val = PTR_PAR; /* o segundo parâmetro de memcmp é também um ponteiro para char */
  params[1].orig_val = PARAM;   /* a nova função recebe esse ponteiro e repassa para memcmp */

  params[2].tipo_val = INT_PAR; /* o terceiro parâmetro de memcmp é um inteiro */
  params[2].orig_val = PARAM;   /* a nova função recebe esse inteiro e repassa para memcmp */

  mesmo_prefixo = (func_ptr) cria_func (memcmp, params, 3);

  tam = 12;
  printf ("'%s' tem mesmo prefixo-%d de '%s'? %s\n", s, tam, fixa, mesmo_prefixo (s, tam)?"NAO":"SIM");
  tam = 15;
  printf ("'%s' tem mesmo prefixo-%d de '%s'? %s\n", s, tam, fixa, mesmo_prefixo (s, tam)?"NAO":"SIM");

  libera_func(mesmo_prefixo);
  return 0;
}