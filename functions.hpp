#ifndef _FUNCTIONS_CE2_
#define _FUNCTIONS_CE2_

#define versao "1.0 - 18/11/2016"

#define MAX_LINHA 80
#define MAX_NOME 11
#define MAX_ELEM 50
#define MAX_NOS 50
#define TOLG 1e-9
#define DEBUG

#define OK			    0
#define ERRO_RESOLUCAO_SISTEMA	    1
#define ERRO_ELEMENTO_DESCONHECIDO  2
#define ERRO_NUM_ARGUMENTOS	    3
#define ERRO_NUM_VARIAVEIS	    4 
#define ERRO_NUM_ELEMENTOS	    5

#include <string>
#include <vector>

#include "classes.hpp"

//Cabecalhos de funcoes
int resolverSistema(std::vector< std::vector<long double>>&, int&);
int numero(std::vector<std::string>&, std::string, int&);
void leituraNetlist(std::vector<std::string>&, std::vector<Elemento>&, int, char**, int&, int&);
void adicionarVariaveis(std::vector<std::string>&, std::vector<Elemento>&, int&, int&, int&);
void listarVariaveis(std::vector<std::string>, int);
void mostrarNetlist(std::vector<Elemento>, int);
void montarSistema(std::vector<Elemento>&, std::vector< std::vector<long double>>&, int, int);
void mostrarSistema(std::string, std::vector<std::vector<long double>>, int);

#endif
