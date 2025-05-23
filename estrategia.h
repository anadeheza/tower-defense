#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include "mapa.h"
#include "nivel.h"

typedef struct Stack_{
    Coordenada *torres;
    int ultimo;
}Pila;

typedef struct Queque_{
    Coordenada *vacios;
    int ultimo, primero;
}Cola;

/*
typedef struct Caso_{
    int hay_bifurcacion;    //0 si no hay bifurcaciones, 1 si las hay
    Coordenada pos_bifurcacion;     //irrelevante si hay_bifurcacion = 0
}CasoMapa;
*/

typedef struct Torre_{
    Coordenada pos_torre;
    int ataque;
}Tower;

int comparar_torres(Tower *a, Tower *b);

int distancia_al_origen(Tower *torre);

typedef void (*DisposicionTorres)(Nivel*, Mapa*);

void disponer(Nivel* nivel, Mapa* mapa);

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa);

Coordenada* posiciones_validas_backtracking(TipoCasilla **casillas, int alto, int ancho);

//static int determinar_posicion_torre_backtracking(int *casilla_elegida, Coordenada *validas);

void disponer_custom(Nivel* nivel, Mapa* mapa);

void pila_desapilar(Pila *pilita);

#endif
