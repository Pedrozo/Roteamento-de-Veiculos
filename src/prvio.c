#include "prv.h"
#include "prvio.h"

/*
  Retorna:
  0 -> quando não houver mais linhas para serem lidas
  1 -> quando há pelo menos uma linha para ser lida
*/
int lerLinha(FILE *arquivo, char *linha) {

	int entrada = fgetc(arquivo);

	if(entrada == -1)
		return 0;

	while(entrada != -1 && entrada != (int) '\n') {
		*linha = (char) entrada;
		*linha++;

		entrada = fgetc(arquivo);
	}

	*linha = '\0';

	return 1;
}

/*
 * Retorna a dimensão que foi lida no arquivo, ou seja, a quantidade total de pontos
 * Caso o arquivo seja inválido retorna 0
*/
int lerDimensao(FILE *arquivo) {

	int dimensao = -1;

	char linha[256];

	while(dimensao == -1 && lerLinha(arquivo, linha)) {
		if(strncmp(linha, "DIMENSION", 9) == 0) {

			char *valor = linha;

			// Adiciona os valores numéricos seguidos
			while(*valor < '0' || *valor > '9')
				*valor++;

			dimensao = toInt(valor);
		}
	}

	if(dimensao != -1)
		return dimensao;
	else
		return 0; // diz que o arquivo é invalido, dimensão deve ser sempre positiva
}

/*
 * Retorna a capacidade de cada veículo, que foi lida no arquivo da instância
 * Na possibilidade do arquivo ser inválido, retorna 0
*/
int lerCapacidade(FILE *arquivo) {

	int capacidade = -1;

	char linha[256];

	while(capacidade == -1 && lerLinha(arquivo, linha)) {
		if(strncmp(linha, "CAPACITY", 8) == 0) {

			char *valor = linha;

			while(*valor < '0' || *valor > '9')
				*valor++;

			capacidade = toInt(valor);
		}
	}

	if(capacidade != -1)
		return capacidade;
	else
		return 0;
}

/*
 * Preenche as coordenadas dos pontos de acordo com o arquivo
*/
void lerCoordenadas(FILE *arquivo, Ponto pontos[], int dimensao) {

	char linha[256];

	while(strncmp(linha, "NODE_COORD_SECTION", 18))
		lerLinha(arquivo, linha);

	int i;
	for(i = 0; i < dimensao; i++) {
		lerLinha(arquivo, linha);

		char *coord_x;
		char *coord_y;

		char *str = linha;

		// Pula os espaços inicias
		while(*str == ' ')
			*str++;

		// Pula o valor que diz qual o indice da coordenada
		while(*str >= '0' && *str <= '9')
			*str++;

		// Pula os espaços após o indice das coordenadas
		while(*str == ' ')
			*str++;

		coord_x = str;

		while(*str >= '0' && *str <= '9')
			*str++;

		*str++ = '\0';

		while(*str == ' ')
			*str++;

		coord_y = str;

		while(*str >= '0' && *str <= '9')
			*str++;

		*str = '\0';

		pontos[i].x = toInt(coord_x);
		pontos[i].y = toInt(coord_y);
	}
}

/*
 * Preenche a demanda dos pontos de acordo com o arquivo
*/
void lerDemanda(FILE *arquivo, Ponto pontos[], int dimensao) {

	char linha[256];

	while(strncmp(linha, "DEMAND_SECTION", 14))
		lerLinha(arquivo, linha);

	int i;
	for(i = 0; i < dimensao; i++) {
		lerLinha(arquivo, linha);

		char *str = linha;

		while(*str == ' ')
			*str++;

		while(*str >= '0' && *str <= '9')
			*str++;

		while(*str == ' ')
			*str++;

		char *deman = str;

		while(*str >= '0' && *str <= '9')
			*str++;

		*str = '\0';

		pontos[i].demanda = toInt(deman);
	}
}

/*
 * Escreve a solução no arquivo que foi escolhido, no padrão do site http://neo.lcc.uma.es/vrp/vrp-instances/capacitated-vrp-instances/
*/
void escreverSolucao(FILE *arquivo, Rota rotas[], int quantidade, Ponto deposito) {

	int i, j;
	for(i = 0; i < quantidade; i++) {
		fprintf(arquivo, "Route #%d: ", i + 1);

		for(j = 0; j < rotas[i].quant; j++)
			fprintf(arquivo, "%d ", rotas[i].caminho[j].id - 1);
		
		fprintf(arquivo, "\n");
	}

	fprintf(arquivo, "Cost: %.2f", custo(deposito, rotas, quantidade));
}

void escreverSolucaoDetalhada(FILE *arquivo, Rota rotas[], int quantidade, Ponto deposito) {
	
	int i, j;
	for(i = 0; i < quantidade; i++) {
		fprintf(arquivo, "Rota #%d:\n", i + 1);

		fprintf(arquivo, "+----------------------------------+\n");
		fprintf(arquivo, "| ID | COORD_X | COORD_Y | DEMANDA |\n");
		fprintf(arquivo, "|----+---------+---------+---------|\n");
		fprintf(arquivo, "|  1 | %7d | %7d | %7d |\n", deposito.x, deposito.y, 0);

		float custo = distancia(deposito, rotas[i].caminho[0]);
		int carga_util = 0;

		for(j = 0; j < rotas[i].quant; j++) {
			fprintf(arquivo, "| %2d | %7d | %7d | %7d |\n", rotas[i].caminho[j].id, rotas[i].caminho[j].x, rotas[i].caminho[j].y, rotas[i].caminho[j].demanda);
			carga_util += rotas[i].caminho[j].demanda;

			if(j < rotas[i].quant - 1)
				custo += distancia(rotas[i].caminho[j], rotas[i].caminho[j + 1]);
			else
				custo += distancia(rotas[i].caminho[j], deposito);
		}

		fprintf(arquivo, "|  1 | %7d | %7d | %7d |\n", deposito.x, deposito.y, 0);
		fprintf(arquivo, "+----------------------------------+\n");
		fprintf(arquivo, "distância percorrida: %.2f\n", custo);
		fprintf(arquivo, "carga útil: %d\n\n", carga_util);
	}

	fprintf(arquivo, "Custo: %.2f\n", custo(deposito, rotas, quantidade));
}