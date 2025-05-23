CC       := gcc
CFLAGS   := -Wall -g
BIN      := tower_defense
LFLAGS   := -lm

SRC      := simulador.c archivo.c estrategia.c mapa.c nivel.c turno.c
DEPS     := archivo.h mapa.h nivel.h simulador.h estrategia.h turno.h
OBJ      := $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)

clean:
	rm -f *.o

.PHONY: all clean
