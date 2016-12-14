
/* Trabalho de Circuitos Eletricos II
 * Alice Fontes - alicefontes@poli.ufrj.br
 * Camyla Tsukuda Romao - camyla.romao@poli.ufrj.br
 * Paulo Oliveira Lenzi Valente - paulovalente@poli.ufrj.br
 * 
 * Baseado no programa mna1 (por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br)
 *
 * O simulador aceita os seguintes tipos de elementos no netlist:
 * Resistor:  R<nome> <no+> <no-> <resistencia>
 * VCCS:      G<nome> <io+> <io-> <vi+> <vi-> <transcondutancia>
 * VCVC:      E<nome> <vo+> <vo-> <vi+> <vi-> <ganho de tensao>
 * CCCS:      F<nome> <io+> <io-> <ii+> <ii-> <ganho de corrente>
 * CCVS:      H<nome> <vo+> <vo-> <ii+> <ii-> <transresistencia>
 * Fonte I:   I<nome> <io+> <io-> <corrente>
 * Fonte V:   V<nome> <vo+> <vo-> <tensao>
 * Amp. op.:  O<nome> <vo1> <vo2> <vi1> <vi2>
 * 
 * As fontes F e H tem o ramo de entrada em curto
 * O amplificador operacional ideal tem a saida suspensa
 * Os nos podem ser nomes
 *
 * */
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "classes.hpp"
#include "functions.hpp"
#include "constants.hpp"

/* Resolucao de sistema de equacoes lineares.
   Metodo de Gauss-Jordan com condensacao pivotal */
int resolverSistema(std::vector< std::vector<long double>>& Yn, int& num_variaveis){
  int i,j,l, a;
  long double t, p;

  for (i=1; i<=num_variaveis; i++) {
    t=0.0;
    a=i;
    for (l=i; l<=num_variaveis; l++) {
      if (std::abs(Yn[l][i])>std::abs(t)) {
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
    if (std::abs(t)<TOLG) {
			std::cout << "Sistema singular" << std::endl;
			#ifdef DEBUG_COMPLETO
				mostrarSistema("Estado de erro: ",Yn, num_variaveis);
			#endif
      exit(ERRO_RESOLUCAO_SISTEMA);
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
			#ifdef DEBUG_COMPLETO
				mostrarSistema("Estado atual: ",Yn, num_variaveis);
				std::cin.get();
			#endif
  }

	Yn[0][Yn[0].size()-1] = 0; // Tensao no terra = 0
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
		std::string& nomeArquivo,  
		int& num_variaveis,
		double& tempo_final,
		double& passo,
		std::string& metodo,
		unsigned& passos_por_ponto,
		std::vector<Elemento>& componentesNaoLineares){
		//std::vector<Elemento>& amp_ops){
	
	using namespace std;

	FILE *arquivo = NULL;
	char tipo;
	string na,nb,nc,nd;
	int num_elementos=0;

	if (argc == 2){
		arquivo = fopen(nomeArquivo.c_str(), "r");
	}

	while(!arquivo){
		cout << "Nome do arquivo com o netlist (ex: mna.net): ";
		cin >> nomeArquivo;
		arquivo=fopen(nomeArquivo.c_str(),"r");
			if (!arquivo) {
				cout << "Arquivo " << nomeArquivo << " inexistente" << endl;
			}
	}
	fclose(arquivo);
  cout << "Lendo netlist" << endl;
	
	ifstream input_file(nomeArquivo);
	num_variaveis=0;
	lista.push_back("0");
  
	string linha;
	getline(input_file, linha);
	cout << "Titulo: " << linha << endl;
	
	while (getline(input_file, linha)) {
		Elemento elemento;
    num_elementos++; /* Nao usa o netlist[0] */
		netlist.push_back(elemento);

    linha[0]=toupper(linha[0]);
    tipo=linha[0];

		istringstream input(linha);

    if (tipo=='R') {
			input >> netlist[num_elementos].nome >> na >> nb >> netlist[num_elementos].valor;
      netlist[num_elementos].a=numero(lista, na, num_variaveis);
      netlist[num_elementos].b=numero(lista, nb, num_variaveis);
      cout << netlist[num_elementos].nome << " " << netlist[num_elementos].a << " " << netlist[num_elementos].b << " " << netlist[num_elementos].valor << endl;
    }
		else if (tipo == 'D' || tipo == '$'){
			num_elementos--;
			netlist.pop_back();
			elemento.tipoFonte = "curto";
			if (tipo == 'D'){
				input >> elemento.nome >> na >> nb;
				elemento.valor = 0;
			}	
			else{
				input >> elemento.nome >> na >> nb >> nc >> nd >> elemento.valor;
				elemento.c = numero(lista, nc, num_variaveis);
				elemento.d = numero(lista, nd, num_variaveis);
			}
			elemento.a = numero(lista, na, num_variaveis);
			elemento.b = numero(lista, nb, num_variaveis);
			componentesNaoLineares.push_back(elemento);
			cout << elemento.nome << " " << elemento.a << " " << elemento.b;
			if(tipo == '$')
				cout << " " << elemento.c << " " << elemento.d << " " << elemento.valor;
			cout << endl;
		}
		else if (tipo == 'I' || tipo == 'V'){
			num_elementos--;
			netlist.pop_back();
			input >> elemento.nome >> na >> nb >> elemento.tipoFonte;

			elemento.a = numero(lista, na, num_variaveis);
			elemento.b = numero(lista, nb, num_variaveis);
			cout << elemento.nome << " " << elemento.a << " " << elemento.b << " " << elemento.tipoFonte;
			if (elemento.tipoFonte == "DC"){
				input >> elemento.valor;
				cout << " " << elemento.valor << endl;
			}
			else if(elemento.tipoFonte == "SIN"){
				input >> elemento.nivelDC >> elemento.amplitude >> elemento.frequencia >> elemento.atraso >> elemento.amortecimento >> elemento.defasagem >> elemento.numeroCiclos;
				cout << " " << elemento.nivelDC << " " << elemento.amplitude << " " << elemento.frequencia << " " << elemento.atraso << " " << elemento.amortecimento << " " << elemento.defasagem << " " << elemento.numeroCiclos << endl;
			}
			else if(elemento.tipoFonte == "PULSE"){
				input >> elemento.amplitude >> elemento.amplitude2 >> elemento.atraso >> elemento.tempoSubida >> elemento.tempoDescida >> elemento.tempoLigada >> elemento.periodo >> elemento.numeroCiclos;
				cout << " " << elemento.amplitude << " " << elemento.amplitude2 << " " << elemento.atraso << " " << elemento.tempoSubida << " " << elemento.tempoDescida << " " << elemento.tempoLigada << " " << elemento.periodo << " " << elemento.numeroCiclos << endl;
			}
			else{
				cout << "Tipo da fonte " << elemento.nome << " nao reconhecido." << endl;
				cout << "Tipo recebido: " << elemento.tipoFonte << endl;
				exit(ERRO_ELEMENTO_DESCONHECIDO);
			}
			componentesVariantes.push_back(elemento);
		}
		else if (tipo=='C' || tipo=='L'){
			netlist.pop_back();
			num_elementos--;
			input >> elemento.nome >> na >> nb >> elemento.valor; 	
			elemento.a = numero(lista, na, num_variaveis);
			elemento.b = numero(lista, nb, num_variaveis);
			cout << elemento.nome << " " << elemento.a << " " << elemento.b << " " << elemento.valor << endl;
			componentesVariantes.push_back(elemento);
		}
    else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
			input >> netlist[num_elementos].nome >> na >> nb >> nc >> nd >> netlist[num_elementos].valor;
      netlist[num_elementos].a=numero(lista, na, num_variaveis);
      netlist[num_elementos].b=numero(lista, nb, num_variaveis);
      netlist[num_elementos].c=numero(lista, nc, num_variaveis);
      netlist[num_elementos].d=numero(lista, nd, num_variaveis);
      cout << netlist[num_elementos].nome << " " << elemento.a << " " <<  elemento.b << " " << elemento.c << " " << elemento.d << " " << netlist[num_elementos].valor << endl;
    }
    else if (tipo=='O') {
			input >> netlist[num_elementos].nome >> na >> nb >> nc >> nd;
      netlist[num_elementos].a=numero(lista, na, num_variaveis);
      netlist[num_elementos].b=numero(lista, nb, num_variaveis);
      netlist[num_elementos].c=numero(lista, nc, num_variaveis);
      netlist[num_elementos].d=numero(lista, nd, num_variaveis);
			//amp_ops.push_back(netlist[num_elementos]);
      cout << netlist[num_elementos].nome << " " << netlist[num_elementos].a << " " <<  netlist[num_elementos].b << " " << netlist[num_elementos].c << " " << netlist[num_elementos].d << endl;
    }
		else if (tipo=='K') {
			input >> netlist[num_elementos].nome >> na >> nb >> nc >> nd >> netlist[num_elementos].valor;
			netlist[num_elementos].a = numero(lista, na, num_variaveis);
			netlist[num_elementos].b = numero(lista, nb, num_variaveis);
			netlist[num_elementos].c = numero(lista, nc, num_variaveis);
			netlist[num_elementos].d = numero(lista, nd, num_variaveis);
			cout << netlist[num_elementos].nome << " " << netlist[num_elementos].a << " " << netlist[num_elementos].b << " " << netlist[num_elementos].c << " " << netlist[num_elementos].d << " " << netlist[num_elementos].valor << endl;
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

void adicionarVariaveis(std::vector<std::string>& lista, std::vector<Elemento>& netlist, int& num_variaveis, int& num_nos){
	char tipo;

  num_nos=num_variaveis;
  for (auto &componente: netlist) {
    tipo=componente.nome[0];
    if (tipo=='V' || 
				tipo=='E' || 
				tipo=='F' || 
				tipo=='O' || 
				tipo=='K' || 
				tipo=='C' || 
				tipo=='L' ||
				tipo=='D' || 
				tipo=='$') 
		{
      num_variaveis++;
      lista.push_back("j"+componente.nome);
      componente.x=num_variaveis;
    }
    else if (tipo=='H') {
      num_variaveis+=2;
      lista.push_back("jx"+componente.nome);
      componente.x=num_variaveis-1;
      lista.push_back("jy"+componente.nome);
      componente.y=num_variaveis;
    }
  }
}

void listarVariaveis(std::vector<std::string> lista, int num_variaveis){
	std::cout << "Variaveis internas:  " << std::endl;
  for (int i=0; i<=num_variaveis; i++)
		std::cout << i << " -> " << lista[i] << std::endl;
}

void mostrarNetlist(std::vector<Elemento> netlist){
	using namespace std;
	char tipo;
	unsigned i;

	cout << "Netlist interno final" << endl;
		for (i=1; i<=(netlist.size()-1); i++) {
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

void montarSistemaDC(std::vector<Elemento>& netlist, std::vector< std::vector<long double>>& Yn, int num_variaveis){
	int i, j;
	long double g;
	char tipo;
	

	for (i=0; i<=num_variaveis; i++) {
    for (j=0; j<=num_variaveis+1; j++)
      Yn[i][j]=0;
  }

  /* Monta estampas */
	for (auto &elemento: netlist){
    tipo=elemento.nome[0];
    if (tipo=='R') {
      g=1/elemento.valor;
      Yn[elemento.a][elemento.a]+=g;
      Yn[elemento.b][elemento.b]+=g;
      Yn[elemento.a][elemento.b]-=g;
      Yn[elemento.b][elemento.a]-=g;
    }
    else if (tipo=='G') {
      g=elemento.valor;
      Yn[elemento.a][elemento.c]+=g;
      Yn[elemento.b][elemento.d]+=g;
      Yn[elemento.a][elemento.d]-=g;
      Yn[elemento.b][elemento.c]-=g;
    }
    else if (tipo=='I') {
      g=elemento.valor;
      Yn[elemento.a][num_variaveis+1]-=g;
      Yn[elemento.b][num_variaveis+1]+=g;
    }
    else if (tipo=='V') {
      Yn[elemento.a][elemento.x]+=1;
      Yn[elemento.b][elemento.x]-=1;
      Yn[elemento.x][elemento.a]-=1;
      Yn[elemento.x][elemento.b]+=1;
      Yn[elemento.x][num_variaveis+1]-=elemento.valor;
    }
    else if (tipo=='E') {
      g=elemento.valor;
      Yn[elemento.a][elemento.x]+=1;
      Yn[elemento.b][elemento.x]-=1;
      Yn[elemento.x][elemento.a]-=1;
      Yn[elemento.x][elemento.b]+=1;
      Yn[elemento.x][elemento.c]+=g;
      Yn[elemento.x][elemento.d]-=g;
    }
    else if (tipo=='F') {
      g=elemento.valor;
      Yn[elemento.a][elemento.x]+=g;
      Yn[elemento.b][elemento.x]-=g;
      Yn[elemento.c][elemento.x]+=1;
      Yn[elemento.d][elemento.x]-=1;
      Yn[elemento.x][elemento.c]-=1;
      Yn[elemento.x][elemento.d]+=1;
    }
    else if (tipo=='H') {
      g=elemento.valor;
      Yn[elemento.a][elemento.y]+=1;
      Yn[elemento.b][elemento.y]-=1;
      Yn[elemento.c][elemento.x]+=1;
      Yn[elemento.d][elemento.x]-=1;
      Yn[elemento.y][elemento.a]-=1;
      Yn[elemento.y][elemento.b]+=1;
      Yn[elemento.x][elemento.c]-=1;
      Yn[elemento.x][elemento.d]+=1;
      Yn[elemento.y][elemento.x]+=g;
    }
    else if (tipo=='O') {
      Yn[elemento.a][elemento.x]+=1;
      Yn[elemento.b][elemento.x]-=1;
      Yn[elemento.x][elemento.c]+=1;
      Yn[elemento.x][elemento.d]-=1;
    }
		else if (tipo == 'K') {
			Yn[elemento.a][elemento.x] -= elemento.valor;
			Yn[elemento.b][elemento.x] += elemento.valor;
			Yn[elemento.c][elemento.x] += 1;
			Yn[elemento.d][elemento.x] -= 1;

			Yn[elemento.x][elemento.a] += elemento.valor;
			Yn[elemento.x][elemento.b] -= elemento.valor;
			Yn[elemento.x][elemento.c] -= 1;
			Yn[elemento.x][elemento.d] += 1;
		}
	
	#ifdef DEBUG
	/* Opcional: Mostra o sistema apos a montagem da estampa */
		std::ostringstream msg;
		msg <<  "Sistema apos a estampa de " << elemento.nome;
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

void adicionarEstampasComponentesVariantes(std::vector<std::vector<long double>>& sistema, std::vector<Elemento> componentesVariantes, std::vector<long double> solucao_anterior, double passo, double t){
	// Funcao que adiciona as estampas da aproximacao de cada componente variante
	using std::cout;
	using std::cin;
	using std::endl;

	for(auto &componente: componentesVariantes){
		char tipo = componente.nome[0];
		unsigned index = sistema[componente.a].size() - 1;
		if (tipo == 'C'){
			long double G = 2*(componente.valor)/passo;
			long double V = solucao_anterior[componente.a] - solucao_anterior[componente.b] + passo/(2*componente.valor) * solucao_anterior[componente.x];

			sistema[componente.a][componente.a] += G;
			sistema[componente.a][componente.b] -= G;
			sistema[componente.b][componente.a] -= G;
			sistema[componente.b][componente.b] += G;
			
			sistema[componente.a][index] += V * G;
			sistema[componente.b][index] -= V * G;

			sistema[componente.x][componente.a] += -G;
			sistema[componente.x][componente.b] += G;
			sistema[componente.x][componente.x] += 1; 
			sistema[componente.x][index]        += -G*V;
		}
		else if (tipo == 'L'){
			/*long double G = 2*componente.valor/passo;
			long double I = solucao_anterior[componente.x] + passo/(2*componente.valor)*(solucao_anterior[componente.a] - solucao_anterior[componente.b]);  

			sistema[componente.a][componente.a] += G;
			sistema[componente.a][componente.b] -= G;
			sistema[componente.b][componente.a] -= G;
			sistema[componente.b][componente.b] += G;
			sistema[componente.a][index] -= I;
			sistema[componente.b][index] += I;

			sistema[componente.x][componente.a] += G;
			sistema[componente.x][componente.b] -= G;
			sistema[componente.x][componente.x] -= 1;
			sistema[componente.x][index] -= I;*/

			sistema[componente.a][componente.x] += 1;
			sistema[componente.b][componente.x] -= 1;

			sistema[componente.x][componente.a] -= 1;
			sistema[componente.x][componente.b] += 1;
			sistema[componente.x][componente.x] += 2*componente.valor/passo;
			sistema[componente.x][index] += 2*componente.valor/passo * solucao_anterior[componente.x] + (solucao_anterior[componente.a] - solucao_anterior[componente.b]);
		}
		else if(tipo == 'V'){
			if (componente.tipoFonte == "DC"){
				sistema[componente.x][componente.a] += 1;
				sistema[componente.x][componente.b] -= 1;
				sistema[componente.x][index] += componente.valor;

				sistema[componente.a][componente.x] += 1;
				sistema[componente.b][componente.x] -= 1;
			}
			else if (componente.tipoFonte == "SIN"){
				sistema[componente.x][componente.a] += 1;
				sistema[componente.x][componente.b] -= 1;
				sistema[componente.x][index] += componente.valorFonteSenoidal(t);

				sistema[componente.a][componente.x] += 1;
				sistema[componente.b][componente.x] -= 1;
			}
			else if (componente.tipoFonte == "PULSE"){
				sistema[componente.x][componente.a] += 1;
				sistema[componente.x][componente.b] -= 1;
				sistema[componente.x][index] += componente.valorFontePulse(t,passo);

				sistema[componente.a][componente.x] += 1;
				sistema[componente.b][componente.x] -= 1;
			}
			else{
				cout << "Elemento desconhecido: " << componente.nome << endl;
				cin.get();
				exit(ERRO_ELEMENTO_DESCONHECIDO);
			}
		}
		else if(tipo == 'I'){
			if (componente.tipoFonte == "DC"){
				sistema[componente.a][index] -= componente.valor;
				sistema[componente.b][index] += componente.valor;
			}
			else if (componente.tipoFonte == "SIN"){
				sistema[componente.a][index] -= componente.valorFonteSenoidal(t);
				sistema[componente.b][index] += componente.valorFonteSenoidal(t);
			}
			else if (componente.tipoFonte == "PULSE"){
				sistema[componente.a][index] -= componente.valorFontePulse(t,passo);
				sistema[componente.b][index] += componente.valorFontePulse(t,passo);
			}
			else{
				cout << "Elemento desconhecido: " << componente.nome << endl;
				cin.get();
				exit(ERRO_ELEMENTO_DESCONHECIDO);
			}
		}
		else{
			cout << "Componente desconhecido: " << componente.nome << endl;	
			exit(ERRO_ELEMENTO_DESCONHECIDO);
		}

		#ifdef DEBUG
		mostrarSistema("Sistema apos a estampa de "+componente.nome+": ", sistema, sistema.size()-1);
		#endif 
	}

}

int simulacaoTrapezios(
		std::vector<Elemento> netlist, 
		std::vector<Elemento> componentesVariantes, 
		std::vector<std::string>& lista, 
		int num_nos, 
		int& num_variaveis, 
		double passo, 
		double tempo_final, 
		unsigned passos_por_ponto,
		std::vector<std::vector<long double>>& solucoes,
		std::vector<Elemento>& componentesNaoLineares){
	//	std::vector<Elemento>& amp_ops){
	// montar sistema dc
	// a cada iteracao:
	//   adiciona estampas dos componentes variantes no tempo
	//   resolve o sistema nodal
	//   guarda a solucao da iteracao na linha de uma matriz
	using namespace std;
	adicionarVariaveis(lista, componentesVariantes, num_variaveis, num_nos); 
	if(componentesNaoLineares.size() != 0){
		adicionarVariaveis(lista, componentesNaoLineares, num_variaveis, num_nos);
	}
	
	vector<vector<long double>> sistemaEsqueleto(num_variaveis+1, vector<long double>(num_variaveis+2));
	montarSistemaDC(netlist, sistemaEsqueleto, num_variaveis);
	vector<vector<long double>> sistemaCompleto(num_variaveis+1, vector<long double>(num_variaveis+2));

	//vector<vector<long double>> solucoes;
	#ifdef DEBUG
		cout << "Calculando ponto de operacao" << endl;
		cin.get();
	#endif
	vector<long double> solucao_anterior = resolverPontoOperacao(sistemaEsqueleto, componentesVariantes, componentesNaoLineares, num_variaveis, passo);
	solucoes.push_back(solucao_anterior);
	#ifdef DEBUG
		cout << "Iniciando solucao por trapezios" << endl;
		listarVariaveis(lista, num_variaveis);
		cin.get();
	#endif

	for (double t0 = 0; t0 <= tempo_final; t0+=passo/passos_por_ponto){
		#ifdef DEBUG
			cout << "t: " << t0 << endl;
			for(auto &i: solucoes[solucoes.size()-1]){
				cout << i << endl;
			}
			cout << endl;
			cin.get();
		#endif
		sistemaCompleto = sistemaEsqueleto;			
		adicionarEstampasComponentesVariantes(sistemaCompleto, componentesVariantes, solucao_anterior, passo/passos_por_ponto, t0);
		vector<long double> solucao(num_variaveis+1);
		if (componentesNaoLineares.size() == 0){
			resolverSistema(sistemaCompleto, num_variaveis);
			for (int i=0; i<num_variaveis+1; i++)
				solucao[i] = sistemaCompleto[i][num_variaveis+1];
		}
		else{
			bool convergiu;
			solucao = resolverNewtonRaphson(sistemaCompleto, componentesNaoLineares, num_variaveis, solucao_anterior, convergiu);
			if (!convergiu){
				solucoes.push_back(solucao);
				return ERRO_CONVERGENCIA_NEWTON_RAPHSON;
			}
		}
		solucao_anterior = solucao;
		solucoes.push_back(solucao);
	}
	
	return OK;

}

std::vector<long double> resolverNewtonRaphson(std::vector<std::vector<long double>> sistemaInicial, std::vector<Elemento> componentesNaoLineares, int num_variaveis, std::vector<long double> solucaoInicial, bool& convergiu){
	std::vector<long double> solucao_anterior = solucaoInicial;
	std::vector<long double> solucao = solucaoInicial;
	std::vector<std::vector<long double>> sistema;
	int reinicializacoes = 0;
	convergiu = false;

	srand(time(0));
	
	do{
		for(unsigned iter = 0; iter < MAX_ITER; iter++){
			sistema = sistemaInicial;
			unsigned ultimaColuna = sistema[0].size() - 1;
			for(auto &componente: componentesNaoLineares){
				//adicionar estampas dos elementos baseado na solucao anterior
				char tipo = componente.nome[0];
				if(tipo == 'D'){
					double v = solucao_anterior[componente.a] - solucao_anterior[componente.b];
					if(v > componente.valor){
						//curto
#ifdef DEBUG_DIODO
							std::cout << "Entrou >" << std::endl;
#endif						
						sistema[componente.a][componente.x]    += 1;
						sistema[componente.b][componente.x]    -= 1;
						sistema[componente.x][componente.a]    += 1;
						sistema[componente.x][componente.b]    -= 1;
						sistema[componente.x][ultimaColuna] += componente.valor;
						componente.tipoFonte = "curto";
					}
					else if (std::abs(v - componente.valor) < TOLG){
						if(componente.tipoFonte == "curto"){
#ifdef DEBUG_DIODO
							std::cout << "Entrou =" << std::endl;
#endif						
							sistema[componente.a][componente.x]    += 1;
							sistema[componente.b][componente.x]    -= 1;
							sistema[componente.x][componente.a]    += 1;
							sistema[componente.x][componente.b]    -= 1;
							sistema[componente.x][ultimaColuna] += componente.valor;
						}
						else{
							sistema[componente.x][componente.x]    += 1;
							sistema[componente.x][ultimaColuna] += 0;
						}
					}
					else{
						//aberto
#ifdef DEBUG_DIODO
							std::cout << "Entrou <" << std::endl;
#endif						
						sistema[componente.x][componente.x]    += 1;
						sistema[componente.x][ultimaColuna] += 0;
						componente.tipoFonte = "aberto";
					}
				}
				else if(tipo == '$'){
					double v = solucao_anterior[componente.c] - solucao_anterior[componente.d];
					if(v > componente.valor){
						//curto
						sistema[componente.a][componente.x]    += 1;
						sistema[componente.b][componente.x]    -= 1;
						sistema[componente.x][componente.a]    += 1;
						sistema[componente.x][componente.b]    -= 1;
						sistema[componente.x][ultimaColuna] += 0;
					}
					else if (v == componente.valor){
						if(componente.tipoFonte == "curto"){
							sistema[componente.a][componente.x]    += 1;
							sistema[componente.b][componente.x]    -= 1;
							sistema[componente.x][componente.a]    += 1;
							sistema[componente.x][componente.b]    -= 1;
							sistema[componente.x][ultimaColuna] += 0;
						}
						else{
							sistema[componente.x][componente.x]    += 1;
							sistema[componente.x][ultimaColuna] += 0;
						}
					}
					else{
						//aberto
						sistema[componente.x][componente.x]    += 1;
						sistema[componente.x][ultimaColuna] += 0;
					}
				}
			}
			#ifdef DEBUG
				mostrarSistema("Sistema antes da resolucao: ", sistema, num_variaveis);
			#endif

			resolverSistema(sistema, num_variaveis);
			bool divergiu = false;
			for(int index = 0; index <= num_variaveis; index++){
				solucao[index] = sistema[index][num_variaveis+1];
				if(std::abs(solucao[index] - solucao_anterior[index]) > TOLG)
					divergiu = true;
			}

			solucao_anterior = solucao;
			
			if((!divergiu) && (iter > 0)){
				convergiu = true;
				return solucao;
			}
		}//end for
		//Nao convergiu: reinicializar a solucao aleatoriamente e tentar novamente
		for(int index = 0; index < num_variaveis+1; index++){
			solucao_anterior[index] = (rand()%100)/100;
			solucao[index] = solucao_anterior[index];
			reinicializacoes++;
		}
	} while (reinicializacoes < MAX_ITER/10);
	return solucao;
}

std::vector<long double> resolverPontoOperacao(std::vector<std::vector<long double>> sistema, std::vector<Elemento> componentesVariantes, std::vector<Elemento> componentesNaoLineares, int num_variaveis, double passo){
	using namespace std;
	vector<vector<long double>> Yn(num_variaveis+1, vector<long double>(num_variaveis+2));
	Yn = sistema;
	double g;
	double t = 0.0;
	for(auto &elemento: componentesVariantes){
		char tipo = elemento.nome[0];
		unsigned index = Yn[elemento.a].size() - 1;
    if (tipo=='C') {
      g=COND_ABERTO;
			Yn[elemento.a][elemento.x] += 1;
			Yn[elemento.b][elemento.x] -= 1;

			Yn[elemento.x][elemento.a] -= 1;
			Yn[elemento.x][elemento.b] += 1;
			Yn[elemento.x][elemento.x] += 1/g;
		}
		else if (tipo=='L') {
      g=COND_CURTO;
			Yn[elemento.a][elemento.x] += 1;
			Yn[elemento.b][elemento.x] -= 1;

			Yn[elemento.x][elemento.a] -= 1;
			Yn[elemento.x][elemento.b] += 1;
			Yn[elemento.x][elemento.x] += 1/g;
		}
		else if(tipo == 'V'){
			if (elemento.tipoFonte == "DC"){
				Yn[elemento.x][elemento.a] += 1;
				Yn[elemento.x][elemento.b] -= 1;
				Yn[elemento.x][index] += elemento.valor;

				Yn[elemento.a][elemento.x] += 1;
				Yn[elemento.b][elemento.x] -= 1;
			}
			else if (elemento.tipoFonte == "SIN"){
				Yn[elemento.x][elemento.a] += 1;
				Yn[elemento.x][elemento.b] -= 1;
				Yn[elemento.x][index] += elemento.valorFonteSenoidal(t);

				Yn[elemento.a][elemento.x] += 1;
				Yn[elemento.b][elemento.x] -= 1;
			}
			else if (elemento.tipoFonte == "PULSE"){
				Yn[elemento.x][elemento.a] += 1;
				Yn[elemento.x][elemento.b] -= 1;
				Yn[elemento.x][index] += elemento.valorFontePulse(t,passo);

				Yn[elemento.a][elemento.x] += 1;
				Yn[elemento.b][elemento.x] -= 1;
			}
			else{
				cout << "Elemento desconhecido: " << elemento.nome << endl;
				cin.get();
				exit(ERRO_ELEMENTO_DESCONHECIDO);
			}
		}
		else if(tipo == 'I'){
			if (elemento.tipoFonte == "DC"){
				Yn[elemento.a][num_variaveis+1] -= elemento.valor;
				Yn[elemento.b][num_variaveis+1] += elemento.valor;
			}
			else if (elemento.tipoFonte == "SIN"){
				Yn[elemento.a][num_variaveis+1] -= elemento.valorFonteSenoidal(t);
				Yn[elemento.b][num_variaveis+1] += elemento.valorFonteSenoidal(t);
			}
			else if (elemento.tipoFonte == "PULSE"){
				Yn[elemento.a][num_variaveis+1] -= elemento.valorFontePulse(t,passo);
				Yn[elemento.b][num_variaveis+1] += elemento.valorFontePulse(t,passo);
			}
			else{
				cout << "Elemento desconhecido: " << elemento.nome << endl;
				cin.get();
				exit(ERRO_ELEMENTO_DESCONHECIDO);
			}
		}
		else{
			cout << "Elemento desconhecido: " << elemento.nome << endl;
			cin.get();
			exit(ERRO_ELEMENTO_DESCONHECIDO);
		}
	}

#ifdef DEBUG
	mostrarSistema("Ponto de operacao: ",Yn, num_variaveis);
	cin.get();
#endif
	
		vector<long double> solucao(num_variaveis+1);
		if (componentesNaoLineares.size() == 0){
			resolverSistema(Yn, num_variaveis);
			for (int i=0; i<num_variaveis+1; i++)
				solucao[i] = Yn[i][num_variaveis+1];
		}
		else{
#ifdef DEBUG
			cout << "Newton-Raphson Ponto de Operacao" << endl;
#endif
			bool convergiu;
			vector<long double> solucao_anterior(num_variaveis+1);
			for(auto &i: solucao_anterior) i = 0.1;
			solucao = resolverNewtonRaphson(Yn, componentesNaoLineares, num_variaveis, solucao_anterior, convergiu);
			if (!convergiu){
				exit(ERRO_CONVERGENCIA_NEWTON_RAPHSON);
			}
		}
	/*resolverSistema(Yn, num_variaveis);
	vector<long double> solucao(num_variaveis+1);
	for (int i=0; i<num_variaveis+1; i++)
		solucao[i] = Yn[i][num_variaveis+1];*/
	return solucao;
}

/*void condensarLinhas(std::vector<std::vector<long double>>& sistema, std::vector<std::vector<int>> linhas){
	std::vector<int> linhas_para_remover;
	for (unsigned index=0; index < linhas.size(); index++){
		int	linha_destino = linhas[index][0];
		for (unsigned i=1; i < linhas[index].size(); i++){
			int linha_atual = linhas[index][i];
			for (unsigned j=0; j < sistema[linha_destino].size(); j++)
				sistema[linha_destino][j] += sistema[linha_atual][j]; 				
			linhas_para_remover.push_back(linha_atual);
		}	
	}
	
	std::sort(linhas_para_remover.begin(), linhas_para_remover.end());

	for(int index=linhas_para_remover.size()-1; index >= 0; index--){
		sistema.erase(sistema.begin()+linhas_para_remover[index]);
	}
}

std::vector<std::vector<std::string>> condensarColunas(std::vector<std::vector<long double>>& sistema, std::vector<std::vector<int>> colunas, std::vector<std::string> lista){
	std::vector<std::vector<std::string>> mapaVariaveis(lista.size());
	for (unsigned index=0; index < lista.size(); index++){
		mapaVariaveis[index].push_back(lista[index]);
	}

	std::vector<int> colunas_para_remover;
	for (unsigned index=0; index < colunas.size(); index++){
		int coluna_destino = colunas[index][0];
		for (unsigned i=1; i < colunas[index].size(); i++){
			int coluna_atual = colunas[index][i];
			for (unsigned j=0; j<sistema.size(); j++){
				sistema[j][coluna_destino] += sistema[j][coluna_atual];
				colunas_para_remover.push_back(coluna_atual);
				mapaVariaveis[coluna_destino].push_back(mapaVariaveis[coluna_atual][0]);
			}
		}	
	}
	
	std::sort(colunas_para_remover.begin(), colunas_para_remover.end());
	
	for(unsigned index=colunas_para_remover.size()-1; index > 0; index--){
		for(unsigned linha=0; linha < sistema.size(); linha++){
			sistema[linha].erase(sistema[linha].begin()+colunas_para_remover[index]);
		}
		mapaVariaveis.erase(mapaVariaveis.begin()+index);
	}
	return mapaVariaveis;
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
	for(unsigned index = 0; index < lista.size(); index++){
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

		i = std::find(lista[index].begin(), lista[index].end(), b);
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

std::vector< std::vector<std::string> > condensarVariaveis(std::vector< std::vector<long double> >& sistema, std::vector<Elemento> amp_ops, std::vector<std::string> lista){
	std::vector< std::vector<int> > colunas;
	std::vector< std::vector<int> > linhas;
	
	using namespace std;

	for (unsigned index=0; index < amp_ops.size(); index++){
		adicionarLista(colunas, amp_ops[index].c, amp_ops[index].d);
		adicionarLista(linhas, amp_ops[index].a, amp_ops[index].b);
	}


	condensarLinhas(sistema, linhas);
	std::vector< std::vector<std::string> > mapaVariaveis = condensarColunas(sistema, colunas, lista);

	return mapaVariaveis; // colunas contem o mapa de variaveis necessario para o resultado final
}*/


std::string converterExtensao(std::string nomeArquivo, std::string extensao);
std::string converterExtensao(std::string nomeArquivo, std::string extensao){
	std::istringstream ss(nomeArquivo);
	std::string palavra;
	std::string nomeArquivoSaida = "";
	std::string ultimaPalavra;
	
	std::getline(ss, ultimaPalavra, '.');
	
	bool entrou = false;
	while(std::getline(ss, palavra, '.')){
		entrou = true;
		nomeArquivoSaida += ultimaPalavra + ".";
		ultimaPalavra = palavra;
	}
	if(!entrou)//caso nao haja extensao de arquivo, apenas faz o append
		nomeArquivoSaida += ultimaPalavra+".";

	nomeArquivoSaida += extensao;
	return nomeArquivoSaida;
}

void escreverResultadosNoArquivo(
		std::string nomeArquivo, 
		std::vector<std::vector<long double>> solucoes, 
		double passo, 
		double tempo_final, 
		unsigned passos_por_ponto, 
		std::vector<std::string> lista){
/*
 * Funcao que grava os resultados em um arquivo com nome igual ao netlist. 
 * Ex: netlist: circuit.net
 *		 arquivo: circuit.res
 */
	using std::string;
	using std::vector;
	using std::cout;
	using std::endl;

	std::ofstream arquivoSaida;
	string nomeArquivoSaida = converterExtensao(nomeArquivo,"res");
	//std::string nomeArquivoSaida = nomeArquivo + ".res";
	cout << "Iniciando a escrita no arquivo: " << nomeArquivoSaida << endl;

	arquivoSaida.open(nomeArquivoSaida);
	arquivoSaida << "t ";
	for(auto &variavel: lista){
		arquivoSaida << variavel;
		//if (variavel != lista[lista.size()-1])
			arquivoSaida << " ";
	}
	arquivoSaida << "\n";

	double tempo = -passo;
	unsigned contador_passos = 0;
	solucoes.erase(solucoes.begin());
	for(auto &solucao: solucoes){
		if (contador_passos % passos_por_ponto != 0){
			contador_passos++;
			continue;
		}

		tempo += passo;
		arquivoSaida << tempo << " ";
		int ii = 0;
		for(unsigned index_solucao = 0; index_solucao < lista.size(); index_solucao++){
			arquivoSaida << solucao[index_solucao];
			//std::cout << lista[ii] << ": "<< solucao[index_solucao] << std::endl;
			ii++;
			//if (val != solucao[solucao.size()-1])
				arquivoSaida << " ";
		}
		arquivoSaida << "\n";
	}
	arquivoSaida.close();
}
