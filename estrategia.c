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
    // pilitaizar torre
    mapa->torres[nro_torre].x = x;
    mapa->torres[nro_torre].y = y;

    // pilitaizar mapa
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

void reiniciar_mapa(Mapa *mapa) {
    for (int i = 0; i < mapa->alto; i++) {
        for (int j = 0; j < mapa->ancho; j++) {
            if (mapa->casillas[i][j] == TORRE) {
                mapa->casillas[i][j] = VACIO;
            }
        }
    }

     for (int i = 0; i < mapa->cant_torres; i++) {
        mapa->torres[i].x = -1;
        mapa->torres[i].y = -1;
    }
}
//hice la funcion para reiniciar el mapa y que las pruebas de torres se hagan desde 0 y no haya problemas


void disponer_con_backtracking(Nivel* nivel, Mapa* mapa) {
    Coordenada* validas = filtrar_validas(mapa->casillas, mapa->alto, mapa->ancho);
    int cant_validas = posiciones_validas(validas, mapa->casillas, mapa->alto, mapa->ancho);

    int cant_torres = mapa->cant_torres;
    if (cant_torres > cant_validas)
        cant_torres  = cant_validas;

    Pila* pilita = pila_crear(100);
    Pila* mejor = pila_crear(cant_torres);
    int win = 0;

    Coordenada* prueba = malloc(sizeof(Coordenada) * cant_torres);

    pila_apilar(pilita, NULL, copiar);

    while (pilita->ultimo > 0 && !win) {
        int cant_apiladas = pilita->ultimo - 1;
        Coordenada** torres_probadas = (Coordenada**) pilita->datos;

        // Copiar combinación pilita
        for (int i = 0; i < cant_apiladas; i++)
            prueba[i] = *(torres_probadas[i]);

        int inicio = 0;
        if (cant_apiladas > 0)
            inicio = torres_probadas[cant_apiladas - 1]->x * mapa->ancho + torres_probadas[cant_apiladas - 1]->y + 1;
            //hago que la coordenada sea un unico número en un arreglo que de las posiciones del mapa.
  
        for (int i = inicio; i < cant_validas; i++) {
            Coordenada siguiente = validas[i];

            if (cant_apiladas + 1 <= cant_torres) {
                pila_apilar(pilita, &siguiente, copiar);

                if (cant_apiladas + 1 == cant_torres ) {
                    //pongo las torres
                    for (int j = 0; j < cant_torres; j++) {
                        Coordenada* c = (Coordenada*)pilita->datos[j + 1];
                        colocar_torre(mapa, c->x, c->y, j);
                    }

                    //simulo el nivel, si ganaste congrats!!
                    if (simular_nivel(nivel, mapa, NULL)) {
                        win = 1;
                        for (int j = 0; j < cant_torres; j++)
                            pila_apilar(mejor, pilita->datos[j + 1], copiar);
                        break;
                    }

                    reiniciar_mapa(mapa); //para que la prueba se repita desde 0 y no haya problemas
                }

                pila_desapilar(pilita, destruir); //vuelvo para probar nuevas
            }
        }

        if (!win)
            pila_desapilar(pilita, destruir); //saco las torres que ya probe
    }

    // Colocar mejor estrategia encontrada
    for (int i = 0; i < mejor->ultimo; i++) {
        Coordenada* c = (Coordenada*)mejor->datos[i];
        colocar_torre(mapa, c->x, c->y, i);
    }

    free(validas);
    free(prueba);
    pila_destruir(pilita, destruir);
    pila_destruir(mejor, destruir);
}

/*
void disponer_con_backtracking(Nivel* nivel, Mapa* mapa){
    Coordenada *pos_validas = filtrar_validas(mapa->casillas, mapa->alto, mapa->ancho);
    int cant_validas = posiciones_validas(pos_validas, mapa->casillas, mapa->alto, mapa->ancho);
    Pila *slots_torres = pila_crear(mapa->cant_torres);

    qsort(pos_validas, cant_validas, sizeof(Pila), distancia_al_origen); //ordeno para obtener la prueba mas rapida

    for(int i=0; i<mapa->cant_torres; i++)
        pila_apilar(slots_torres, pos_validas[i]); //apilo la primera prueba
        
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



int distancia_al_origen(const Tower *torre) {
    return torre->pos_torre.x * torre->pos_torre.x + torre->pos_torre.y * torre->pos_torre.y;
}

int comparar_torres(const void *a, const void *b) {
    const Tower *torre_a = (const Tower *)a;
    const Tower *torre_b = (const Tower *)b;

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
    posibles_torres[i] = custom(&pos_validas[i], mapa); //lcant_apiladaso el arreglo
    
    qsort(posibles_torres, cant_validas, sizeof(Tower), comparar_torres); //ordena por primera prioridad el ataque y por segunda la cerania a (0,0)
    
    for(int i = 0; i < mapa->cant_torres; i++)
        colocar_torre(mapa, posibles_torres[i].pos_torre.x, posibles_torres[i].pos_torre.y, i); //coloca una i cantidad de torres
}
