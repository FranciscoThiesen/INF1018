#include <stdio.h>
#include "cria_func.h"
#include <stdlib.h>

void* cria_func (void* f, DescParam params[], int n)
{
	unsigned char* ins = (unsigned char*) malloc(1024);

	unsigned char registers[] = {0xbf, 0xbe, 0xba}; // edi, esi, edx
	unsigned char aux[] = {0xb9, 0xba, 0xbb}; // r9, r10, r11 -> Nao sao callee-saved
	unsigned char aux2[] = {0x39, 0x32, 0x13}; // mov (%r9), %edi .. mov (%r10), %esi ... mov (%r11), %edx

	unsigned long ptr = (unsigned long) f, var_ptr, ptr_par;
	int i = 0, livre[] = {0, 0, 0};

	/////////////////////////////////////// Prologo ///////////////////////////////////////////////
	ins[i++] = 0x55;
	ins[i++] = 0x48;
	ins[i++] = 0x89;
	ins[i++] = 0xe5;
	///////////////////////////////////////////////////////////////////////////////////////////////
	
	// Tratando casos aonde tenho variaveis livres, depois de variaveis amarradas..
	// Nesse caso geralmente tenho que fazer algo do tipo mov %rdi, %rsi
	if(n == 2)
	{
		// mov %rdi, %rsi ou mov %edi, %esi
		if(params[0].orig_val != PARAM && params[1].orig_val == PARAM)
		{
			if(params[1].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfe;
		}
	}

	// Com 3 parametros, faco basicamente a mesma coisa..
	// O caso que achei mais "delicado", eh o que ocorre quando a primeira variavel esta fixa, e as duas seguintes
	// estao livres.. Ai preciso fazer mov %rsi, %rdx e depois %rdi, %rsi... A ordem das operacoes importa nesse caso!
	// OBS: o byte 0x48 eh necessario quando vou fazer mov de 8 bytes...
	if(n == 3)
	{
		for(int i = 0; i < 3; ++i) livre[i] = (params[i].orig_val == PARAM)? 1 : 0;
		if(livre[0] && !livre[1] && livre[2])
		{
			// mov %esi, %edx .. ou o equivalente para 8 bytes mov %rdi, %rdx
			if(params[2].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xf2;
		}
		else if(!livre[0] && livre[1] && livre[2])
		{
			// esse eh um dos casos que achei mais "complexo" de tratar
			// mov %esi, %edx, ou o equivalente para 8 bytes
			if(params[2].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xf2;

			// mov %edi, %esi, ou o equivalente para 8 bytes
			if(params[1].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfe;
		}
		else if(!livre[0] && livre[1] && !livre[2])
		{
			// mov %edi, %esi, ou o equivalente para 8 bytes...
			if(params[1].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfe;
		}
		else if(!livre[0] && !livre[1] && livre[2])
		{
			// mov %edi, %edx ou o equivalente para 8 bytes
			if(params[2].tipo_val == PTR_PAR) ins[i++] = 0x48;
			ins[i++] = 0x89;
			ins[i++] = 0xfa;
		}

	}

	// Agora eh realizado o trabalho de verdade (:)
	for(int idx = 0; idx < n; ++idx)
	{
		if(params[idx].tipo_val == INT_PAR)
		{
			// Se a variavel for parametro, nao faco nada
			if(params[idx].orig_val == PARAM) continue;
			else if(params[idx].orig_val == FIX)
			{
				// Se a variavel for fixa e do tipo inteiro
				// eu coloco o valor recebido no registrador correspondente
				ins[i++] = registers[idx];
				ins[i++] = (params[idx].valor.v_int) & 0xff;
				ins[i++] = (params[idx].valor.v_int >> 8) & 0xff;
				ins[i++] = (params[idx].valor.v_int >> 16) & 0xff;
				ins[i++] = (params[idx].valor.v_int >> 24) & 0xff;
			}
			else
			{
				// aqui eu coloco o valor do ponteiro para o parametro em um registrador
				// auxiliar (%r9, %r10, %r11) que nao sao callee-saved
				// movq $constante, (%r9 ou %r10 ou %r11)
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

				// aqui faco o algo do tipo, movq (%r9), %edi... ou movo para o parametro da vez
				ins[i++] = 0x41;
				ins[i++] = 0x8b;
				ins[i++] = aux2[idx];
			}
		}
		else if(params[idx].tipo_val == PTR_PAR)
		{
			if(params[idx].orig_val == PARAM) continue; // posso continuar sem problemas
			else if(params[idx].orig_val == FIX)
			{
				// Aqui faco, movq $constante, (%rdi, %rsi ou %rdx)
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
				// esse eh o caso de um ponteiro para um ponteiro que amarra uma variavel
				// faco movq $constante, (%r9 ou %r10 ou %r11)
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

				// e depois faco movq (%r9), %rdi (ou para o registrador da vez) 
				ins[i++] = 0x49;
				ins[i++] = 0x8b;
				ins[i++] = aux2[idx];
			}
		}
	}

	////////////////////////// Chamada de funcao //////////////////////////
	// movq $constante, %rax
	// call *%rax
	
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
