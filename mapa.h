#ifndef MAPA_H_INCLUDED
#define MAPA_H_INCLUDED

#include <allegro.h>
#include <stdio.h>
#include "estruturas.h"

int verificaPropriedade(Tile* square, int jogador);

void preencheMatriz(Jogo* jogo);

void preencheDC(Jogo* jogo);

Jogo* inicializaJogo();

char auxContorno (char c); //importante para tiles que se unem (ex: estrada e oceano)

void mudaConstrucao(Jogo* jogo,int i, int j, int player); //troca o dono da construção

void montaConstrucoes(Jogo* jogo); //monta as camadas das construções

BITMAP* retornaTile(int i, int j, Jogo* jogo, BITMAP* lands);

void geraTerreno(Jogo* jogo); //monta a parte fixa do mapa

#endif // MAPA_H_INCLUDED
