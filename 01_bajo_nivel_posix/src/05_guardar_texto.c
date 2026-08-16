/*
 * Ejemplo 5: crea mensaje.txt y escribe una línea de texto.
 */
#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <sys/types.h> /* ssize_t */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

int main(void)
{
    const char mensaje[] = "Hola, archivos.\n"; /* Texto que se guardará. */
    int fd;                /* Descriptor del archivo mensaje.txt. */
    ssize_t escritos;     /* Cantidad de bytes escritos. */

    fd = open("mensaje.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("No se pudo abrir mensaje.txt");
        return EXIT_FAILURE;
    }

    escritos = write(fd, mensaje, sizeof(mensaje) - 1);
    if (escritos != (ssize_t)(sizeof(mensaje) - 1)) {
        if (escritos == -1) perror("No se pudo escribir mensaje.txt");
        else
            (void)write(STDERR_FILENO,
                        "La escritura quedo incompleta.\n",
                        sizeof("La escritura quedo incompleta.\n") - 1);
        close(fd);
        return EXIT_FAILURE;
    }

    if (close(fd) == -1) {
        perror("No se pudo cerrar mensaje.txt");
        return EXIT_FAILURE;
    }

    printf("Archivo creado: se escribieron %zd bytes.\n", escritos);
    return EXIT_SUCCESS;
}
