#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "util.h"
#include "prv.h"
#include "prvio.h"
#include "hill.h"

void nSolucoes(Instancia instancia, int quant_min_rotas);
void solucoesPeriodo(Instancia instancia, int quant_min_rotas);

int main() {
	system("cls");
	printf("Roteamento de Veiculos, Otimizacao por Hill Climbing\n");
	printf("----------------------------------------------------\n");
	printf("Arquivo da instancia: ");

	char nome_arquivo[256];

	scanf("%[^\n]", nome_arquivo);

	Instancia instancia;
	FILE *arquivo = fopen(nome_arquivo, "r");

	if(arquivo == NULL) {
		printf("Caminho para arquivo invalido!\n");
		return 1;
	}

	// Preenche os dados da instância pelo arquivo
	instancia.dimensao = lerDimensao(arquivo);
	instancia.capacidade = lerCapacidade(arquivo);
	Ponto pontos[instancia.dimensao];
	instancia.pontos = pontos;
	lerCoordenadas(arquivo, instancia.pontos, instancia.dimensao);
	lerDemanda(arquivo, instancia.pontos, instancia.dimensao);

	fclose(arquivo);

	int i;

	// Preenche os id's dos pontos
	for(i = 0; i < instancia.dimensao; i++)
		instancia.pontos[i].id = i + 1;

	// Calcula a quantidade minima de rotas
	int demanda_total = demandaTotal(instancia.pontos, instancia.dimensao);
	int quant_min_rotas = (demanda_total - 1) / instancia.capacidade + 1;

	srand(time(NULL));

	system("cls");
	printf("O que deseja fazer ?\n");
	printf("--------------------\n");
	printf("[1] Gerar N solucoes\n");
	printf("[2] Gerar por um determindado periodo\n\n");
	printf("> ");

	int op;
	scanf("%d", &op);


	switch(op) {

		case 1:
			nSolucoes(instancia, quant_min_rotas);
			break;

		case 2:
			solucoesPeriodo(instancia, quant_min_rotas);
			break;

		default:
			printf("Opcao invalida!\n");
	}

	return 0;
}

// Função que vai executar o algoritmo N vezes
void nSolucoes(Instancia instancia, int quant_min_rotas) {
	system("cls");
	printf("Quantas solucoes deseja gerar ?\n");
	printf("-------------------------------\n");
	printf("> ");

	int quantidade;
	scanf("%d", &quantidade);

	if(quantidade <= 0) {
		printf("Quantidade invalida!\n");
		return;
	}

	// Será utilizado uma matriz de rotas, pois é necessário armazenar cada uma das soluções para calcular a média, mediana e desvio padrão
	Solucao solucoes[quantidade];

	int rep;
	for(rep = 0; rep < quantidade; rep++) {

		// Aloca espaço para as rotas e gera as rotas aleatórias
		Rota *rotas = criarRotas(instancia);
		rotasAleatorias(rotas, instancia);

		printf("%d:\n", rep + 1);

		// Exibe o custo da solução gerada aleatoriamente
		printf("Aleatorio (custo): %f\n", custo(instancia.pontos[0], rotas, quant_min_rotas));

		hillClimbing(rotas, instancia);

		solucoes[rep].custo = custo(instancia.pontos[0], rotas, quant_min_rotas);

		printf("Otimizado (custo): %f\n\n", solucoes[rep].custo);

		char caminho[256];
		caminho[0] = '\0';
		strcat(caminho, "solucoes/solucao");

		char numero[10];
		toString(rep + 1, numero);
		strcat(caminho, numero);
		strcat(caminho, ".txt");

		FILE *arq = fopen(caminho, "w");
		escreverSolucao(arq, rotas, quant_min_rotas, instancia.pontos[0]);
		fclose(arq);

		// Armazena a solução encontrada
		solucoes[rep].rotas = rotas;
	}

	// long double por que em uma grande quantidade de teste, os valores podem ficar um tanto quanto grandes...
	long double media = 0;
	int indice_melhor = 0, indice_pior = 0; // Os indices no vetor de soluções da melhor e da pior rota

	// Soma todos os custos para calcular a média e também procura pelas melhores e piores soluções
	int i;
	for(i = 0; i < rep; i++) {

		if(solucoes[i].custo < solucoes[indice_melhor].custo)
			indice_melhor = i;
		else if(solucoes[i].custo > solucoes[indice_pior].custo)
			indice_pior = i;

		media += solucoes[i].custo;
	}

	media /= rep;


	// Calcula a variância das rotas 
	long double variancia = 0;
	for(i = 0; i < rep; i++) {
		variancia += (solucoes[i].custo - media)*(solucoes[i].custo - media);
	}

	variancia /= rep;

	// Arquivo onde será armazenado as estatísticas do problema
	FILE *estat = fopen("solucoes/estatisticas.txt", "w");

	fprintf(estat, "Média: %.2f\n", (float) media);
	fprintf(estat, "Desvio Padrão: %.2f\n", sqrt(variancia));
	fprintf(estat, "Melhor Solução: solucao%d (%.2f)\n", indice_melhor + 1, solucoes[indice_melhor].custo);
	fprintf(estat, "Pior Solução: solucao%d (%.2f)\n", indice_pior + 1, solucoes[indice_pior].custo);
}

// Função que vai executar o algoritmo durante um periodo de tempo
void solucoesPeriodo(Instancia instancia, int quant_min_rotas) {
	system("cls");
	printf("Quantos solucoes no ranking ?\n");
	printf("-----------------------------\n");
	printf("> ");

	int size;
	scanf("%d", &size);

	if(size <= 0) {
		printf("Quantidade invalida!\n");
		return;
	}

	Solucao solucoes[size];

	int i, j;

	// Gera as primeiras soluções
	for(i = 0; i < size; i++) {
		solucoes[i].rotas = criarRotas(instancia);

		rotasAleatorias(solucoes[i].rotas, instancia);
		
		hillClimbing(solucoes[i].rotas, instancia);

		solucoes[i].custo = custo(instancia.pontos[0], solucoes[i].rotas, quant_min_rotas);
	}

	// Ordena elas através do custo
	for(i = 0; i < size - 1; i++) {
		for(j = i + 1; j < size; j++) {
			if(solucoes[i].custo > solucoes[j].custo) {
				Solucao aux = solucoes[i];
				solucoes[i] = solucoes[j];
				solucoes[j] = aux;
			}
		}
	}

	system("cls");
	printf("Quanto tempo deseja executalo (segundos) ?\n");
	printf("-----------------------------------------\n");
	printf("> ");

	long long tempo, inicio;

	scanf("%lli", &tempo);

	if(tempo <= 0) {
		printf("Tempo invalido!\n");
		return;
	}

	// milisegundos para segundos
	tempo *= 1000;

	// Armazena o momento do inicio
	inicio = clock();

	Solucao atual;
	atual.rotas = criarRotas(instancia);

	unsigned int iteracoes = 0; // Quantidade de iterações no loop a seguir, também é utilizado para dizer quantas soluções diferentes foram geradas
	while(clock() - inicio < tempo) {

		for(i = 0; i < quant_min_rotas; i++)
			atual.rotas[i].quant = 0;

		rotasAleatorias(atual.rotas, instancia);
		hillClimbing(atual.rotas, instancia);

		// Armazena o valor do custo, portanto não será necessário calcular ele novamente
		atual.custo = custo(instancia.pontos[0], atual.rotas, quant_min_rotas);

		// Procura pela posição da solução gerada no ranking
		i = size;
		while(i > 0 && solucoes[i - 1].custo > atual.custo)
			i--;

		// se i < size, então a solução gerada deverá estar no rank!
		if(i < size) {
			int k,l;
			int j = size - 2;

			// Abre "espaço" para a nova solução gerada
			for(j = size - 2; j >= i; j--) {
				for(k = 0; k < quant_min_rotas; k++) {
					for(l = 0; l < solucoes[j].rotas[k].quant; l++)
						solucoes[j + 1].rotas[k].caminho[l] = solucoes[j].rotas[k].caminho[l];
					
					solucoes[j + 1].rotas[k].quant = solucoes[j].rotas[k].quant;
				}

				solucoes[j + 1].custo = solucoes[j].custo;
			}


			// Adiciona a nova solução gerada em seu devido local
			for(k = 0; k < quant_min_rotas; k++) {
				for(l = 0; l < atual.rotas[k].quant; l++)
					solucoes[i].rotas[k].caminho[l] = atual.rotas[k].caminho[l];

				solucoes[i].rotas[k].quant = atual.rotas[k].quant;
			}

			solucoes[i].custo = atual.custo;

			system("cls");
			for(i = 0; i < size; i++)
				printf("%d: %.2f\n", i + 1, solucoes[i].custo);

			printf("\n");
		}
		
		// Calcula o tempo restante
		long long restante = tempo - clock() + inicio;

		printf("\rFaltam");

		if(restante > 60000)
			printf(" %d minutos e", restante / 60000);

		printf(" %d segundos           ", (restante % 60000) / 1000);

		iteracoes++;
	}

	printf("\n");

	// Quantidade de iterações = quantidade de soluções aleatórias geradas
	printf("Total de solucoes geradas: %d\n", iteracoes);

	// Armazena as melhores soluções encontradas
	for(i = 0; i < size; i++) {

		char nome_arquivo[256];
		nome_arquivo[0] = '\0';
		strcat(nome_arquivo, "ranking/ranking");
		char numero[10];
		toString(i + 1, numero);
		strcat(nome_arquivo, numero);
		strcat(nome_arquivo, ".txt");


		FILE *arq = fopen(nome_arquivo, "w");
		escreverSolucao(arq, solucoes[i].rotas, quant_min_rotas, instancia.pontos[0]);
		fclose(arq);
	}
}