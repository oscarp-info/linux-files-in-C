/*
 * Ejemplo 3: lee hasta tres caracteres de database.dat de forma secuencial.
 */
#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h> /* ssize_t */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

int main(void)
{
    char data = 0;       /* Carácter leído desde el archivo. */
    int fd;              /* Descriptor del archivo database.dat. */
    ssize_t leidos;   /* Cantidad de bytes leídos por read. */
    int i;               /* Contador de las tres lecturas. */

    fd = open("database.dat", O_RDONLY);
    if (fd == -1) {
        perror("No se pudo abrir database.dat");
        return EXIT_FAILURE;
    }

    for (i = 0; i < 3; i++) {
        leidos = read(fd, &data, sizeof(data));
        if (leidos == -1) {
            perror("No se pudo leer database.dat");
            close(fd);
            return EXIT_FAILURE;
        }
        if (leidos == 0) {
            printf("Se llego al final del archivo.\n");
            break;
        }

        printf("Se leyo 1 bloque de %zu byte: data = %c\n", sizeof(data), data);
    }

    if (close(fd) == -1) {
        perror("No se pudo cerrar database.dat");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}