Falta implementar:
Simulacao no tempo pelo metodo dos trapezios

OK - C - Capacitor
		 G = 2C/deltaT
		 V = V(t0) + deltat/2C * j(t0)
OK - L - Indutor
		 G = deltaT/2L
		 V = -2L/delta * j(t0) - V(t0) 

K - Transformador Ideal

D - Diodo ideal
$ - Chave ideal
V - Fonte de tensao variante no tempo
I - Fonte de corrente variante no tempo

Para cada elemento adicionado:
	Mudar funcao de leitura do netlist
	Colocar a estampa em:
		- adicionarEstampasComponentesVariantes
		- resolverPontoOperacao
