/*
 * Ejemplo 12: muestra el tamaño y los permisos de un archivo usando stat.
 */

#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/stat.h>  /* stat y struct stat */

int main(int argc, char *argv[])
{
    struct stat info; /* Estructura donde stat guarda los metadatos. */

    if (argc != 2) {
        printf("Uso: %s ARCHIVO\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (stat(argv[1], &info) == -1) {
        perror("No se pudo obtener la información del archivo");
        return EXIT_FAILURE;
    }

    printf("Archivo: %s\n", argv[1]);
    printf("Tamaño: %lld bytes\n", (long long)info.st_size);
    printf("Permisos: %03o\n", (unsigned int)(info.st_mode & 0777));

    return EXIT_SUCCESS;
}