/*
 * Ejemplo 1: guarda dos valores enteros en el archivo binario database.dat.
 */
#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h> /* ssize_t */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

int main(void)
{
    int data_01 = 1;       /* Primer entero que se guardará. */
    int data_02 = 2;       /* Segundo entero que se guardará. */
    int fd;                /* Descriptor del archivo database.dat. */
    ssize_t escritos;     /* Cantidad de bytes escritos por write. */

    fd = open("database.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("No se pudo abrir database.dat");
        return EXIT_FAILURE;
    }

    escritos = write(fd, &data_01, sizeof(data_01));
    if (escritos != (ssize_t)sizeof(data_01)) {
        perror("No se pudo escribir el primer entero");
        close(fd);
        return EXIT_FAILURE;
    }

    escritos = write(fd, &data_02, sizeof(data_02));
    if (escritos != (ssize_t)sizeof(data_02)) {
        perror("No se pudo escribir el segundo entero");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Se escribieron 2 enteros de %zu bytes cada uno.\n", sizeof(data_01));

    if (close(fd) == -1) {
        perror("No se pudo cerrar database.dat");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}