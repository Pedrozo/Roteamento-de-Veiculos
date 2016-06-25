#include <stdlib.h>
#include <math.h>
#include "prv.h"

// Atalho para criar ponto mais facilmente
Ponto criarPonto(int x, int y, int demanda) {
	Ponto p;

	p.x = x;
	p.y = y;
	p.demanda = demanda;

	return p;
}

// Atalho para criar rota mais facilmente
Rota criarRota(int quant_max) {
	Rota r;

	r.caminho = (Ponto *) malloc(quant_max * sizeof(Ponto));
	r.quant = 0;

	return r;
}

// Utiliza a formula de distância entre 2 pontos para calcular
float distancia(Ponto a, Ponto b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Calcula o custo total de uma rota
float custo(Ponto deposito, Rota rotas[], int quantidade) {
	float custo = 0.0;

	int i, j;
	for(i = 0; i < quantidade; i++) {
		custo += distancia(deposito, rotas[i].caminho[0]); // Distância entre o depósito e o primeiro pontos

		// Distancias entre os pontos "vizinhos" da rota
		for(j = 0; j < rotas[i].quant - 1; j++)
			custo += distancia(rotas[i].caminho[j], rotas[i].caminho[j + 1]);

		// Distância entre o ultimo ponto e o depósito
		custo += distancia(rotas[i].caminho[rotas[i].quant - 1], deposito);
	}

	return custo;
}

// Soma todas as demandas dos pontos do problema
int demandaTotal(Ponto pontos[], int dimensao) {
	int demanda_total = 0;	

	int i;
	for(i = 0; i < dimensao; i++)
		demanda_total += pontos[i].demanda;
	
	return demanda_total;
}

// Calcula a quantidade mínima de rotas que o problema terá
int quantMinRotas(Instancia instancia) {
	return (demandaTotal(instancia.pontos, instancia.dimensao) - 1) / instancia.capacidade + 1;
}