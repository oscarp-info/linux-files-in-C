CC = gcc
CFLAGS = -Wall

SOURCES = \
	01_guardar_enteros.c \
	02_guardar_caracter.c \
	03_lectura_secuencial.c \
	04_registros_busqueda_directa.c

PROGRAMAS = $(SOURCES:.c=)

.PHONY: all clean

all: $(PROGRAMAS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(PROGRAMAS)
