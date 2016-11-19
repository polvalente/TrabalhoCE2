#ifndef SIMLIB
#define SIMLIB

#define MAX_LINHA 80
#define MAX_NOME 11
#define MAX_ELEM 50
#define MAX_NOS 50
#define TOLG 1e-9
#define DEBUG

#define OK														0
#define ERRO_RESOLUCAO_SISTEMA				1
#define ERRO_ELEMENTO_DESCONHECIDO		2
#define ERRO_NUM_ARGUMENTOS						3
#define ERRO_NUM_VARIAVEIS						4
#define ERRO_NUM_ELEMENTOS						5

#include <string>

class Elemento{
	public:
		std::string nome;
		double valor;
		int a,b,c,d,x,y;
		Elemento();
};

Elemento::Elemento(void){
}

#endif
