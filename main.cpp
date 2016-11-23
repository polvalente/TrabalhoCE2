/* Trabalho de Circuitos Eletricos II
 * Camyla Tsukuda Romao - 
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
#include "constants.hpp"
#include "functions.hpp"
#include "classes.hpp"

int main(int argc, char** argv)
{
	using namespace std;
	int num_nos, num_variaveis, num_elementos;
	string metodo = "DC";
	double passo = 0.1;
	double tempo_final = 1.0;
	double passos_por_ponto = 1e3;


	vector<string> lista; 
	vector<Elemento> netlist(1);
	vector<Elemento> componentesVariantes;
	vector<Elemento> amp_ops;
  
	cout << "Simulador construido para o trabalho de Circuitos Eletricos II" << endl;
  cout << "Camyla Tsukuda Romao - camyla.romao@poli.ufrj.br\nPaulo Oliveira Lenzi Valente - paulovalente@poli.ufrj.br" << endl;
	cout << "Baseado no programa MNA1 - por Antonio Carlos M. de Queiroz - acmq@coe.ufrj.br"<< endl;
  cout << "Versao " << versao << endl;

  /* Leitura do netlist */
	leituraNetlist(lista, netlist, componentesVariantes, argc, argv, num_elementos, num_variaveis, tempo_final, passo, metodo, passos_por_ponto, amp_ops);
	if (componentesVariantes.size() > 0) metodo = "TRAP";

  /* Acrescenta variaveis de corrente acima dos nos, anotando no netlist */
	adicionarVariaveis(lista, netlist, num_variaveis, num_nos, num_elementos);
	cin.get();

  /* Lista tudo */
	listarVariaveis(lista, num_variaveis);
	cin.get();

	mostrarNetlist(netlist, num_elementos);
	cin.get();
  
	/* Monta o sistema nodal modificado */
  cout << "O circuito tem " << num_nos << " nos, " << num_variaveis << " variaveis e " << num_elementos << " elementos" << endl;
	cin.get();
	
	vector<vector<long double>> Yn(num_variaveis+1, vector<long double>(num_variaveis+2));

	if (metodo == "TRAP"){
		if (int erro = simulacaoTrapezios(netlist, componentesVariantes, lista, num_elementos, num_nos, num_variaveis, passo, tempo_final, passos_por_ponto, amp_ops)){
			exit(erro);
		}
	}
	else{ /* DC */
		montarSistemaDC(netlist, Yn, num_variaveis, num_elementos);
		/* Resolve o sistema */
		if (resolverSistema(Yn, num_variaveis)) {
			cin.get();
			exit(ERRO_RESOLUCAO_SISTEMA);
		}
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
  return OK;
}
