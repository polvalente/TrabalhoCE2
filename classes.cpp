#include "classes.hpp"
#include <cmath>

#define PI 3.14159265358979323846

Elemento::Elemento(void){
}

double Elemento::valorFonteSenoidal(double t){
	if (t < atraso){
		return nivelDC + amplitude * std::sin(PI/180*defasagem);
	}
	else
		return nivelDC + amplitude * std::exp(-amortecimento*(t-atraso)) * std::sin(2*PI*frequencia * (t-atraso) + PI/180*defasagem);
}

double Elemento::valorFontePulse(double t, double passo){
	if (t < atraso)
		return amplitude;
	double tempo = fmod(t-atraso, periodo);
	tempoDescida = (tempoDescida == 0) ? passo : tempoDescida;
  tempoSubida = (tempoSubida == 0) ? passo : tempoSubida;

	if (tempo < tempoSubida){
		double a = (amplitude2 - amplitude)/tempoSubida;
		double b = amplitude;

		return a * tempo + b;
	}

	if (tempo < tempoSubida + tempoLigada){
		return amplitude2;
	}

	if (tempo < tempoSubida + tempoLigada + tempoDescida){
		double a = (amplitude - amplitude2)/tempoDescida;
		double b = amplitude2 - a * (tempoSubida + tempoLigada);
		return a * tempo + b;
	}

	return amplitude;

	/*
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
	}*/
}
