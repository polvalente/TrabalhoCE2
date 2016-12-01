#include "classes.hpp"
#include <cmath>

Elemento::Elemento(void){
}

double Elemento::valorFonteSenoidal(double t){
	if (t < atraso){
		return nivelDC + amplitude * std::sin(M_PI/180*defasagem);
	}
	else
		return nivelDC + amplitude * std::exp(-amortecimento*(t-atraso)) * std::sin(2*M_PI*frequencia * (t-atraso) + M_PI/180*defasagem);
}

double Elemento::valorFontePulse(double t, double passo){
	double tempo = fmod(t,periodo);
	if (tempoDescida == 0){
		tempoDescida = passo;
	}
	if (tempoSubida == 0){
		tempoSubida = passo;
	}
	if (tempo < atraso){
		return amplitude;
	}
	else if ((tempo >= atraso) && (tempo < (atraso + tempoSubida))){
		double a = (amplitude2 - amplitude)/tempoSubida;
		double b = amplitude - atraso*a;
		return tempo*a + b;
	}
	else if ((tempo >= (atraso + tempoSubida)) && (tempo < (periodo - tempoDescida))){
	//constante em amp2
		return amplitude2;
	}
	else{
	//decaindo de amp2 a amp1
		return amplitude2 + (amplitude - amplitude2)/tempoDescida * (tempo - (atraso + tempoSubida+ tempoLigada));
	}
}
