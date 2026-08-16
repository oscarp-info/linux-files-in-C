/*
 * Ejemplo 6: abre mensaje.txt y lee su contenido en un buffer.
 */
#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <sys/types.h> /* ssize_t */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

int main(void)
{
    char buffer[128];      /* Espacio donde se guarda el texto leído. */
    int fd;                /* Descriptor del archivo mensaje.txt. */
    ssize_t leidos;       /* Cantidad de bytes leídos. */

    fd = open("mensaje.txt", O_RDONLY);
    if (fd == -1) {
        perror("No se pudo abrir mensaje.txt para lectura");
        return EXIT_FAILURE;
    }

    leidos = read(fd, buffer, sizeof(buffer) - 1);
    if (leidos == -1) {
        perror("No se pudo leer mensaje.txt");
        close(fd);
        return EXIT_FAILURE;
    }

    buffer[leidos] = '\0';
    printf("Se leyeron %zd bytes:\n%s", leidos, buffer);

    if (close(fd) == -1) {
        perror("No se pudo cerrar mensaje.txt");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
