#include "estrategia.h"
#include "turno.h"
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



Coordenada* posiciones_validas_backtracking(Coordenada *pos_validas, TipoCasilla **casillas, int alto, int ancho) {
    int cant_posiciones_validas = 0;
    
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            if (casillas[i][j] == VACIO) {
                validas[cant_posiciones_validas].x = i;
                validas[cant_posiciones_validas++].y = j;
            }
        }
    }

    return pos_validas;
}
//static int determinar_posicion_torre_backtracking(int *casilla_elegida, Coordenada *validas, int cant_validas) {
  //  for(int i = 0; i < cant_validas; i++){
    //    posibles_torres->torres[i] = validas[i];
    //}
//}

int backtrack(Mapa* mapa, Nivel* nivel, Coordenada* pos_validas, int cant_validas, Coordenada* torres_actuales, int indice_torre) {
    if (indice_torre == mapa->cant_torres) {
        // Ya colocamos todas las torres, ahora evaluamos si la configuración es efectiva
        int hubo_escape = simular_turno(mapa, nivel, torres_actuales, mapa->cant_torres);
        return (hubo_escape == 0);
    }

    for (int i = 0; i < cant_validas; i++) {
        // Evitar usar la misma casilla más de una vez
        bool ya_usada = 0;
        for (int j = 0; j < indice_torre; j++) {
            if (torres_actuales[j].x == pos_validas[i].x &&
                torres_actuales[j].y == pos_validas[i].y) {
                ya_usada = 1;
                break;
            }
        }
        if (ya_usada) continue;

        torres_actuales[indice_torre] = posiciones_validas[i];
        
        return (backtrack(mapa, nivel, posiciones_validas, cant_validas, torres_actuales, indice_torre + 1));

    }
}
void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada pos_validas[cantidad_casillas];
    int cant_validas = posiciones_validas(pos_validas, mapa->casillas, mapa->alto, mapa->ancho);

    Coordenada torres_actuales[mapa->cant_torres];

    if (backtrack(mapa, nivel, pos_validas, cant_validas, torres_actuales, 0)) {
        for (int i = 0; i < mapa->cant_torres; i++) {
            colocar_torre(mapa, torres_actuales[i].x, torres_actuales[i].y, i);
        }
    }
}

/*
void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada casillas[cantidad_casillas];
    int casilla_elegida[cantidad_casillas];
    int cant_validas = posiciones_validas(casillas, mapa->casillas, mapa->alto, mapa->ancho);
    Coordenada validas = posiciones_validas_backtracking(*casilla_elegida, validas, cant_validas);
    for(int i = 0; i < cantidad_casillas; casilla_elegida[i++] = 0);
    
    Pila posibles_torres[mapa->cant_torres];
    posibles_torres->ultimo = 0;

    for(int torre = 0; torre < mapa->cant_torres; torre++){
        int nueva_torre = determinar_posicion_torre(casilla_elegida, cant_validas);
        casilla_elegida[nueva_torre] = 1;
        posibles_torres->torres[torre].x = validas[nueva_torre].x;
        posibles_torres->torres[torre].y = validas[nueva_torre].y;
        posibles_torres->ultimo++;
    }
        if(simular_turno(mapa, nivel, posibles_torres->torres, ))
            for(int torre_a_colocar =0; torre_a_colocar < mapa->cant_torres; torre_a_colocar++)
            colocar_torre(mapa, posibles_torres->torres[torre_a_colocar].x, posibles_torres->torres[torre_a_colocar].y, torre_a_colocar);
        else
            pila_desapilar(posibles_torres);
    return;
}
*/

//la idea es que se coloquen las torres lo mas arriba y con mayor ataque posible (en esquinas por ej). Pense en hacer un orden de prioridad, alcance y cercania al inicio
/* typedef struct {
    int ancho, alto, cant_torres, distancia_ataque;
    TipoCasilla **casillas;
    Coordenada *torres;
} Mapa;
*/

CasoMapa bifurcacion(Mapa *mapita){
    CasoMapa bifurcacion;
    bifurcacion.hay_bifurcacion = 0;

    for (int i = 1; i < mapita->ancho - 1; i++)
        for (int j = 1; j < mapita->alto - 1; j++) {
            if (mapita->casillas[i][j] == CAMINO) {
                int vecinos = 0;
                if (mapita->casillas[i+1][j] == CAMINO) vecinos++;
                if (mapita->casillas[i-1][j] == CAMINO) vecinos++;
                if (mapita->casillas[i][j+1] == CAMINO) vecinos++;
                if (mapita->casillas[i][j-1] == CAMINO) vecinos++;

                if(vecinos >=3){
                    bifurcacion.hay_bifurcacion = 1;
                    bifurcacion.pos_bifurcacion.x = i;
                    bifurcacion.pos_bifurcacion.y = j;
                    return bifurcacion;
                }
            }
        }

    return bifurcacion;
}

int custom (Coordenada torre,  Mapa* mapa) {
    int ataque = 0;
    int alcance = mapa->distancia_ataque;
    for (int i = torre->x - alcance; i <= torre->x + alcance; i++) {
        for (int j = torre->y - alcance; j <= torre->y + alcance; j++) {
            if (i >= 0 && i < mapa->alto && j >= 0 && j < mapa->largo) {
                if (mapa->casillas[i][j] == CAMINO) { 
                    ataque++;
                }
            }
        }
    }
    return ataque;
}

//no esta hecho!!
void disponer_custom(Nivel* nivel, Mapa* mapa) {
    int cantidad_casillas = mapa->alto * mapa->ancho;
    Coordenada pos_validas[cantidad_casillas];
    int cant_validas = posiciones_validas(pos_validas, mapa->casillas, mapa->alto, mapa->ancho);

    Coordenada torres_actuales[mapa->cant_torres];

    if () {
        for (int i = 0; i < mapa->cant_torres; i++) {
            colocar_torre(mapa, torres_actuales[i].x, torres_actuales[i].y, i);
        }
    }
}
