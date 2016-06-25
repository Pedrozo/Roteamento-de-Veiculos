#ifndef HILL_H
#define HILL_H

#include "prv.h"

Rota *criarRotas(Instancia instancia);
void rotasAleatorias(Rota *rotas, Instancia instancia);
void hillClimbing(Rota rotas[], Instancia instancia);
void hillClimbing2(Rota rotas[], Instancia instancia);

#endif