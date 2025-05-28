#ifndef PILA_H
#define PILA_H
#include "nivel.h"


typedef struct Stack_ {
int tam;
Coordenada **datos;
int ultimo;
} Pila;

typedef void*(*FuncionCopia)(void*);
typedef void(*FuncionDestructora)(void*);

Pila *pila_crear(int); 

int pila_es_vacia(Pila*); 

int pila_tope(Pila*); 

void pila_apilar(Pila*, void*, FuncionCopia);

void pila_desapilar(Pila*, FuncionDestructora);

void pila_destruir(Pila*, FuncionDestructora);

void* copiar(void* elemento);
void destruir(void* elemento);

#endif
