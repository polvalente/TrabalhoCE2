#include "classes.hpp"
#include <cmath>

#define PI 3.14159265358979323846

Elemento::Elemento(void){
}

double Elemento::valorFonteSenoidal(double t){
	// method for calculating sine wave value at instant t
	if (t > atraso + numeroCiclos/frequencia)
		// freeze the wave at the instant it should have been turned off
		t = atraso + numeroCiclos/frequencia;

	if (t < atraso){
		// wave hasn't been turned on yet
		return nivelDC + amplitude * std::sin(PI/180*defasagem);
	}
	else
		// wave is on
		return nivelDC + amplitude * std::exp(-amortecimento*(t-atraso)) * std::sin(2*PI*frequencia * (t-atraso) + PI/180*defasagem);
}

double Elemento::valorFontePulse(double t, double passo){
	//method for calculating pulse wave value at instant t
	if (t < atraso)
		// wave hasn't been turned on yet
		return amplitude;
	if (t > (atraso + periodo*numeroCiclos))
		// freeze the wave at the instant it should have been turned off
		t = atraso + periodo*numeroCiclos;
	double tempo = fmod(t-atraso, periodo);
	
	//if rise or fall times are 0, use the time step instead
	tempoDescida = (tempoDescida == 0) ? passo : tempoDescida;
  tempoSubida = (tempoSubida == 0) ? passo : tempoSubida;

	if (tempo < tempoSubida){
		// wave is rising
		double a = (amplitude2 - amplitude)/tempoSubida;
		double b = amplitude;

		return a * tempo + b;
	}

	if (tempo < tempoSubida + tempoLigada){
		// wave is turned on
		return amplitude2;
	}

	if (tempo < tempoSubida + tempoLigada + tempoDescida){
		//wave is falling
		double a = (amplitude - amplitude2)/tempoDescida;
		double b = amplitude2 - a * (tempoSubida + tempoLigada);
		return a * tempo + b;
	}

	// wave is off
	return amplitude;
}
