#ifndef _FUNCTIONS_CE2_
#define _FUNCTIONS_CE2_

#include <string>
#include <vector>

#include "classes.hpp"

//Cabecalhos de funcoes
int resolverSistema(std::vector< std::vector<long double>>&, int&);
int numero(std::vector<std::string>&, std::string, int&);
//void leituraNetlist(std::vector<std::string>&, std::vector<Elemento>&, std::vector<Elemento>&, int, char**, int&, int&, double&, double&, std::string, double&);
void leituraNetlist(std::vector<std::string>&, 
		std::vector<Elemento>&, 
		std::vector<Elemento>&, 
		int, 
		char**, 
		int&, 
		int&,
		double&,
		double&,
		std::string&,
                double&,
                std::vector<Elemento>&);
	
void adicionarVariaveis(std::vector<std::string>&, std::vector<Elemento>&, int&, int&, int&);
void listarVariaveis(std::vector<std::string>, int);
void mostrarNetlist(std::vector<Elemento>, int);
void montarSistemaDC(std::vector<Elemento>&, std::vector< std::vector<long double>>&, int, int);
void mostrarSistema(std::string, std::vector<std::vector<long double>>, int);
int simulacaoTrapezios(std::vector<Elemento>, std::vector<Elemento>, std::vector<std::string>&, int, int, int&, double, double, double, std::vector<Elemento>&);
void adicionarVariaveisDinamicas(std::vector<std::string>&, std::vector<Elemento>&, int&, int&);
std::vector<long double> resolverPontoOperacao(std::vector<std::vector<long double>>, std::vector<Elemento>, int); 

void adicionarEstampasComponentesVariantes(std::vector<std::vector<long double>>&, std::vector<Elemento>, std::vector<long double>); 

void condensarVariaveis(std::vector<std::vector<long double>>&, std::vector<Elemento>);
void condensarLinhas(std::vector<std::vector<long double>>&, std::vector<std::vector<int>>);
void condensarColunas(std::vector<std::vector<long double>>&, std::vector<std::vector<int>>);

#endif
