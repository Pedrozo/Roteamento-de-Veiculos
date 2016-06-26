#include "hill.h"
#include <stdlib.h>

/*
 * Retorna um vetor de rotas, com os vetores internos de pontos devidamente alocados
 * Necessita somente do instância que possui os pontos que as rotas vão percorrer
*/
Rota *criarRotas(Instancia instancia) {
	
	// O tamanho do vetor que será retornado sera o menor possível, de acordo com as regras para resolver o problema
	int quant_min_rotas = quantMinRotas(instancia);
	
	Rota *rotas = (Rota *) malloc(quant_min_rotas * sizeof(Rota));

	// Será utilizado para calcular a quantidade máxima de pontos que cada pode ter
	Ponto tmp[instancia.dimensao];

	int i, j;
	// copia os pontos para o vetor temporário
	for(i = 0; i < instancia.dimensao; i++)
		tmp[i] = instancia.pontos[i];

	// Ordena os pontos pela sua demanda
	for(i = 0; i < instancia.dimensao - 1; i++) {
		for(j = i + 1; j < instancia.dimensao; j++) {
			if(tmp[i].demanda > tmp[j].demanda) {
				Ponto aux = tmp[i];
				tmp[i] = tmp[j];
				tmp[j] = aux;
			}
		}
	}

	int quant_max_pontos = -1; // quantidade máxima de pontos, de forma a não disperdisar memória
	int carga_util = 0;

	i = 1; // i = 1, pois o primeiro do vetor (o depósito) possui demanda igual a 0
	while(i < instancia.dimensao && carga_util < instancia.capacidade) {
		quant_max_pontos++;
		carga_util += tmp[i].demanda;
		i++;
	}

	for(i = 0; i < quant_min_rotas; i++)
		rotas[i] = criarRota(quant_max_pontos);

	return rotas;
}

/*
 * Cria um conjunto de rotas aleatórias para resolver o problema
 * As rotas aleatórias serão armazenadas no vetor "rotas" 
*/
void rotasAleatorias(Rota rotas[], Instancia instancia) {

	int quant_min_rotas = quantMinRotas(instancia);

	int i, j;
	// Embaralha os pontos
	for(i = 1; i < instancia.dimensao; i++) {
		int r = rand() % (instancia.dimensao - 1) + 1;

		Ponto aux = instancia.pontos[i];
		instancia.pontos[i] = instancia.pontos[r];
		instancia.pontos[r] = aux;
	}

	// Zera a quantidade de pontos de cada rota
	for(i = 0; i < quant_min_rotas; i++)
		rotas[i].quant = 0;

	// Distribui os pontos aleatórios nas rotas
	for(i = 1; i < instancia.dimensao; i++) {
		for(j = 0; j < quant_min_rotas; j++) {

			int demanda_rota = 0; // Para verificar se a rota excedeu a capacidade de cada veículo

			int k;
			for(k = 0; k < rotas[j].quant; k++)
				demanda_rota += rotas[j].caminho[k].demanda;
	
			// Verifica se é possível adicionar mais um ponto na rota
			if(demanda_rota + instancia.pontos[i].demanda <= instancia.capacidade) {
				rotas[j].caminho[rotas[j].quant] = instancia.pontos[i];
				rotas[j].quant++;
				j = quant_min_rotas; // Termina esse loop
			}
		}
	}

	// No fim, é necessário contar os pontos para verificar se não houve nenhum tipo de erro
	int total_pontos = 0;

	for(i = 0; i < quant_min_rotas; i++)
		total_pontos += rotas[i].quant;

	if(total_pontos != instancia.dimensao - 1)
		rotasAleatorias(rotas, instancia);
}

/*
 * Otimiza uma determinada rota, utilizando a técnica do algoritmo Hill Climbing
 * A rota otimizada será armazenada no vetor "rotas"
*/
void hillClimbing(Rota rotas[], Instancia instancia) {
	int quant_min_rotas = quantMinRotas(instancia);

	float custo_inicial;

	// O loop vai verificar se não é possível continuar a otimização, portanto foi encontrado um topo local
	do {
		// Antes de executar a otimização, ele salva o custo atual, para verificar no fim se ele foi diminuido (otimizado)
		custo_inicial = custo(instancia.pontos[0], rotas, quant_min_rotas);

		int i, j, k, l;
		for(i = 0; i < quant_min_rotas; i++) {
			for(j = 0; j < rotas[i].quant; j++) {

				for(k = 0; k < quant_min_rotas; k++) {
					for(l = 0; l < rotas[k].quant; l++) {

						// Antes de fazer uma alteração na rota, ele salva o custo
						float custo_anterior = custo(instancia.pontos[0], rotas, quant_min_rotas);

						Ponto aux = rotas[i].caminho[j];
						rotas[i].caminho[j] = rotas[k].caminho[l];
						rotas[k].caminho[l] = aux;

						int excedeu = 0; // Para verificar se a nova rota excede a capacidade de algum dos veículos

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

						/*
						 * Caso a nova solução tenha excedido a capacidade de algum dos veículos ou
						 * caso a nova rota seja válida mas possua um custo maior, a alteração 
						 * é desfeita
						*/
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