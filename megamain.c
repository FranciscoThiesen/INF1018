#include <stdio.h>
#include <string.h>
#include "cria_func.h"

typedef int (*func_ptr) (int* a);

int add(int a, int b, int* c){
  return a + b + *c;
}

int main (void) {
  DescParam p[3];
  func_ptr f_add;
  int tam;

  p[0].tipo_val = INT_PAR; /* o segundo parâmetro de memcmp é também um ponteiro para char */
  p[0].orig_val = FIX;   /* a nova função recebe esse ponteiro e repassa para memcmp */
  p[0].valor.v_int = 35;

  p[1].tipo_val = INT_PAR; /* o primeiro parâmetro de memcmp é um ponteiro para char */
  p[1].orig_val = FIX;     /* a nova função passa para memcmp o endereço da string "fixa" */
  p[1].valor.v_int = 10;

  p[2].tipo_val = PTR_PAR; /* o terceiro parâmetro de memcmp é um inteiro */
  p[2].orig_val = PARAM;   /* a nova função recebe esse inteiro e repassa para memcmp */

  f_add = (func_ptr) cria_func (add, p, 3);
  int val = 5;
  printf("add (%d, %d) = %d\n", 5, 35, f_add(&val));

  libera_func(f_add);
  return 0;
}