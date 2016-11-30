#include "classes.hpp"
#include <cmath>

Elemento::Elemento(void){
}

Elemento::valorFonteSenoidal(double t){
	return valorDC + Amplitude * std::exp(-amortecimento*t) * std::sin(2*std::pi*frequencia * t + M_PI/180*defasagem);
}

Elemento::valorFontePulse(double t){
	double tempo = t % periodo;
	if (tempo < atraso){
		return amplitude;
	}
	else if ((tempo > atraso) && (tempo < (atraso + tempoSubida)){
		double a = (amplitude2 - amplitude)/tempoSubida;
		double b = amplitude - atraso*a;
		return t*a + b;
	}
	else if (tempo > (atraso + tempoSubida)) && (tempo < (periodo - tempoDescida)){
	//constante em amp2
		return amplitude2;
	}
	else{
	//decaindo de amp2 a amp1
		return (amplitude - amplitude2)/tempoDescida * (t - (atraso + tempoLigada));
	}
}
