#include <stdio.h>
#include "cria_func.h"

typedef int (*func_ptr) ();

int id(int x) {
  return x;
}

void dump(void* p, int n){
  unsigned char *p1 = p;
  while(n--){
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
}

int main (void) {
  DescParam params[2];
  func_ptr f_id;
  int i;

  params[0].tipo_val = INT_PAR; /* a nova função passa para mult um valor inteiro */
  params[0].orig_val = FIX;     /* que é o valor corrente da variavel i */
  params[0].valor.v_int = 666;

  //dump(params, sizeof(DescParam));

  f_id = (func_ptr) cria_func (id, params, 1);

  printf("Cheguei aqui\n");


  for (i = 1; i <=10; i++) {
    printf("%d\n", f_id()); /* a nova função não recebe argumentos */
  }

  libera_func(f_id);
  return 0;
}