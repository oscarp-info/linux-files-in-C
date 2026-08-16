CC = gcc
CFLAGS = -Wall

SOURCES = \
	05_caracteres.c \
	06_lineas.c \
	07_datos_formateados.c \
	08_agregar_lineas.c \
	09_separar_campos.c \
	10_conversion_segura.c \
	11_stdout_stderr.c

PROGRAMAS = $(SOURCES:.c=)

.PHONY: all clean

all: $(PROGRAMAS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(PROGRAMAS)
