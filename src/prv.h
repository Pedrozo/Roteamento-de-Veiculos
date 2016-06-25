#ifndef PRV_H
#define PRV_H

typedef struct {
	int x;
	int y;
	int demanda;
	int id;
} Ponto;

typedef struct {
	Ponto *caminho; // Vetor com os pontos da rota, visitados na ordem do vetor
	int quant;      // Quantidade de elementos no vetor
} Rota;

typedef struct {
	Ponto *pontos;  // Vetor com os pontos da instancia
	int dimensao;   // Quantidade de pontos da instancia
	int capacidade; // Capacidade de cada veiculo da instância
} Instancia;

typedef struct {
	Rota *rotas;
	float custo;
} Solucao;

Ponto criarPonto(int x, int y, int demanda);
Rota criarRota(int quant_max);
float distancia(Ponto a, Ponto b);
float custo(Ponto deposito, Rota rotas[], int quantidade);
int demandaTotal(Ponto pontos[], int dimensao);
int quantMinRotas(Instancia instancia);

#endif