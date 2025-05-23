#include "simulador.h"
#include "estrategia.h"

void inicializar_simulacion(const char* filename, Nivel** nivel, Mapa **mapa) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("No se pudo abrir el archivo de nivel");
        exit(1);
    }

    int ancho, alto, cant_enemigos, cant_torres, largo_camino, distancia_ataque, vida_inicial;
    fscanf(f, "Ancho: %d\n", &ancho);
    fscanf(f, "Alto: %d\n", &alto);
    fscanf(f, "Cantidad de enemigos: %d\n", &cant_enemigos);
    fscanf(f, "Vida inicial: %d\n", &vida_inicial);
    fscanf(f, "Cantidad de torres: %d\n", &cant_torres);
    fscanf(f, "Distancia de ataque: %d\n", &distancia_ataque);
    fscanf(f, "Largo camino: %d\n", &largo_camino);

    *nivel = inicializar_nivel(largo_camino, cant_enemigos, vida_inicial);
    *mapa = inicializar_mapa(ancho, alto, cant_torres, distancia_ataque);

    char fila[MAX_LINEA];
    int n_camino = 0;
    for (int i = 0; i < alto; i++) {
        fscanf(f, "%s", fila);
        n_camino += procesar_fila(fila, ancho, i, (*mapa)->casillas, (*nivel)->camino->posiciones, n_camino);
    }

    fclose(f);
}

void liberar_simulacion(Nivel *nivel, Mapa *mapa) {
    liberar_nivel(nivel);
    liberar_mapa(mapa);
}

static int area_ataque(int distancia) {
    return 4 * distancia * (distancia + 1);
}

static int calcular_posiciones(Coordenada posicion_torre, Coordenada *posiciones_ataque, int indice_ataque, int ancho, int alto, int distancia_ataque) {
    int nro_ataques = 0;

    for (int dx = -distancia_ataque; dx <= distancia_ataque; dx++) {
        for (int dy = -distancia_ataque; dy <= distancia_ataque; dy++) {
            int nuevo_x = posicion_torre.x + dx;
            int nuevo_y = posicion_torre.y + dy;

            if (dx == 0 && dy == 0) continue;
            if (nuevo_x < 0 || nuevo_y < 0) continue;
            if (nuevo_x >= alto || nuevo_y >= ancho) continue;

            posiciones_ataque[indice_ataque + nro_ataques].x = nuevo_x;
            posiciones_ataque[indice_ataque + nro_ataques].y = nuevo_y;
            nro_ataques++;
        }
    }

    return nro_ataques;
}

int simular_nivel(Nivel *nivel, Mapa *mapa, DisposicionTorres colocar_torres) {
    inicializar_turno(nivel, mapa, colocar_torres);

    int nro_ataques = mapa->cant_torres * area_ataque(mapa->distancia_ataque);
    Coordenada posiciones_ataque[nro_ataques];
    int nro_ataques_efectivos = 0;

    for (int i = 0; i < mapa->cant_torres; i++) {
        nro_ataques_efectivos += calcular_posiciones(mapa->torres[i], posiciones_ataque, nro_ataques_efectivos, mapa->ancho, mapa->alto, mapa->distancia_ataque);
    }
        
    int escape = 0;
    for (int turno = 0; nivel->enemigos->cantidad_activos && !escape; turno++) {
        mostrar_mapa(mapa, nivel->enemigos);

        escape += simular_turno(mapa, nivel, posiciones_ataque, nro_ataques_efectivos);
        sleep(1);
    }

    return !(nivel->enemigos->cantidad_activos);
}

static int mostrar_menu(DisposicionTorres estrategia_actual, char *ruta_nivel_actual) {
    int opcion = 3;

    limpiar_pantalla();
    printf("\n--- Menú del simulador ---\n");
    printf("1. Seleccionar estrategia (actual: %s)\n", (estrategia_actual == disponer) ? "Básica" : "Otra");
    printf("2. Seleccionar nivel (actual: %s)\n", ruta_nivel_actual);
    printf("3. Iniciar simulación\n");
    printf("0. Salir\n");

    printf("Seleccione una opción: ");
    scanf("%d", &opcion);

    return opcion;
}

static DisposicionTorres determinar_estrategia() {
    int opcion = 0;

    limpiar_pantalla();
    printf("\n--- Opciones de estrategia ---\n");
    printf("0. Disposición básica\n");
    printf("1. Disposición con backtracking\n");
    printf("2. Disposición custom\n");

    printf("Seleccione una opción: ");
    scanf("%d", &opcion);

    switch(opcion) {
        case 1:
            return disponer_con_backtracking;
        case 2:
            return disponer_custom;
        case 0:
        default:
            return disponer;
    }
}

static void mostrar_mensaje_final(int ganador) {
    if(ganador) {
        printf("\n¡Conseguiste eliminar a todos los enemigos!\n");
        return;
    }

    printf("\n¡Se te escaparon!\n");
}

int main() {
    srand(time(NULL));
    DisposicionTorres estrategia_actual = disponer;
    char ruta_nivel_actual[MAX_LINEA];
    strcpy(ruta_nivel_actual, "Levels/nivel01.txt");

    Nivel *nivel = NULL;
    Mapa *mapa = NULL;

    int opcion = 1, memoria_a_liberar = 0;
    while (opcion != 0) {
        opcion = mostrar_menu(estrategia_actual, ruta_nivel_actual);

        switch(opcion) {
            case 1:
                estrategia_actual = determinar_estrategia();
                break;
            case 2:
                printf("Ingrese la ruta al archivo de nivel: ");
                scanf("%s", ruta_nivel_actual);

                if(memoria_a_liberar) {
                    liberar_simulacion(nivel, mapa);
                    memoria_a_liberar = 0;
                }
                
                inicializar_simulacion(ruta_nivel_actual, &nivel, &mapa);
                memoria_a_liberar = 1;
                
                printf("Vas a jugar en el sigueinte nivel\n");
                mostrar_mapa(mapa, nivel->enemigos);
                sleep(2);
                break;
            case 3:
                if(memoria_a_liberar) {
                    liberar_simulacion(nivel, mapa);
                    memoria_a_liberar = 0;
                }
                
                inicializar_simulacion(ruta_nivel_actual, &nivel, &mapa);
                int ganador = simular_nivel(nivel, mapa, estrategia_actual);
                mostrar_mapa(mapa, nivel->enemigos);
                mostrar_mensaje_final(ganador);
                sleep(3);
                memoria_a_liberar = 1;
                break;
            default:
                if(memoria_a_liberar) {
                    liberar_simulacion(nivel, mapa);
                    memoria_a_liberar = 0;
                }

                break;
        }            
    }

    return 0;
}