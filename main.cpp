/* Electrical Circuits Simulator - work for class EEL525 at DEL/UFRJ
 *
 * Based on MNA1 by Antonio Carlos Moreirao de Queiroz - acmq@coe.ufrj.br
 * Alice Fontes - alicefontes@poli.ufrj.br
 * Camyla Tsukuda Romao - camyla.romao@poli.ufrj.br
 * Paulo Oliveira Lenzi Valente - paulovalente@poli.ufrj.br
 *
 * The following components can be in the netlist:
 *
 * Comment     - *<comment>
 *
 * Resistor    -  R<name> <node 1> <node 2> <value>
 * Capacitor   -  C<name> <node 1> <node 2> <value>
 * Inductor    -  L<name> <node 1> <node 2> <value>
 *
 * Transformer -  K<name> <node 1+> <node 1-> <node 2+> <node 2-> <n>
 *
 * Op.Amp.     -  O<name> <node out+> <node out-> <node in+> <node in->
 *
 * V.C.V.S.    -  E<name> <node out+> <node out-> <node in+> <node in-> <Gain> 
 * C.C.C.S.    -  F<name> <node out+> <node out-> <node in+> <node in-> <Gain>
 * V.C.C.S.    -  G<name> <node out+> <node out-> <node in+> <node in-> <Gain> 
 * C.C.V.S.    -  H<name> <node out+> <node out-> <node in+> <node in-> <Gain>
 *
 * Diode       -  D<name> <node +> <node ->
 * Switch      -  $<name> <node +> <node -> <control node +> <control node -> <limit voltage>
 * */

#include <iostream>
#include "constants.hpp"
#include "functions.hpp"
#include "classes.hpp"

int main(int argc, char** argv)
{
	using namespace std;
	int num_nos, num_variaveis;
	string metodo = "DC";
	double passo = 0.1;
	double tempo_final = 1.0;
	unsigned passos_por_ponto = 1;


	vector<string> lista; 
	vector<Elemento> netlist(1);
	vector<Elemento> componentesVariantes;
	vector<Elemento> componentesNaoLineares;
	std::vector<std::vector<long double>> resultados;
  
	cout << "Simulador construido para o trabalho de Circuitos Eletricos II\n\n" << endl;
	cout << "Feito por: " << endl;
  cout << "Alice Fontes - alicefontes@poli.ufrj.br" << endl;
	cout << "Camyla Tsukuda Romao - camyla.romao@poli.ufrj.br" << endl;
	cout << "Paulo Oliveira Lenzi Valente - paulovalente@poli.ufrj.br\n" << endl;
	cout << "Baseado no programa MNA1 - por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br\n"<< endl;
  cout << "Versao " << versao << "\n\n" << endl;

  /* Reading the netlist - can take optional command line argument as filename*/
	string nomeArquivo = "";
	if (argc == 2)
		nomeArquivo = argv[1];
	leituraNetlist(lista, netlist, componentesVariantes, argc, nomeArquivo, num_variaveis, tempo_final, passo, metodo, passos_por_ponto, componentesNaoLineares);
	if (componentesVariantes.size() > 0) metodo = "TRAP";

	/* Creating the variables for the currents that need to be calculated*/
	adicionarVariaveis(lista, netlist, num_variaveis, num_nos);
	
#ifdef DEBUG
	cout << "Metodo de simulacao: " << metodo << endl;
#endif

	if (metodo == "TRAP"){
		simulacaoTrapezios(netlist, componentesVariantes, lista, num_nos, num_variaveis, passo, tempo_final, passos_por_ponto, resultados, componentesNaoLineares);
		escreverResultadosNoArquivo(nomeArquivo, resultados, passo, tempo_final, passos_por_ponto, lista);
	}
	else{ /* DC */
		/* Lista tudo */
		vector<vector<long double>> Yn(num_variaveis+1, vector<long double>(num_variaveis+2));
		
		listarVariaveis(lista, num_variaveis);
		cin.get();

		mostrarNetlist(netlist);
		cin.get();
		/* Monta o sistema nodal modificado */
		montarSistemaDC(netlist, Yn, num_variaveis);
		/* Resolve o sistema */
		if (resolverSistema(Yn, num_variaveis)) {
			cin.get();
			exit(ERRO_RESOLUCAO_SISTEMA);
		}
		#ifdef DEBUG
			/*Opcional: Mostra o sistemar resolvido*/
			mostrarSistema("Sistema resolvido: ", Yn, num_variaveis);
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
	}
  return OK;
}
