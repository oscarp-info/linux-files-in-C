CC = gcc
CFLAGS = -Wall

SOURCES = \
	01_lectura_no_bloqueante.c \
	02_esperar_entrada.c \
	03_consultar_terminal.c \
	04_audio_oss.c

PROGRAMAS = $(SOURCES:.c=)

.PHONY: all clean

all: $(PROGRAMAS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(PROGRAMAS)
