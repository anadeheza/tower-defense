#include "pila.h"

Pila * pila_crear (int tamIn){

    Pila *pilita = malloc(sizeof(Pila));
    pilita->datos = malloc(sizeof(Coordenada*) * tamIn);
    pilita->tam = tamIn;
    pilita->ultimo = 0;
    return  pilita;
}

void* copiar(void* elemento) {
    if (elemento == NULL) return NULL;

    Coordenada* original = (Coordenada*)elemento;
    Coordenada* copia = malloc(sizeof(Coordenada));
    if (copia == NULL) return NULL;

    *copia = *original; //Copia los valores de las coordenadas x e y 
    return copia;
}



void pila_apilar(Pila *pilita, void *nuevo, FuncionCopia crear) {
    if(pilita->tam == pilita->ultimo) {
        pilita->tam *= 2;
        pilita->datos = realloc(pilita->datos, pilita->tam * sizeof(Coordenada*));
    }
    pilita->datos[pilita->ultimo] = crear(nuevo);
    pilita->ultimo ++;
}

void destruir(void* elemento) {
    if (elemento != NULL) {
        free(elemento);
    }
}

void pila_desapilar(Pila *pilita, FuncionDestructora destruir) {
    if (pilita->ultimo > 0) {
        pilita->ultimo--;
        destruir(pilita->datos[pilita->ultimo]);
    }
}

void pila_destruir(Pila *pilita, FuncionDestructora destruir){
    while (pilita->ultimo > 0)
        pila_desapilar(pilita, destruir);
    free(pilita->datos);
    free(pilita);
}