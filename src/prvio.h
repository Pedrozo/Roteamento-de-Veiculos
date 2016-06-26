#ifndef PRVIO_H
#define PRVIO_H

#include <stdio.h>
#include "util.h"
#include "prv.h"

int lerLinha(FILE *arq, char *linha);
int lerDimensao(FILE *arquivo);
int lerCapacidade(FILE *arquivo);
void lerCoordenadas(FILE *arquivo, Ponto pontos[], int dimensao);
void lerDemanda(FILE *arquivo, Ponto pontos[], int dimensao);
void escreverSolucao(FILE *arquivo, Rota rotas[], int quantidade, Ponto deposito);
void escreverSolucaoDetalhada(FILE *arquivo, Rota rotas[], int quantidade, Ponto deposito);

#endif