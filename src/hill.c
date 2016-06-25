#include "hill.h"
#include <stdlib.h>

Rota *criarRotas(Instancia instancia) {
	int quant_min_rotas = quantMinRotas(instancia);
	
	Rota *rotas = (Rota *) malloc(quant_min_rotas * sizeof(Rota));

	Ponto tmp[instancia.dimensao];

	int i, j;
	for(i = 0; i < instancia.dimensao; i++)
		tmp[i] = instancia.pontos[i];

	for(i = 0; i < instancia.dimensao - 1; i++) {
		for(j = i + 1; j < instancia.dimensao; j++) {
			if(tmp[i].demanda > tmp[j].demanda) {
				Ponto aux = tmp[i];
				tmp[i] = tmp[j];
				tmp[j] = aux;
			}
		}
	}

	int quant_max_pontos = -1;
	int carga_util = 0;

	i = 1;
	while(i < instancia.dimensao && carga_util < instancia.capacidade) {
		quant_max_pontos++;
		carga_util += tmp[i].demanda;
		i++;
	}

	for(i = 0; i < quant_min_rotas; i++)
		rotas[i] = criarRota(quant_max_pontos);

	return rotas;
}

void rotasAleatorias(Rota rotas[], Instancia instancia) {

	int quant_min_rotas = quantMinRotas(instancia);

	int i, j;
	for(i = 1; i < instancia.dimensao; i++) {
		int r = rand() % (instancia.dimensao - 1) + 1;

		Ponto aux = instancia.pontos[i];
		instancia.pontos[i] = instancia.pontos[r];
		instancia.pontos[r] = aux;
	}

	for(i = 0; i < quant_min_rotas; i++)
		rotas[i].quant = 0;

	for(i = 1; i < instancia.dimensao; i++) {
		for(j = 0; j < quant_min_rotas; j++) {

			int demanda_rota = 0;

			int k;
			for(k = 0; k < rotas[j].quant; k++)
				demanda_rota += rotas[j].caminho[k].demanda;
	

			if(demanda_rota + instancia.pontos[i].demanda <= instancia.capacidade) {
				rotas[j].caminho[rotas[j].quant] = instancia.pontos[i];
				rotas[j].quant++;
				j = quant_min_rotas;
			}
		}
	}


	int total_pontos = 0;

	for(i = 0; i < quant_min_rotas; i++)
		total_pontos += rotas[i].quant;

	if(total_pontos != instancia.dimensao - 1)
		rotasAleatorias(rotas, instancia);
}

void hillClimbing(Rota rotas[], Instancia instancia) {
	int quant_min_rotas = quantMinRotas(instancia);

	float custo_inicial;
	do {
		custo_inicial = custo(instancia.pontos[0], rotas, quant_min_rotas);

		int i, j, k, l;
		for(i = 0; i < quant_min_rotas; i++) {
			for(j = 0; j < rotas[i].quant; j++) {

				for(k = 0; k < quant_min_rotas; k++) {
					for(l = 0; l < rotas[k].quant; l++) {

						float custo_anterior = custo(instancia.pontos[0], rotas, quant_min_rotas);

						Ponto aux = rotas[i].caminho[j];
						rotas[i].caminho[j] = rotas[k].caminho[l];
						rotas[k].caminho[l] = aux;

						int excedeu = 0;

						int m, deman = 0;
						for(m = 0; m < rotas[i].quant; m++)
							deman += rotas[i].caminho[m].demanda;

						if(deman > instancia.capacidade)
							excedeu = 1;

						deman = 0;
						for(m = 0; m < rotas[k].quant; m++)
							deman += rotas[k].caminho[m].demanda;

						if(deman > instancia.capacidade)
							excedeu = 1;

						if(excedeu || custo(instancia.pontos[0], rotas, quant_min_rotas) > custo_anterior) {
							Ponto aux = rotas[i].caminho[j];
							rotas[i].caminho[j] = rotas[k].caminho[l];
							rotas[k].caminho[l] = aux;
						}
					}
				}
			}
		}
	} while(custo_inicial != custo(instancia.pontos[0], rotas, quant_min_rotas));
}