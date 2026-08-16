/*
 * Ejemplo 2: guarda el carácter '1' en el archivo database.dat.
 */
#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h> /* ssize_t */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

int main(void)
{
    char data = '1';       /* Carácter que se guardará. */
    int fd;                /* Descriptor del archivo database.dat. */
    ssize_t escritos;     /* Cantidad de bytes escritos por write. */

    fd = open("database.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("No se pudo abrir database.dat");
        return EXIT_FAILURE;
    }

    escritos = write(fd, &data, sizeof(data));
    if (escritos != (ssize_t)sizeof(data)) {
        perror("No se pudo escribir el caracter");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Se escribio 1 bloque de %zu byte.\n", sizeof(data));

    if (close(fd) == -1) {
        perror("No se pudo cerrar database.dat");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}