#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

//#include <cmath>

#include "classes.hpp"
#include "functions.hpp"
#include "constants.hpp"

//int
//  num_elementos, /* Elementos */
//  num_variaveis, /* Variaveis */
//  num_nos; /* Nos */

/* Resolucao de sistema de equacoes lineares.
   Metodo de Gauss-Jordan com condensacao pivotal */
int resolverSistema(std::vector< std::vector<long double>>& Yn, int& num_variaveis){
  int i,j,l, a;
  long double t, p;

  for (i=1; i<=num_variaveis; i++) {
    t=0.0;
    a=i;
    for (l=i; l<=num_variaveis; l++) {
      if (fabs(Yn[l][i])>fabs(t)) {
				a=l;
				t=Yn[l][i];
      }
    }
    if (i!=a) {
      for (l=1; l<=num_variaveis+1; l++) {
				p=Yn[i][l];
				Yn[i][l]=Yn[a][l];
				Yn[a][l]=p;
      }
    }
    if (fabs(t)<TOLG) {
			std::cout << "Sistema singular" << std::endl;
      return ERRO_RESOLUCAO_SISTEMA;
    }
    for (j=num_variaveis+1; j>0; j--) {  /* Basta j>i em vez de j>0 */
      Yn[i][j]/= t;
      p=Yn[i][j];
      if (p!=0)  /* Evita operacoes com zero */
        for (l=1; l<=num_variaveis; l++) {  
	  if (l!=i)
	    Yn[l][j]-=Yn[l][i]*p;
        }
    }
  }
  return OK;
}

/* Rotina que conta os nos e atribui numeros a eles */
int numero(std::vector<std::string>& lista, std::string nome, int& num_variaveis)
{
	auto i = std::find(lista.begin(), lista.end(), nome);
	if (i != lista.end()){
		//Encontrou na lista
		return i - lista.begin();
	}
	
	lista.push_back(nome);
	num_variaveis++;
	return num_variaveis;	
}
void leituraNetlist(
		std::vector<std::string>& lista, 
		std::vector<Elemento>& netlist, 
		std::vector<Elemento>& componentesVariantes, 
		int argc, 
		char **argv, 
		int& num_elementos, 
		int& num_variaveis,
		double& tempo_final,
		double& passo,
		std::string& metodo,
		double& passos_por_ponto,
		std::vector<Elemento>& amp_ops){
	
	using namespace std;

	FILE *arquivo = NULL;
  string nomearquivo;
	char tipo;
	string na,nb,nc,nd;

	if (argc == 2){
		nomearquivo = argv[1];
		arquivo = fopen(nomearquivo.c_str(), "r");
	}

	while(!arquivo){
		cout << "Nome do arquivo com o netlist (ex: mna.net): ";
		cin >> nomearquivo;
		arquivo=fopen(nomearquivo.c_str(),"r");
			if (!arquivo) {
				cout << "Arquivo " << nomearquivo << " inexistente" << endl;
			}
	}
	fclose(arquivo);
  cout << "Lendo netlist" << endl;
	
	ifstream input_file(nomearquivo);
	num_elementos=0;
	num_variaveis=0;
	lista.push_back("0");
  
	string linha;
	getline(input_file, linha);
	cout << "Titulo: " << linha << endl;
	
	while (getline(input_file, linha)) {
		Elemento elemento;
    num_elementos++; /* Nao usa o netlist[0] */
		netlist.push_back(elemento);

    /*if (num_elementos>MAX_ELEM) {
      cout << "O programa so aceita ate " << MAX_ELEM << " elementos" << endl;
      exit(ERRO_NUM_ELEMENTOS);
    }*/

    linha[0]=toupper(linha[0]);
    tipo=linha[0];

		istringstream input(linha);

    if (tipo=='R' || tipo=='I' || tipo=='V') {
			input >> netlist[num_elementos].nome >> na >> nb >> netlist[num_elementos].valor;
      cout << netlist[num_elementos].nome << " " << na << " " <<  nb << " " << netlist[num_elementos].valor << endl;
      netlist[num_elementos].a=numero(lista, na, num_variaveis);
      netlist[num_elementos].b=numero(lista, nb, num_variaveis);
    }
		else if (tipo=='C' || tipo=='L'){
			netlist.pop_back();
			num_elementos--;
			input >> elemento.nome >> na >> nb >> elemento.valor; 	
			elemento.a = numero(lista, na, num_variaveis);
			elemento.b = numero(lista, nb, num_variaveis);
			componentesVariantes.push_back(elemento);
		}
    else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
			input >> netlist[num_elementos].nome >> na >> nb >> nc >> nd >> netlist[num_elementos].valor;
      cout << netlist[num_elementos].nome << " " << na << " " <<  nb << " " << nc << " " << nd << " " << netlist[num_elementos].valor << endl;
      netlist[num_elementos].a=numero(lista, na, num_variaveis);
      netlist[num_elementos].b=numero(lista, nb, num_variaveis);
      netlist[num_elementos].c=numero(lista, nc, num_variaveis);
      netlist[num_elementos].d=numero(lista, nd, num_variaveis);
    }
    else if (tipo=='O') {
			input >> netlist[num_elementos].nome >> na >> nb >> nc >> nd;
      cout << netlist[num_elementos].nome << " " << na << " " <<  nb << " " << nc << " " << nd << endl;
      netlist[num_elementos].a=numero(lista, na, num_variaveis);
      netlist[num_elementos].b=numero(lista, nb, num_variaveis);
      netlist[num_elementos].c=numero(lista, nc, num_variaveis);
      netlist[num_elementos].d=numero(lista, nd, num_variaveis);
			amp_ops.push_back(netlist[num_elementos]);
    }
    else if (tipo=='*') { /* Comentario comeca com "*" */
      cout << "Comentario: " << linha << endl;
			netlist.pop_back();
      num_elementos--;
    }
		else if (tipo == '.'){ /* Linha que contem o comando .TRAN*/
			netlist.pop_back();
			num_elementos--;
			string temp;
			input >> temp >> tempo_final >> passo >> metodo >> passos_por_ponto; 
		}
    else {
      cout << "Elemento desconhecido: " << linha << endl;
      cin.get();
      exit(ERRO_ELEMENTO_DESCONHECIDO);
    }
  }
}

void adicionarVariaveis(std::vector<std::string>& lista, std::vector<Elemento>& netlist, int& num_variaveis, int& num_nos, int& num_elementos){
	int i;
	char tipo;

  num_nos=num_variaveis;
  for (i=1; i<=num_elementos; i++) {
    tipo=netlist[i].nome[0];
    if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O') {
      num_variaveis++;
      /*if (num_variaveis>MAX_NOS) {
        cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NOS << ")" << endl;
        exit(ERRO_NUM_VARIAVEIS);
      }*/
      lista.push_back("j"+netlist[i].nome);
      netlist[i].x=num_variaveis;
    }
    else if (tipo=='H') {
      num_variaveis+=2;
      /*if (num_variaveis>MAX_NOS) {
				std::cout << "As correntes extra excederam o numero de variaveis permitido (" << MAX_NOS << ")" << std::endl;
        exit(ERRO_NUM_VARIAVEIS);
      }*/
      lista.push_back("jx"+netlist[i].nome);
      netlist[i].x=num_variaveis-1;
      lista.push_back("jy"+netlist[i].nome);
      netlist[i].y=num_variaveis;
    }
  }
}

void listarVariaveis(std::vector<std::string> lista, int num_variaveis){
	std::cout << "Variaveis internas:  " << std::endl;
  for (int i=0; i<=num_variaveis; i++)
		std::cout << i << " -> " << lista[i] << std::endl;
}

void mostrarNetlist(std::vector<Elemento> netlist, int num_elementos){
	using namespace std;
	char tipo;
	int i;

	cout << "Netlist interno final" << endl;
		for (i=1; i<=num_elementos; i++) {
			tipo=netlist[i].nome[0];
			if (tipo=='R' || tipo=='I' || tipo=='V') {
				cout << netlist[i].nome << " " << netlist[i].a << " " << netlist[i].b << " " << netlist[i].valor << endl;
			}
			else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
				cout << netlist[i].nome << " " << netlist[i].a << " " << netlist[i].b << " " << netlist[i].c << " " << netlist[i].d << " " << netlist[i].valor << endl;
			}
			else if (tipo=='O') {
				cout << netlist[i].nome << " " << netlist[i].a << " " << netlist[i].b << " " << netlist[i].c << " " << netlist[i].d << endl;
			}
			if (tipo=='V' || tipo=='E' || tipo=='F' || tipo=='O')
				cout << "Corrente jx: " << netlist[i].x << endl;
			else if (tipo=='H')
				cout << "Correntes jx e jy: " << netlist[i].x << " " << netlist[i].y << endl;
		}
}

void montarSistemaDC(std::vector<Elemento>& netlist, std::vector< std::vector<long double>>& Yn, int num_variaveis, int num_elementos){
	int i, j;
	long double g;
	char tipo;
	

	for (i=0; i<=num_variaveis; i++) {
    for (j=0; j<=num_variaveis+1; j++)
      Yn[i][j]=0;
  }

  /* Monta estampas */
  for (i=1; i<=num_elementos; i++) {
    tipo=netlist[i].nome[0];
    if (tipo=='R') {
      g=1/netlist[i].valor;
      Yn[netlist[i].a][netlist[i].a]+=g;
      Yn[netlist[i].b][netlist[i].b]+=g;
      Yn[netlist[i].a][netlist[i].b]-=g;
      Yn[netlist[i].b][netlist[i].a]-=g;
    }
    else if (tipo=='G') {
      g=netlist[i].valor;
      Yn[netlist[i].a][netlist[i].c]+=g;
      Yn[netlist[i].b][netlist[i].d]+=g;
      Yn[netlist[i].a][netlist[i].d]-=g;
      Yn[netlist[i].b][netlist[i].c]-=g;
    }
    else if (tipo=='I') {
      g=netlist[i].valor;
      Yn[netlist[i].a][num_variaveis+1]-=g;
      Yn[netlist[i].b][num_variaveis+1]+=g;
    }
    else if (tipo=='V') {
      Yn[netlist[i].a][netlist[i].x]+=1;
      Yn[netlist[i].b][netlist[i].x]-=1;
      Yn[netlist[i].x][netlist[i].a]-=1;
      Yn[netlist[i].x][netlist[i].b]+=1;
      Yn[netlist[i].x][num_variaveis+1]-=netlist[i].valor;
    }
    else if (tipo=='E') {
      g=netlist[i].valor;
      Yn[netlist[i].a][netlist[i].x]+=1;
      Yn[netlist[i].b][netlist[i].x]-=1;
      Yn[netlist[i].x][netlist[i].a]-=1;
      Yn[netlist[i].x][netlist[i].b]+=1;
      Yn[netlist[i].x][netlist[i].c]+=g;
      Yn[netlist[i].x][netlist[i].d]-=g;
    }
    else if (tipo=='F') {
      g=netlist[i].valor;
      Yn[netlist[i].a][netlist[i].x]+=g;
      Yn[netlist[i].b][netlist[i].x]-=g;
      Yn[netlist[i].c][netlist[i].x]+=1;
      Yn[netlist[i].d][netlist[i].x]-=1;
      Yn[netlist[i].x][netlist[i].c]-=1;
      Yn[netlist[i].x][netlist[i].d]+=1;
    }
    else if (tipo=='H') {
      g=netlist[i].valor;
      Yn[netlist[i].a][netlist[i].y]+=1;
      Yn[netlist[i].b][netlist[i].y]-=1;
      Yn[netlist[i].c][netlist[i].x]+=1;
      Yn[netlist[i].d][netlist[i].x]-=1;
      Yn[netlist[i].y][netlist[i].a]-=1;
      Yn[netlist[i].y][netlist[i].b]+=1;
      Yn[netlist[i].x][netlist[i].c]-=1;
      Yn[netlist[i].x][netlist[i].d]+=1;
      Yn[netlist[i].y][netlist[i].x]+=g;
    }
    else if (tipo=='O') {
      Yn[netlist[i].a][netlist[i].x]+=1;
      Yn[netlist[i].b][netlist[i].x]-=1;
      Yn[netlist[i].x][netlist[i].c]+=1;
      Yn[netlist[i].x][netlist[i].d]-=1;
    }
	
	#ifdef DEBUG
	/* Opcional: Mostra o sistema apos a montagem da estampa */
		std::ostringstream msg;
		msg <<  "Sistema apos a estampa de " << netlist[i].nome;
		mostrarSistema(msg.str(),Yn, num_variaveis);
		std::cin.get();
	#endif
  }
}

void mostrarSistema(std::string msg, std::vector<std::vector<long double>> Yn, int num_variaveis){
  /* Opcional: Mostra o sistema resolvido */
	std::cout << msg << std::endl;

  for (int i=1; i<=num_variaveis; i++) {
      for (int j=1; j<=num_variaveis+1; j++)
        if ( (Yn[i][j]!=0) || (j == num_variaveis+1)) 
					printf("%+3.1Lf ",Yn[i][j]);
        else printf(" ... ");
			std::cout << std::endl;
    }
}

void adicionarVariaveisDinamicas(std::vector<std::string>& lista, std::vector<Elemento>& componentesVariantes, int& num_variaveis, int& num_nos){
	unsigned i;
	char tipo;

  num_nos=num_variaveis;
  for (i=0; i<componentesVariantes.size(); i++) {
    tipo=componentesVariantes[i].nome[0];
    if (tipo=='C' || tipo=='L') {
      num_variaveis++;
      lista.push_back("j"+componentesVariantes[i].nome);
			componentesVariantes[i].x = num_variaveis;
    }
  }
}

void adicionarEstampasComponentesVariantes(std::vector<std::vector<long double>>& sistema, std::vector<Elemento> componentesVariantes, std::vector<long double> solucoes){
}

int simulacaoTrapezios(
		std::vector<Elemento> netlist, 
		std::vector<Elemento> componentesVariantes, 
		std::vector<std::string>& lista, 
		int num_elementos, 
		int num_nos, 
		int& num_variaveis, 
		double passo, 
		double tempo_final, 
		double passos_por_ponto){
	// montar sistema dc
	// a cada iteracao:
	//   adiciona estampas dos componentes variantes no tempo
	//   resolve o sistema nodal
	//   guarda a solucao da iteracao na linha de uma matriz
	using namespace std;
	adicionarVariaveisDinamicas(lista, componentesVariantes, num_variaveis, num_nos); 
	
	vector<vector<long double>> sistemaEsqueleto(num_variaveis+1, vector<long double>(num_variaveis+2));
	montarSistemaDC(netlist, sistemaEsqueleto, num_variaveis, num_elementos);
	vector<vector<long double>> sistemaCompleto(num_variaveis+1, vector<long double>(num_variaveis+2));

	vector<vector<long double>> solucoes;
	solucoes.push_back(resolverPontoOperacao(sistemaEsqueleto, componentesVariantes, num_variaveis));

	for (int t0 = 0; t0+passo <= tempo_final; t0+=passo){
		sistemaCompleto = sistemaEsqueleto;			
		adicionarEstampasComponentesVariantes(sistemaCompleto, componentesVariantes, solucoes[solucoes.size()-1]);
		resolverSistema(sistemaCompleto, num_variaveis);
		vector<long double> solucao(num_variaveis+2);
		for (int i=0; i<num_variaveis+1; i++)
			solucao[i] = sistemaCompleto[i][num_variaveis+1];
		solucoes.push_back(solucao);
	}

	return OK;
}

std::vector<long double> resolverPontoOperacao(std::vector<std::vector<long double>> sistema, std::vector<Elemento> componentesVariantes, int num_variaveis){
	using namespace std;
	vector<vector<long double>> Yn(num_variaveis+1, vector<long double>(num_variaveis+2));
	Yn = sistema;
	double g;
	for (unsigned i = 0; i < componentesVariantes.size(); i++){
		char tipo = componentesVariantes[i].nome[0];
    if (tipo=='C') {
      g=COND_ABERTO;
      Yn[componentesVariantes[i].a][componentesVariantes[i].a]+=g;
      Yn[componentesVariantes[i].b][componentesVariantes[i].b]+=g;
      Yn[componentesVariantes[i].a][componentesVariantes[i].b]-=g;
      Yn[componentesVariantes[i].b][componentesVariantes[i].a]-=g;

			Yn[componentesVariantes[i].x][componentesVariantes[i].a] -= 1;
			Yn[componentesVariantes[i].x][componentesVariantes[i].b] += 1;
			Yn[componentesVariantes[i].x][componentesVariantes[i].x] += 1/g;
		}
		else if (tipo=='L') {
      g=COND_CURTO;
      Yn[componentesVariantes[i].a][componentesVariantes[i].a]+=g;
      Yn[componentesVariantes[i].b][componentesVariantes[i].b]+=g;
      Yn[componentesVariantes[i].a][componentesVariantes[i].b]-=g;
      Yn[componentesVariantes[i].b][componentesVariantes[i].a]-=g;

			Yn[componentesVariantes[i].x][componentesVariantes[i].a] -= 1;
			Yn[componentesVariantes[i].x][componentesVariantes[i].b] += 1;
			Yn[componentesVariantes[i].x][componentesVariantes[i].x] += 1/g;
		}
		else{
			cout << "Elemento desconhecido: " << componentesVariantes[i].nome << endl;
			cin.get();
			exit(ERRO_ELEMENTO_DESCONHECIDO);
		}
	}
	
	resolverSistema(Yn, num_variaveis);
	vector<long double> solucao(num_variaveis+1);
	for (int i=0; i<num_variaveis+1; i++)
		solucao[i] = Yn[i][num_variaveis+1];
	return solucao;
}

void condensarLinhas(std::vector<std::vector<long double>>& sistema, std::vector<std::vector<int>> linhas){
	std::vector<int> linhas_para_remover;
	for (int index=0; index < linhas.size(); index++){
		linha_destino = linhas[index][0];
		for (int i=1; i < linhas[index].size(); i++){
			int linha_atual = linhas[index][i];
			sistema[linha_destino] += sistema[linha_atual]; 				
			linhas_para_remover.push_back(linha_atual);
		}	
	}
	
	linhas_para_remover = std::sort(linhas_para_remover.begin(), linhas_para_remover.end());

	for(int index=linhas_para_remover.size()-1; index >= 0; index--){
		sistema.erase(sistema.begin()+linhas_para_remover[index]);
	}
}

void condensarColunas(std::vector<std::vector<long double>>& sistema, std::vector<std::vector<int>> colunas){
	std::vector<int> colunas_para_remover;
	for (int index=0; index < colunas.size(); index++){
		coluna_destino = colunas[index][0];
		for (int i=1; i < colunas[index].size(); i++){
			int coluna_atual = colunas[index][i];
			for (int j=0; j<sistema.size(); j++){
				sistema[j][coluna_destino] += sistema[j][coluna_atual];
				colunas_para_remover.push_back(coluna_atual);
			}
		}	
	}
	
	colunas_para_remover = std::sort(colunas_para_remover.begin(), colunas_para_remover.end());

	for(int index=colunas_para_remover.size()-1; index >= 0; index--){
		for(int linha=0; linha < sistema.size(); linha++){
			sistema[linha].erase(sistema[linha].begin()+colunas_para_remover[index]);
		}
	}
}

int adicionarLista(std::vector<std::vector<int>>& lista, int a, int b){
	//Se a lista esta vazia, adiciona os elementos recebidos numa nova sublista
	if (lista.size() == 0){
		std::vector<int> novo(2);
		novo[0] = a;
		novo[1] = b;
		lista.push_back(novo);
		return OK;
	}

	//Procura em cada sublista se um elemento esta, e adiciona o outro caso ainda nao esteja nela
	for(int index = 0; index < lista.size(); index++){
		auto i = std::find(lista[index].begin(), lista[index].end(), a);
		if (i != lista[index].end()){
			//Encontrou a na lista
			auto j = std::find(lista[index].begin(), lista[index].end(), b);
			if (j == lista[index].end()){
				//Nao encontrou b na lista
				lista[index].push_back(b);
			}
			return OK;
		}

		auto i = std::find(lista[index].begin(), lista[index].end(), b);
		if (i != lista[index].end()){
			//Encontrou b na lista
			auto j = std::find(lista[index].begin(), lista[index].end(), a);
			if (j == lista[index].end()){
				//Nao encontrou a na lista
				lista[index].push_back(a);
			}
			return OK;
		}
	}

	//Se chegou aqui, nao encontrou nenhum dos elementos na lista, entao devemos adicionar uma nova sublista formada pelos dois elementos
	std::vector<int> novo(2);
	novo[0] = a;
	novo[1] = b;
	lista.push_back(novo);
	return OK;
}

void condensarVariaveis(std::vector<std::vector<long double>>& sistema, std::vector<Elemento> amp_ops){
	std::vector<std::vector<int>> colunas;
	std::vector<std::vector<int>> linhas;

	for (int index=0; index < amp_ops.size(); index++){
		adicionarLista(colunas, amp_ops[index].c, amp_ops[index].d);
		adicionarLista(linhas, amp_ops[index].a, amp_ops[index].b);
	}

	condensarLinhas(sistema, linhas);
	condensarColunas(sistema, colunas);

	return colunas; // colunas contem o mapa de variaveis necessario para o resultado final
}
