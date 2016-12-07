#include "classes.hpp"
#include <cmath>


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

	//fonte deve ser desligada, parando aonde estava
	double tempo = fmod((t-atraso),periodo);
	if (t >= (atraso + numeroCiclos * periodo)){
		tempo = atraso + numeroCiclos*periodo;
	}
	if (tempoDescida == 0){
		tempoDescida = passo;
	}
	if (tempoSubida == 0){
		tempoSubida = passo;
	}
	//fonte ainda nao ligou
	if (tempo < atraso){
		return amplitude;
	}else if ((tempo >= atraso) && (tempo < (atraso + tempoSubida))){
		double a = (amplitude2 - amplitude)/tempoSubida;
		double b = amplitude - atraso*a;
		return tempo*a + b;
	}
	else if ((tempo >= (atraso + tempoSubida)) && (tempo < (periodo - tempoDescida))){
	//constante em amp2
		return amplitude2;
	}
	
	//decaindo de amp2 a amp1
	return amplitude2 + (amplitude - amplitude2)/tempoDescida * (tempo - (atraso + tempoSubida+ tempoLigada));
}
