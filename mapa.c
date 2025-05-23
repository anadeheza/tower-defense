#include "mapa.h"
#include "turno.h"

void limpiar_pantalla() {
    system(limpiar);
}

Mapa *inicializar_mapa(int ancho, int alto, int cant_torres, int distancia_ataque) {
    Mapa *mapa = malloc(sizeof(Mapa));
    mapa->casillas = malloc(sizeof(TipoCasilla*) * alto);

    for (int x = 0; x < alto; x++)
        mapa->casillas[x] = malloc(sizeof(TipoCasilla) * ancho);

    mapa->cant_torres = cant_torres;
    mapa->torres = malloc(sizeof(Coordenada) * cant_torres);
    mapa->ancho = ancho;
    mapa->alto = alto;
    mapa->distancia_ataque = distancia_ataque;

    return mapa;
}

void liberar_mapa(Mapa *mapa) {
    for (int i = 0; i < mapa->alto; i++)
        free(mapa->casillas[i]);
    free(mapa->casillas);
    free(mapa->torres);
    free(mapa);
}

static void hsv_to_rgb_ansi(float h, int *r, int *g, int *b) {
    float c = 1.0;
    float x = (1 - fabs(fmodf(h / 60.0, 2) - 1)) * c;
    float r_f = 0, g_f = 0, b_f = 0;

    if (h < 60)       { r_f = c; g_f = x; }
    else if (h < 120) { r_f = x; g_f = c; }
    else if (h < 180) { g_f = c; b_f = x; }
    else if (h < 240) { g_f = x; b_f = c; }
    else if (h < 300) { r_f = x; b_f = c; }
    else              { r_f = c; b_f = x; }

    *r = (int)(r_f * 5 + 0.5);
    *g = (int)(g_f * 5 + 0.5);
    *b = (int)(b_f * 5 + 0.5);
}

static int color_de_vida(int vida, int vida_max) {
    if (vida <= 1) return RED;
    if (vida >= vida_max) return GREEN;

    int steps = vida_max - 2;
    int step = vida_max - vida;
    float hue = 120.0f * step / (float)steps;

    int r, g, b;
    hsv_to_rgb_ansi(hue, &r, &g, &b);
    return ANSI_COLOR_INDEX(r, g, b);
}

void imprimir_casilla(TipoCasilla tipo, int vida, int vida_max) {
    switch(tipo) {
        case CAMINO:
            printf(".  ");
            return;
        case TORRE:
            printf("T  ");
            return;
        case ENEMIGO:
            int color = color_de_vida(vida, vida_max);
            // Usar esta linea cuando se quieran imprimir vidas en vez de colores
            printf("%d  ", vida);
            // printf("\x1b[38;5;%dm%-3s\x1b[0m", color, "*");
            return;
        case BLOQUEADO:
        case VACIO:
            printf("   ");
            return;
    }
}

static void imprimir_borde_horizontal(int ancho, int top) {
    top ? printf("╔") : printf("╚");
    for (int i = 0; i < ancho; i++)
        printf("═══");
    top ? printf("╗\n") : printf("╝");
}

void mostrar_mapa(Mapa *mapa, Enemigos *enemigos) {
    limpiar_pantalla();

    imprimir_borde_horizontal(mapa->ancho, 1);

    for (int x = 0; x < mapa->alto; x++) {
        printf("║");
        for (int y = 0; y < mapa->ancho; y++) {
            TipoCasilla c = mapa->casillas[x][y];
            Coordenada coord;
            coord.x = x;
            coord.y = y;
            if (c == ENEMIGO) {
                int indice_enemigo = buscarEnemigo(enemigos->posiciones, enemigos->cantidad, coord);
                int vida = enemigos->vida[indice_enemigo];
                imprimir_casilla(c, vida, enemigos->vida_inicial);
            } else {
                imprimir_casilla(c, 0, enemigos->vida_inicial);
            }
        }
        printf("║\n");
    }

    imprimir_borde_horizontal(mapa->ancho, 0);

    printf("\n");
    printf("  .     = path\n");
    printf("  T     = tower\n");
    printf("  *     = enemy (verde -> maxima vida, rojo -> minima vida)\n");
    printf("  Enemigos vivos: %d\n", enemigos->cantidad_activos);
    printf("  Vida inicial: %d\n\n", enemigos->vida_inicial);
}
