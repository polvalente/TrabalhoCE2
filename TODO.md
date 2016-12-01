_OK_ - C - Capacitor
		 G = 2C/deltaT
		 V = V(t0) + deltat/2C * j(t0)
	
_OK_ L - Indutor
		 G = deltaT/2L
		 V = -2L/delta * j(t0) - V(t0) 

_OK_ K - Transformador Ideal

D - Diodo ideal
$ - Chave ideal

_OK_ V - Fonte de tensao variante no tempo
_OK_ I - Fonte de corrente variante no tempo

Para cada elemento adicionado:
	Mudar funcao de leitura do netlist
	Colocar a estampa em:
		- adicionarEstampasComponentesVariantes
		- resolverPontoOperacao

_FONTES VARIANTES NO TEMPO_: (falta lidar com o numero de ciclos)
_LIDAR COM NUMERO DE PASSOS POR PONTO NA HORA DE RETORNAR O RESULTADO FINAL_

