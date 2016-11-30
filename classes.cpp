#include "classes.hpp"
#include <cmath>

Elemento::Elemento(void){
}

double Elemento::valorFonteSenoidal(double t){
	return nivelDC + amplitude * std::exp(-amortecimento*t) * std::sin(2*M_PI*frequencia * t + M_PI/180*defasagem);
}

double Elemento::valorFontePulse(double t){
	double tempo = fmod(t,periodo);
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
