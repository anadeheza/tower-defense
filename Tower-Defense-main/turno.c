#include "turno.h"

static void desplazar_enemigos(Enemigos *enemigos) {
    Coordenada *posiciones = enemigos->posiciones;
    int cant_enemigos = enemigos->cantidad;
    
    for (int i = cant_enemigos - 1; i > 0; i--)
        posiciones[i] = posiciones[i - 1];
}

static int camino(Mapa *mapa, Coordenada *lider, int limite, int dx, int dy) {
    if(dx != 0 && (lider->x + dx < 0 || lider->x + dx >= limite))
        return 0;
    else if (lider->y + dy < 0 || lider->y + dy >= limite)
        return 0;

    int casilla = mapa->casillas[lider->y + dy][lider->x + dx];
    return casilla == CAMINO;
}
static int camino_derecha(Mapa *mapa, Coordenada *lider, int ancho) {
    return camino(mapa, lider, ancho, 1, 0);
}
static int camino_abajo(Mapa *mapa, Coordenada *lider, int alto) {
    return camino(mapa, lider, alto, 0, 1);
}
    
static void decidir_nueva_posicion(Enemigos *enemigos, Mapa *mapa) {
    Coordenada *lider = enemigos->posiciones; // primer enemigo
    int ancho = mapa->ancho;
    int alto = mapa->alto;

    int direccion = rand() % 2; // 0 = abajo, 1 = derecha
    int hay_abajo = camino_abajo(mapa, lider, alto);
    int hay_derecha = camino_derecha(mapa, lider, ancho);

    if ((lider->x >= alto && lider->y >= ancho && direccion) || // fuera del mapa
        !hay_abajo || (hay_derecha && direccion)) { // derecha forzada o derecha elegida
        lider->x++;
        return;
    }
    
    // Abajo
    lider->y++;
}

static int actualizar_activos(Nivel *nivel, Mapa *mapa) {
    int escape = 0;
    int mia = 0;
    
    for (int i = 0; i < nivel->enemigos->cantidad ; i++) {
        if (nivel->enemigos->vida[i] == 0) {
            nivel->enemigos->activos[i] = 0;
            mia++;
        }

        if ((nivel->enemigos->posiciones[i].x >= mapa->ancho ||
            nivel->enemigos->posiciones[i].y >= mapa->alto) &&
            nivel->enemigos->vida[i]) {
            nivel->enemigos->activos[i] = 0;
            escape++;
        }
    }

    nivel->enemigos->cantidad_activos = nivel->enemigos->cantidad - (escape + mia);
    return escape;
}

static int enemigo_en_mapa(Coordenada enemigo, int ancho, int alto) {
    return enemigo.x >= 0 && enemigo.x < ancho &&
           enemigo.y >= 0 && enemigo.y < alto;
}

static void actualizar_mapa(Nivel *nivel, Mapa *mapa) {
    // Actualizar camino
    for (int i = 0; i < nivel->camino->largo_camino ; i++)
        mapa->casillas[nivel->camino->posiciones[i].y][nivel->camino->posiciones[i].x] = CAMINO;

    // Actualizar enemigos
    for (int i = 0; i < nivel->enemigos->cantidad ; i++) {
        if(enemigo_en_mapa(nivel->enemigos->posiciones[i], mapa->ancho, mapa->alto) && nivel->enemigos->activos[i]) {
            mapa->casillas[nivel->enemigos->posiciones[i].y][nivel->enemigos->posiciones[i].x] = ENEMIGO;
        }
    }
}

static int es_enemigo(TipoCasilla t) {
    return t == ENEMIGO;
}

int buscarEnemigo(Coordenada *posiciones_enemigos, int cant_enemigos, Coordenada ataque) {
    int encontrado = 0, posicion = -1;

    for (int i = 0; i < cant_enemigos && !encontrado; i++){
        if(posiciones_enemigos[i].y == ataque.x &&
           posiciones_enemigos[i].x == ataque.y) {
            encontrado = 1;
            posicion = i;
        }
    }
    
    return posicion;
}


static void disminuir_vidas(Nivel *nivel, Mapa *mapa, Coordenada *posiciones_ataque, int nro_ataques) {
    int nro_enemigo = 0, enemigo = 0;
    
    for (int i = 0; i < nro_ataques; i++) {
        enemigo = es_enemigo(mapa->casillas[posiciones_ataque[i].x][posiciones_ataque[i].y]);
        if(!enemigo) continue;

        nro_enemigo = buscarEnemigo(nivel->enemigos->posiciones, nivel->enemigos->cantidad, posiciones_ataque[i]);

        if (nivel->enemigos->vida[nro_enemigo])
            nivel->enemigos->vida[nro_enemigo]--;
    }
}

int simular_turno(Mapa *mapa, Nivel *nivel, Coordenada posiciones_ataque[], int ataques_efectivos) {
    // atacan las torres
    disminuir_vidas(nivel, mapa, posiciones_ataque, ataques_efectivos);
    int hubo_escape = actualizar_activos(nivel, mapa);
    
    // avanza el enemigo a paso redoblado
    desplazar_enemigos(nivel->enemigos);
    decidir_nueva_posicion(nivel->enemigos, mapa);
    actualizar_mapa(nivel, mapa);

    return hubo_escape;
}

void inicializar_turno(Nivel *nivel, Mapa *mapa, DisposicionTorres colocar_torres) {
    colocar_torres(nivel, mapa);
    
    // posicion inicial
    nivel->enemigos->posiciones[0].x = 0;
    nivel->enemigos->posiciones[0].y = 0;
    nivel->enemigos->vida[0] = nivel->enemigos->vida_inicial;

    actualizar_mapa(nivel, mapa);
}