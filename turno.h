#ifndef TURNO_H
#define TURNO_H

#include "mapa.h"
#include "estrategia.h"

int simular_turno(Mapa *mapa, Nivel *nivel, Coordenada*, int);

int buscarEnemigo(Coordenada*, int, Coordenada);

void inicializar_turno(Nivel *nivel, Mapa *mapa, DisposicionTorres estrategia);

#endif