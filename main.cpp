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
#include <cstdlib>
#include "constants.hpp"
#include "functions.hpp"
#include "classes.hpp"

int main(int argc, char** argv)
{
	using namespace std;
	int num_nos, num_variaveis, num_elementos;
	string metodo = "DC";
	double passo = 0;
	double tempo_final = 0;
	unsigned passos_por_ponto = 1;


	vector<string> lista; 
	vector<Elemento> netlist(1);
	vector<Elemento> componentesVariantes;
	vector<Elemento> componentesNaoLineares;
	std::vector< std::vector<long double> > resultados;
  
	cout << "Simulador construido para o trabalho de Circuitos Eletricos II\n\n" << endl;
	cout << "Feito por: " << endl;
  cout << "Alice Fontes - alicefontes@poli.ufrj.br" << endl;
	cout << "Camyla Tsukuda Romao - camyla.romao@poli.ufrj.br" << endl;
	cout << "Paulo Oliveira Lenzi Valente - paulovalente@poli.ufrj.br" << endl << endl;
	cout << "Baseado no programa MNA1 - por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br\n"<< endl;
  cout << "Versao " << versao << endl << endl << endl;

  /* Leitura do netlist */
	string nomeArquivo = "";
	if (argc == 2)
		nomeArquivo = argv[1];
	leituraNetlist(lista, netlist, componentesVariantes, argc, nomeArquivo, num_elementos, num_variaveis, tempo_final, passo, metodo, passos_por_ponto, componentesNaoLineares);
	//if (componentesVariantes.size() > 0) metodo = "TRAP";

  /* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
	adicionarVariaveis(lista, netlist, num_variaveis, num_nos, num_elementos);
	//cin.get();

  
	
#ifdef DEBUG
	cout << "Metodo de simulacao: " << metodo << endl;
#endif

	if (metodo == "TRAP"){
		simulacaoTrapezios(netlist, componentesVariantes, lista, num_elementos, num_nos, num_variaveis, passo, tempo_final, passos_por_ponto, resultados, componentesNaoLineares);
		escreverResultadosNoArquivo(nomeArquivo, resultados, passo, tempo_final, passos_por_ponto, lista);
	}
	else{ /* DC */
		passo = 1;
		tempo_final = 0;
		passos_por_ponto = 1;
		simulacaoTrapezios(netlist, componentesVariantes, lista, num_elementos, num_nos, num_variaveis, passo, tempo_final, passos_por_ponto, resultados, componentesNaoLineares);
		escreverResultadosNoArquivo(nomeArquivo, resultados, passo, tempo_final, passos_por_ponto, lista);

		/* Lista tudo */
		/*adicionarVariaveisDinamicas(lista, componentesVariantes, componentesNaoLineares, num_variaveis, num_nos);
		vector< vector<long double> > Yn(num_variaveis+1, vector<long double>(num_variaveis+2));
		listarVariaveis(lista, num_variaveis);
		cin.get();

		mostrarNetlist(netlist, num_elementos);
		cin.get();
		* Monta o sistema nodal modificado *
		cout << "O circuito tem " << num_nos << " nos, " << num_variaveis << " variaveis e " << num_elementos << " elementos" << endl;
		cin.get();
		montarSistemaDC(netlist, Yn, num_variaveis, num_elementos);
		* Resolve o sistema *
		//if (resolverSistema(Yn, num_variaveis)) {
		bool convergiu;
		resolverNewtonRaphson(Yn, componentesNaoLineares, num_variaveis, convergiu);
		if(!convergiu){
			cin.get();
			exit(ERRO_CONVERGENCIA);
		}
		#ifdef DEBUG
			*Opcional: Mostra o sistemar resolvido*
			mostrarSistema("Sistema resolvido: ", Yn, num_variaveis);
			cin.get();
		#endif
		* Mostra solucao *
		cout << "Solucao:" << endl;
		string txt = "Tensao";
		std::vector<long double> solucao(num_variaveis+1);
		solucao[0] = 0;
		for (int i=1; i<=num_variaveis; i++) {
			if (i==num_nos+1) txt = "Corrente";
			solucao[i] = Yn[i][num_variaveis+1];
			cout << txt << " " << lista[i] << ": " << solucao[i] << endl;
		}
		cin.get();
		std::vector<std::vector<long double> > solucoes;
		solucoes.push_back(solucao);
		escreverResultadosNoArquivo(nomeArquivo, solucoes, 0, 0, 1, lista);*/
	}
  return OK;
}
