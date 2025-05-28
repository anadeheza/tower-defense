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

Pila * pila_crear (int tamIn){

    Pila *pilita = malloc(sizeof(Pila));
    pilita->datos = malloc(sizeof(int) * tamIn);
    pilita->tam = tamIn; 
    
    return  pilita;
}

void pila_apilar(Pila *pilita, Coordenada nuevo) {
    if(pilita->tam == pilita->ultimo) {
        pilita->tam *= 2;
        pilita->datos = realloc(pilita->datos, pilita->tam * sizeof(int));
    }
    pilita->datos[pilita->ultimo] = nuevo;
    pilita->ultimo ++;
}

void pila_desapilar(Pila *pilita) {
    pilita->ultimo --;
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

void disponer_con_backtracking(Mapa* mapa) {
    Pila* pila = pila_crear();
    if (!pila) return;

    Estado inicial;
    inicial.mapita = *mapa;
    inicial.torresColocadas = 0;
    inicial.fila = 0;
    inicial.columna = 0;

    pila_apilar(pila, &inicial);

    while (!pila_es_vacia(pila)) {
        Estado actual;
        pila_desapilar(pila);

        if (actual.torresColocadas == mapa->cant_torres) {
            // Copiar el estado actual al mapa original
            *mapa = actual.mapita;
            break; // Solución encontrada
        }
        
        for (int f = actual.fila; f < mapa->alto; f++) {
            for (int c = (f == actual.fila? actual.columna : 0); c < mapa->ancho; c++) {

                if (es_posicion_valida(&actual.mapita, f, c)) {
                    Estado nuevo;
                    nuevo.mapita = actual.mapita;
                    nuevo.torresColocadas = actual.torresColocadas + 1;
                    nuevo.fila = f;
                    nuevo.columna = c + 1;

                    colocar_torre(&nuevo.mapita, f, c);
                    pila_apilar(pila, &nuevo);
                }
            }
        }
    }
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
            
            int hit_x = torre.x + i;
            int hit_y = torre.y + j;

            if (i == 0 && j == 0) continue;

            if (hit_x >= 0 && hit_x < mapita->ancho && hit_y >= 0 && hit_y < mapita->alto)
                if (mapita->casillas[hit_x][hit_y] == CAMINO) {
                    torre_ord.ataque++;
            }
    }

    torre_ord.pos_torre.x = torre.x;
    torre_ord.pos_torre.y = torre.y;

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
    posibles_torres[i] = custom(pos_validas[i], mapa); //lleno el arreglo
    
    qsort(posibles_torres, cant_validas, sizeof(Tower), comparar_torres); //ordena por primera prioridad el ataque y por segunda la cerania a (0,0)
    
    for(int i = 0; i < mapa->cant_torres; i++)
        colocar_torre(mapa, posibles_torres[i].pos_torre.x, posibles_torres[i].pos_torre.y, i); //coloca una i cantidad de torres
}
