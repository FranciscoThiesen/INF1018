#include <stdio.h>
#include "cria_func.h"
#include <stdlib.h>


static void dump(void* p, int n){
  unsigned char *p1 = p;
  while(n--){
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
}

void* cria_func (void* f, DescParam params[], int n)
{
	unsigned char* ins = (unsigned char*) malloc(1024);

	unsigned char registers[] = {0xbf, 0xbe, 0xba}; // edi, esi, edx
	unsigned char aux[] = {0xb9, 0xba, 0xbb}; // r9, r10, r11 -> Nao sao callee-saved
	unsigned char aux2[] = {0x39, 0x32, 0x13}; // mov (%r9), %edi .. mov (%r10), %esi ... mov (%r11), %edx

	unsigned long ptr = (unsigned long) f;
	unsigned long var_ptr, ptr_par;
	int livre[] = {0, 0, 0};
	int i = 0;


	////////////////////////// Prologo ///////////////////////////////////
	ins[i++] = 0x55;
	ins[i++] = 0x48;
	ins[i++] = 0x89;
	ins[i++] = 0xe5;
    //////////////////////////////////////////////////////////////////////


	// - Tratar a ordem dos parametros
	// - Esta tudo no quadro
	// - Bom senso
	if(n == 2)
	{
		if(params[0].orig_val != PARAM && params[1].orig_val == PARAM)
		{
			if(params[1].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfe;
		}
	}

	if(n == 3)
	{
		for(int i = 0; i < 3; ++i) livre[i] = (params[i].orig_val == PARAM)? 1 : 0;
		if(livre[0] && !livre[1] && livre[2])
		{
			if(params[2].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xf2;
		}
		else if(!livre[0] && livre[1] && livre[2])
		{
			if(params[2].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xf2;
			if(params[1].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfe;
		}
		else if(!livre[0] && livre[1] && !livre[2])
		{
			if(params[1].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfe;
		}
		else if(!livre[0] && !livre[1] && livre[2])
		{
			if(params[2].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfa;
		}

	}

	for(int idx = 0; idx < n; ++idx)
	{
		if(params[idx].tipo_val == INT_PAR)
		{
			if(params[idx].orig_val == PARAM) continue;
			else if(params[idx].orig_val == FIX)
			{
				ins[i++] = registers[idx];
				ins[i++] = (params[idx].valor.v_int) & 0xff;
				ins[i++] = (params[idx].valor.v_int >> 8) & 0xff;
				ins[i++] = (params[idx].valor.v_int >> 16) & 0xff;
				ins[i++] = (params[idx].valor.v_int >> 24) & 0xff;
			}
			else
			{
				ins[i++] = 0x49;
				ins[i++] = aux[idx];
				var_ptr = (unsigned long) params[idx].valor.v_ptr;
				
				ins[i++] = (var_ptr) & 0xff;
				ins[i++] = (var_ptr >> 8) & 0xff;
				ins[i++] = (var_ptr >> 16) & 0xff;
				ins[i++] = (var_ptr >> 24) & 0xff;
				ins[i++] = (var_ptr >> 32) & 0xff;
				ins[i++] = (var_ptr >> 40) & 0xff;	
				ins[i++] = (var_ptr >> 48) & 0xff;
				ins[i++] = (var_ptr >> 56) & 0xff;

				ins[i++] = 0x41;
				ins[i++] = 0x8b;
				ins[i++] = aux2[idx];
			}
		}
		else if(params[idx].tipo_val == PTR_PAR)
		{ // Parametro eh um ponteiro
			if(params[idx].orig_val == PARAM) continue;
			else if(params[idx].orig_val == FIX)
			{
				ins[i++] = 0x48;
				ins[i++] = registers[idx];

				ptr_par = (unsigned long) params[idx].valor.v_ptr;
				
				ins[i++] = ptr_par & 0xff;
				ins[i++] = (ptr_par >> 8) & 0xff;
				ins[i++] = (ptr_par >> 16) & 0xff;
				ins[i++] = (ptr_par >> 24) & 0xff;
				ins[i++] = (ptr_par >> 32) & 0xff;
				ins[i++] = (ptr_par >> 40) & 0xff;	
				ins[i++] = (ptr_par >> 48) & 0xff;
				ins[i++] = (ptr_par >> 56) & 0xff;
			}
			else
			{
				ins[i++] = 0x49;
				ins[i++] = aux[idx];

				ptr_par = (unsigned long) params[idx].valor.v_ptr;
				
				ins[i++] = ptr_par & 0xff;
				ins[i++] = (ptr_par >> 8) & 0xff;
				ins[i++] = (ptr_par >> 16) & 0xff;
				ins[i++] = (ptr_par >> 24) & 0xff;
				ins[i++] = (ptr_par >> 32) & 0xff;
				ins[i++] = (ptr_par >> 40) & 0xff;	
				ins[i++] = (ptr_par >> 48) & 0xff;
				ins[i++] = (ptr_par >> 56) & 0xff;

				ins[i++] = 0x49;
				ins[i++] = 0x8b;
				ins[i++] = aux2[idx];
			}
		}
	}
	

	////////////////////////// Chamada de funcao //////////////////////////
	ins[i++] = 0x48;
	ins[i++] = 0xb8;

	ins[i++] = (ptr) & 0xff;
	ins[i++] = (ptr >> 8) & 0xff;
	ins[i++] = (ptr >> 16) & 0xff;
	ins[i++] = (ptr >> 24) & 0xff;
	ins[i++] = (ptr >> 32) & 0xff;
	ins[i++] = (ptr >> 40) & 0xff;	
	ins[i++] = (ptr >> 48) & 0xff;
	ins[i++] = (ptr >> 56) & 0xff;

	ins[i++] = 0xff;
	ins[i++] = 0xd0;
	//////////////////////////////////////////////////////////////////////
	


	////////////////////////// Epilogo ///////////////////////////////////
	ins[i++] = 0xc9;
	ins[i++] = 0xc3;
	//////////////////////////////////////////////////////////////////////
	
	return ins;
}

void libera_func (void* func)
{
	free(func);
}