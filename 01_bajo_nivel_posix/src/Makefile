CC = gcc
CFLAGS = -Wall

SOURCES = \
	01_guardar_enteros.c \
	02_guardar_caracter.c \
	03_leer_caracteres.c \
	04_registros.c \
	05_guardar_texto.c \
	06_leer_texto.c \
	07_guardar_recuperar_estructura.c \
	08_reemplazar_registro.c \
	09_salidas_estandar.c \
	10_copiar_archivo.c \
	11_entrada_estandar.c \
	12_info_archivo.c

PROGRAMAS = $(SOURCES:.c=)

.PHONY: all clean

all: $(PROGRAMAS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(PROGRAMAS)