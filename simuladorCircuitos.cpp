/*
Programa de demonstracao de analise nodal modificada
Por Antonio Carlos M. de Queiroz acmq@coe.ufrj.br
Versao 1.0 - 6/9/2000
Versao 1.0a - 8/9/2000 Ignora comentarios
Versao 1.0b - 15/9/2000 Aumentado Yn, retirado Js
Versao 1.0c - 19/2/2001 Mais comentarios
Versao 1.0d - 16/2/2003 Tratamento correto de nomes em minusculas
Versao 1.0e - 21/8/2008 Estampas sempre somam. Ignora a primeira linha
Versao 1.0f - 21/6/2009 Corrigidos limites de alocacao de Yn
Versao 1.0g - 15/10/2009 Le as linhas inteiras
Versao 1.0h - 18/6/2011 Estampas correspondendo a modelos
Versao 1.0i - 03/11/2013 Correcoes em *p e saida com sistema singular.
Versao 1.0j - 26/11/2015 Evita operacoes com zero.

Camyla Tsukuda Romao
Paulo Oliveira Lenzi Valente

Versao 2.0 - 18/11/2016 Traducao para C++
*/

/*
Elementos aceitos e linhas do netlist:

Resistor:  R<nome> <no+> <no-> <resistencia>
VCCS:      G<nome> <io+> <io-> <vi+> <vi-> <transcondutancia>
VCVC:      E<nome> <vo+> <vo-> <vi+> <vi-> <ganho de tensao>
CCCS:      F<nome> <io+> <io-> <ii+> <ii-> <ganho de corrente>
CCVS:      H<nome> <vo+> <vo-> <ii+> <ii-> <transresistencia>
Fonte I:   I<nome> <io+> <io-> <corrente>
Fonte V:   V<nome> <vo+> <vo-> <tensao>
Amp. op.:  O<nome> <vo1> <vo2> <vi1> <vi2>

As fontes F e H tem o ramo de entrada em curto
O amplificador operacional ideal tem a saida suspensa
Os nos podem ser nomes
*/

#define versao "2.0 - 18/11/2016"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cstdlib>
//#include <ctype>
#include <cmath>

#include "simuladorLibrary.h"


int
  num_elementos, /* Elementos */
  num_variaveis, /* Variaveis */
  num_nos; /* Nos */
  //i,j,k;



/* Foram colocados limites nos formatos de leitura para alguma protecao
   contra excesso de caracteres nestas variaveis */

//double g;
  //g,
  //Yn[MAX_NOS+1][MAX_NOS+2];

/* Resolucao de sistema de equacoes lineares.
   Metodo de Gauss-Jordan com condensacao pivotal */
int resolversistema(std::vector< std::vector<double>>& Yn)
{
  int i,j,l, a;
  double t, p;

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
      printf("Sistema singular\n");
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
int numero(std::vector<std::string>& lista, std::string nome)
{
 /* int i,achou;

  i=0; achou=0;
  while (!achou && i<=num_variaveis)
    if (!(achou=!lista[i].compare(nome))) i++;
  if (!achou) {
    if (num_variaveis==MAX_NOS) {
			std::cout << "O programa so aceita ate "<< num_variaveis << " nos" << std::endl;
      exit(ERRO_NUM_VARIAVEIS);
    }
    num_variaveis++;
    lista.push_back(nome);
    return num_variaveis; / novo no /
  }
  else {
    return i; / no ja conhecido /
  }*/
	auto i = std::find(lista.begin(), lista.end(), nome);
	if (i != lista.end()){
		return i - lista.begin();
	}
	
	lista.push_back(nome);
	num_variaveis++;
	return num_variaveis;	
}
void leituraNetlist(std::vector<std::string>& lista, std::vector<Elemento>& netlist, int argc, char **argv){
	using namespace std;

	FILE *arquivo = NULL;
  char nomearquivo[MAX_LINHA+1],
			 tipo;
	string na,nb,nc,nd;

	if (argc == 2){
		strcpy(nomearquivo, argv[1]);
		arquivo = fopen(nomearquivo, "r");
	}

	while(!arquivo){
		cout << "Nome do arquivo com o netlist (ex: mna.net): ";
		cin >> nomearquivo;
		arquivo=fopen(nomearquivo,"r");
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
	
	Elemento elemento;
	while (getline(input_file, linha)) {
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
      netlist[num_elementos].a=numero(lista, na);
      netlist[num_elementos].b=numero(lista, nb);
    }
    else if (tipo=='G' || tipo=='E' || tipo=='F' || tipo=='H') {
			input >> netlist[num_elementos].nome >> na >> nb >> nc >> nd >> netlist[num_elementos].valor;
      cout << netlist[num_elementos].nome << " " << na << " " <<  nb << " " << nc << " " << nd << " " << netlist[num_elementos].valor << endl;
      netlist[num_elementos].a=numero(lista, na);
      netlist[num_elementos].b=numero(lista, nb);
      netlist[num_elementos].c=numero(lista, nc);
      netlist[num_elementos].d=numero(lista, nd);
    }
    else if (tipo=='O') {
			input >> netlist[num_elementos].nome >> na >> nb >> nc >> nd;
      cout << netlist[num_elementos].nome << " " << na << " " <<  nb << " " << nc << " " << nd << endl;
      netlist[num_elementos].a=numero(lista, na);
      netlist[num_elementos].b=numero(lista, nb);
      netlist[num_elementos].c=numero(lista, nc);
      netlist[num_elementos].d=numero(lista, nd);
    }
    else if (tipo=='*') { /* Comentario comeca com "*" */
      cout << "Comentario: " << linha << endl;
      num_elementos--;
    }
    else {
      cout << "Elemento desconhecido: " << linha << endl;
      cin.get();
      exit(ERRO_ELEMENTO_DESCONHECIDO);
    }
  }
}

void adicionarVariaveis(std::vector<std::string>& lista, std::vector<Elemento>& netlist){
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

void listarVariaveis(std::vector<std::string> lista){
	std::cout << "Variaveis internas:  " << std::endl;
  for (int i=0; i<=num_variaveis; i++)
		std::cout << i << " -> " << lista[i] << std::endl;
}

void mostrarNetlist(std::vector<Elemento> netlist){
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

void montarSistema(std::vector<Elemento>& netlist, std::vector< std::vector<double>>& Yn){
	int i, j;
	double g;
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
		std::cout <<  "Sistema apos a estampa de " << netlist[i].nome << std::endl;
    for (int k=1; k<=num_variaveis; k++) {
      for (j=1; j<=num_variaveis+1; j++)
        if (Yn[k][j]!=0) printf("%+3.1f ",Yn[k][j]);
        else std::cout << " ... ";
			std::cout << std::endl;
    }
		std::cin.get();
	#endif
  }
}

int main(int argc, char** argv)
{
	using namespace std;

	vector<string> lista; 
	vector<Elemento> netlist(1);
  //Yn[MAX_NOS+1][MAX_NOS+2];
  
	cout << "Programa demonstrativo de analise nodal modificada" << endl;
  cout << "Por:\n Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br\nCamyla Tsukuda Romao - \nPaulo Oliveira Lenzi Valente - paulovalente@poli.ufrj.br" << endl;
  cout << "Versao " << versao << endl;
  /* Leitura do netlist */
	leituraNetlist(lista, netlist, argc, argv);

  /* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
	adicionarVariaveis(lista, netlist);
	cin.get();

  /* Lista tudo */
	listarVariaveis(lista);
	cin.get();

	mostrarNetlist(netlist);
	cin.get();
  
	/* Monta o sistema nodal modificado */
  cout << "O circuito tem " << num_nos << " nos, " << num_variaveis << " variaveis e " << num_elementos << " elementos" << endl;
	cin.get();
	
	vector<vector<double>> Yn(num_variaveis+1, vector<double>(num_variaveis+2));
	montarSistema(netlist, Yn);
	cin.get();

  /* Resolve o sistema */
  if (resolversistema(Yn)) {
		cin.get();
    exit(ERRO_RESOLUCAO_SISTEMA);
  }
#ifdef DEBUG
  /* Opcional: Mostra o sistema resolvido */
  cout << "Sistema resolvido:" << endl;
  for (int i=1; i<=num_variaveis; i++) {
      for (int j=1; j<=num_variaveis+1; j++)
        if (Yn[i][j]!=0) printf("%+3.1f ",Yn[i][j]);
        else printf(" ... ");
      cout << endl;
    }
	cin.get();
#endif
  /* Mostra solucao */
  cout << "Solucao:" << endl;
	string txt = "Tensao";
  for (int i=1; i<=num_variaveis; i++) {
    if (i==num_nos+1) txt = "Corrente";
    cout << txt << " " << lista[i] << ": " << Yn[i][num_variaveis+1] << endl;
  }
	cin.get();
  return OK;
}

