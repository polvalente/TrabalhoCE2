#ifndef _CLASSES_CE2_
#define _CLASSES_CE2_
#include <string>
#include <vector>
//Definicoes de classe
class Elemento{
  using std::string;
	public:
		string nome;
		double valor;
		int a,b,c,d,x,y;
        
    string tipoFonte;
    
    double atraso;
    double amplitude;
    double numeroCiclos;

    //Parametros da fonte senoidal
    double frequencia;
    double nivelDC;
    double amortecimento;
    double defasagem;

    //Parametros da fonte de pulso
    double amplitude2;
    double tempoSubida;
    double tempoDescida;
    double tempoLigada;
    double Periodo;
    
		Elemento();
    double valorFonteSenoidal(double);
    double valorFontePulse(double);
};
#endif
