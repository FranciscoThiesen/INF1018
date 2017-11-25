#include <stdio.h>
#include "cria_func.h"

typedef int (*func_ptr) ();

int id(int* x){
  return *x;
}

int main (void) {
  DescParam params[2];
  func_ptr f_id;
  int i;
  int fixo[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110};
  int* ptr = &fixo[0];
  int** ptr2 = &ptr;

  params[0].tipo_val = PTR_PAR; /* o primeiro parãmetro de mult é int */
  params[0].orig_val = IND;   /* a nova função repassa seu parämetro */
  params[0].valor.v_ptr = ptr2;

  f_id = (func_ptr) cria_func (id, params, 1);

  for (i = 1; i <=10; i++) {
    ptr++;
    printf("%d\n", f_id()); /* a nova função só recebe um argumento */
  }
  libera_func(f_id);

  return 0;
}