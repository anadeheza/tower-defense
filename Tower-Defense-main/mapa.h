#ifndef MAPA_H
#define MAPA_H

#include "nivel.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#define limpiar "clear"
#define RED 196
#define GREEN 46
#define ANSI_COLOR_INDEX(r, g, b) (16 + 36 * (r) + 6 * (g) + (b))

typedef enum { CAMINO, TORRE, VACIO, BLOQUEADO, ENEMIGO } TipoCasilla;

typedef struct {
    int ancho, alto, cant_torres, distancia_ataque;
    TipoCasilla **casillas;
    Coordenada *torres;
} Mapa;

Mapa *inicializar_mapa(int, int, int, int);

void mostrar_mapa(Mapa *mapa, Enemigos *enemigos);

void liberar_mapa(Mapa *mapa);

void limpiar_pantalla();

#endif