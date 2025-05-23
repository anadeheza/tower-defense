#ifndef NIVEL_H
#define NIVEL_H

#include "stdlib.h"
#include "stdio.h"

typedef struct {
    int x, y;
} Coordenada;

typedef struct {
    Coordenada *posiciones;
    int *vida, *activos;
    int cantidad, cantidad_activos, vida_inicial;
} Enemigos;

typedef struct {
    Coordenada *posiciones;
    int largo_camino;
} Camino;

typedef struct {
    Enemigos *enemigos;
    Camino *camino;
} Nivel;

Nivel *inicializar_nivel(int largo_camino, int cant_enemigos, int vida_inicial);

void mostrar_nivel(Nivel *info);

void mostrar_enemigos(Enemigos *enemigos);

void liberar_nivel(Nivel*);

#endif