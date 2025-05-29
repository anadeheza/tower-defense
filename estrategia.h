#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include "mapa.h"
#include "nivel.h"
#include "pila.h"


typedef struct estado {
    Mapa mapita;
    int torresColocadas;
    int fila;
    int columna;
} Estado;

typedef struct Torre_{
    Coordenada pos_torre;
    int ataque;
}Tower;

int comparar_torres(const void *a, const void *b);

int distancia_al_origen(const Tower *torre);

typedef void (*DisposicionTorres)(Nivel*, Mapa*);

void disponer(Nivel* nivel, Mapa* mapa);

void reiniciar_mapa(Mapa* mapa);

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa);

//Coordenada* posiciones_validas_backtracking(TipoCasilla **casillas, int alto, int ancho);

void disponer_custom(Nivel* nivel, Mapa* mapa);

#endif
