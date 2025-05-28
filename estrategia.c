#include "estrategia.h"
#include "turno.h"
#include "simulador.h"
#include "pila.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static int posiciones_validas(Coordenada *validas, TipoCasilla **casillas, int alto, int ancho) {
    int cant_posiciones_validas = 0;
    
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            if (casillas[i][j] == VACIO) {
                validas[cant_posiciones_validas].x = i;
                validas[cant_posiciones_validas++].y = j;
            }
        }
    }

    return cant_posiciones_validas;
}

static void colocar_torre(Mapa *mapa, int x, int y, int nro_torre) {
    // actualizar torre
    mapa->torres[nro_torre].x = x;
    mapa->torres[nro_torre].y = y;

    // actualizar mapa
    mapa->casillas[x][y] = TORRE;
}

static int determinar_posicion_torre(int *casilla_elegida, int cant_validas) {
    int nueva_posicion = rand() % cant_validas;
    while(casilla_elegida[nueva_posicion])
        nueva_posicion = rand() % cant_validas;

    return nueva_posicion;
}


void disponer(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada casillas[cantidad_casillas];
    int casilla_elegida[cantidad_casillas];
    for(int i = 0; i < cantidad_casillas; casilla_elegida[i++] = 0);

    int cant_validas = posiciones_validas(casillas, mapa->casillas, mapa->alto, mapa->ancho);

    for (int colocadas = 0; colocadas < mapa->cant_torres; colocadas++) {
        int nueva_torre = determinar_posicion_torre(casilla_elegida, cant_validas);
        casilla_elegida[nueva_torre] = 1;
        int nueva_torre_x = casillas[nueva_torre].x;
        int nueva_torre_y = casillas[nueva_torre].y;
        colocar_torre(mapa, nueva_torre_x, nueva_torre_y, colocadas);
    }
}

Coordenada* filtrar_validas(TipoCasilla **casillas, int alto, int ancho) {
    Coordenada *validas = malloc (sizeof (Coordenada)*(alto * ancho));
    int cant_posiciones_validas = 0;
    
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            if (casillas[i][j] == VACIO) {
                validas[cant_posiciones_validas].x = i;
                validas[cant_posiciones_validas++].y = j;
            }
        }
    }

    return validas;
}

void backtrack(Coordenada *validas, int index, int cant_validas, Pila *actual, Pila *mejor, Mapa *mapa, int *mejor_ataque) {
    // Caso base: ya coloqué todas las torres necesarias
    if (actual->ultimo == mapa->cant_torres) {
        int ataque_total = 0;

        // Calcular el ataque total de la estrategia actual
        for (int i = 0; i < actual->ultimo; i++) {
            Coordenada *c = actual->datos[i];
            Tower torre = custom(c, mapa);  // Usamos la torre generada en esa coordenada
            ataque_total += torre.ataque;
        }

        // Si es la mejor estrategia encontrada hasta ahora, la guardamos
        if (ataque_total > *mejor_ataque) {
            *mejor_ataque = ataque_total;
            mejor->ultimo = 0;

            for (int i = 0; i < actual->ultimo; i++) {
                pila_apilar(mejor, actual->datos[i], copiar);
            }
        }

        return;
    }

    for (int i = index; i < cant_validas; i++) {
        pila_apilar(actual, &validas[i], copiar);
        backtrack(validas, i + 1, cant_validas, actual, mejor, mapa, mejor_ataque);
        pila_desapilar(actual, destruir); 
    }
}

void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    Coordenada *validas = filtrar_validas(mapa->casillas, mapa->alto, mapa->ancho);
    int cant_validas = posiciones_validas(validas, mapa->casillas, mapa->alto, mapa->ancho);

    Pila *actual = pila_crear(mapa->cant_torres);
    Pila *mejor = pila_crear(mapa->cant_torres);
    int mejor_ataque = -1;

    backtrack(validas, 0, cant_validas, actual, mejor, mapa, &mejor_ataque);

    for (int i = 0; i < mejor->ultimo; i++) {
        Coordenada *c = mejor->datos[i];
        colocar_torre(mapa, c->x, c->y, i);
    }

    free(validas);
    pila_destruir(actual, destruir);
    pila_destruir(mejor, destruir);
}


/*
void disponer_con_backtracking(Nivel* nivel, Mapa* mapa){
    Coordenada *pos_validas = filtrar_validas(mapa->casillas, mapa->alto, mapa->ancho);
    int cant_validas = posiciones_validas(pos_validas, mapa->casillas, mapa->alto, mapa->ancho);
    Pila *slots_torres = pila_crear(mapa->cant_torres);

    qsort(pos_validas, cant_validas, sizeof(Pila), distancia_al_origen); //ordeno para obtener la combinacion mas rapida

    for(int i=0; i<mapa->cant_torres; i++)
        pila_apilar(slots_torres, pos_validas[i]); //apilo la primera combinacion
        
    for(int i = (mapa->cant_torres -1); i < cant_validas; i++){
        for(int j = 0; j < mapa->cant_torres; j++)
            if(simular_nivel(nivel, mapa, disponer_con_backtracking))    
                colocar_torre(mapa, slots_torres->datos[j].x, slots_torres->datos[j].y, j);
            else{
                pila_desapilar(slots_torres);
            }
    }


    return;
}
*/


Tower custom (Coordenada* torre,  Mapa* mapita) {
    Tower torre_ord;
    torre_ord.ataque = 0;
    int rango = mapita->distancia_ataque;

    for (int i = -rango; i <= rango; i++)
        for (int j = -rango; j <= rango; j++) {
            
            int hit_x = torre->x + i;
            int hit_y = torre->y + j;

            if (i == 0 && j == 0) continue;

            if (hit_x >= 0 && hit_x < mapita->ancho && hit_y >= 0 && hit_y < mapita->alto)
                if (mapita->casillas[hit_x][hit_y] == CAMINO) {
                    torre_ord.ataque++;
            }
    }

    torre_ord.pos_torre.x = torre->x;
    torre_ord.pos_torre.y = torre->y;

    return torre_ord;
}

int distancia_al_origen(Tower *torre) {
    return torre->pos_torre.x * torre->pos_torre.x + torre->pos_torre.y * torre->pos_torre.y;
}

int comparar_torres(Tower *torre_a, Tower *torre_b) {
    if (torre_b->ataque != torre_a->ataque) {
        return torre_b->ataque - torre_a->ataque;
    }

    return distancia_al_origen(torre_a) - distancia_al_origen(torre_b);
}
void disponer_custom(Nivel* nivel, Mapa* mapa) {

    Coordenada *pos_validas = filtrar_validas(mapa->casillas, mapa->alto, mapa->ancho);
    int cant_validas = posiciones_validas(pos_validas, mapa->casillas, mapa->alto, mapa->ancho);
    Tower *posibles_torres = malloc(sizeof(Tower) * cant_validas);

    for(int i = 0; i<cant_validas; i++)
    posibles_torres[i] = custom(&pos_validas[i], mapa); //lleno el arreglo
    
    qsort(posibles_torres, cant_validas, sizeof(Tower), comparar_torres); //ordena por primera prioridad el ataque y por segunda la cerania a (0,0)
    
    for(int i = 0; i < mapa->cant_torres; i++)
        colocar_torre(mapa, posibles_torres[i].pos_torre.x, posibles_torres[i].pos_torre.y, i); //coloca una i cantidad de torres
}
