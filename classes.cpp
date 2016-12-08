#include "classes.hpp"
#include "constants.hpp"
#include <cmath>


#ifdef DEBUG
#include <iostream>
#endif

Elemento::Elemento(void){
}

double Elemento::valorFonteSenoidal(double t){
	//passou do numero de ciclos e a fonte deve desligar
	if (t > (atraso + numeroCiclos/frequencia)){
		double tempo = numeroCiclos/frequencia + atraso;
		return	nivelDC + amplitude * std::exp(-amortecimento*(tempo-atraso)) * std::sin(2*pi*frequencia * (tempo-atraso) + pi/180*defasagem);
	}
	//fonte ainda nao deve ligar
	if (t < atraso){
		return nivelDC + amplitude * std::sin(pi/180*defasagem);
	}

	//fonte esta ligada
	return nivelDC + amplitude * std::exp(-amortecimento*(t-atraso)) * std::sin(2*pi*frequencia * (t-atraso) + pi/180*defasagem);
}

double Elemento::valorFontePulse(double t, double passo){
	//fonte desligada antes de t = atraso
	tempoSubida = (tempoSubida == 0) ? passo : tempoSubida;
	tempoDescida = (tempoDescida == 0) ? passo : tempoDescida;

	
	if (t > (atraso + periodo * numeroCiclos))
		t = atraso + periodo * numeroCiclos;
	double tempo = fmod(t-atraso,periodo);
	//fonte para onde estiver apos n ciclos
	
	if (t < atraso)
		return amplitude;

	//fonte subindo de tempo = atraso ate tempo = atraso + tempoSubida
	if (tempo < (tempoSubida)){
		double a = (amplitude2 - amplitude)/tempoSubida;
		double b = amplitude;
		return a * tempo + b;
	} 
	//fonte parada em amplitude2 de tempo = atraso + tempoSubida ate tempo = atraso + tempoSubida + tempo ligada
	else if (tempo < (tempoSubida + tempoLigada)){
		return amplitude2;
	}
	//fonte descendo de tempo = atraso + tempoSubida + tempoLigada ate tempo = atraso + tempoSubida + tempoLigada + tempoDescida
	else if (tempo < (tempoSubida + tempoLigada + tempoDescida)){
		double a = (amplitude - amplitude2)/tempoDescida;
		double b = amplitude2 - a*(tempoSubida + tempoLigada);
		return a*tempo + b;
	}
	//fonte desligada de tempo = atraso + tempoSubida + tempoLigada + tempoDescida ate tempo = periodo
	else if (tempo < (periodo)){
		return amplitude;
	}
	//caso de erro, retorna a amplitude da fonte desligada
	exit(ERRO_FONTE_PULSE);
	return 0;
}
