#include "archivo.h"

static TipoCasilla transformar_a_casilla(char c) {
    switch(c) {
        case 'C':
            return CAMINO;
        case 'V':
            return VACIO;
        case 'B':
        default:
            return BLOQUEADO;
    }
}

int procesar_fila(char *fila, int largo_fila, int nro_fila, TipoCasilla **tablero, Coordenada *camino, int n_camino) {
    int cant_caminos = 0;
    
    for (int i = 0; i < largo_fila; i++){
        tablero[nro_fila][i] = transformar_a_casilla(fila[i]);

        if (fila[i] == 'C') {
            camino[n_camino + cant_caminos].x = i;
            camino[n_camino + cant_caminos++].y = nro_fila;
        }
    }

    return cant_caminos;
}