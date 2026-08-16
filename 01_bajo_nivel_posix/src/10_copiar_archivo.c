/*
 * Ejemplo 10: copia un archivo usando read en un ciclo y write hasta escribir
 * todos los bytes leídos.
 */

#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h> /* ssize_t */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

#define TAM_BUFFER 128

static int escribir_todo(int fd, const char *buffer, size_t cantidad)
{
    size_t total = 0;       /* Bytes ya escritos del bloque actual. */
    ssize_t escritos;         /* Resultado de cada llamada a write. */

    while (total < cantidad) {
        escritos = write(fd, buffer + total, cantidad - total);

        if (escritos <= 0)
            return -1;

        total += (size_t)escritos;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char buffer[TAM_BUFFER]; /* Bloque temporal para copiar datos. */
    int origen;                /* Descriptor del archivo que se copia. */
    int destino;               /* Descriptor del archivo creado. */
    ssize_t leidos;            /* Bytes obtenidos en cada lectura. */

    if (argc != 3) {
        printf("Uso: %s ARCHIVO_ORIGEN ARCHIVO_DESTINO\n", argv[0]);
        return EXIT_FAILURE;
    }

    origen = open(argv[1], O_RDONLY);
    if (origen == -1) {
        perror("No se pudo abrir el archivo de origen");
        return EXIT_FAILURE;
    }

    destino = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destino == -1) {
        perror("No se pudo crear el archivo de destino");
        close(origen);
        return EXIT_FAILURE;
    }

    while ((leidos = read(origen, buffer, sizeof(buffer))) > 0) {
        if (escribir_todo(destino, buffer, (size_t)leidos) == -1) {
            perror("No se pudo escribir el archivo de destino");
            close(origen);
            close(destino);
            return EXIT_FAILURE;
        }
    }

    if (leidos == -1) {
        perror("No se pudo leer el archivo de origen");
        close(origen);
        close(destino);
        return EXIT_FAILURE;
    }

    if (close(origen) == -1 || close(destino) == -1) {
        perror("No se pudo cerrar un archivo");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}