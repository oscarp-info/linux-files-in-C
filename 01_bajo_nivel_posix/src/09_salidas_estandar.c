/*
 * Ejemplo 9: escribe mensajes en la salida normal y en la salida de errores.
 */
#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h> /* ssize_t */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

int main(void)
{
    const char resultado[] = "Resultado: se procesaron 3 alumnos.\n"; /* Mensaje para stdout. */
    const char advertencia[] = "Advertencia: este mensaje se envia a stderr.\n"; /* Mensaje para stderr. */

    if (write(STDOUT_FILENO, resultado, sizeof(resultado) - 1) !=
        (ssize_t)(sizeof(resultado) - 1)) {
        perror("No se pudo escribir la salida normal");
        return EXIT_FAILURE;
    }

    if (write(STDERR_FILENO, advertencia, sizeof(advertencia) - 1) !=
        (ssize_t)(sizeof(advertencia) - 1)) {
        perror("No se pudo escribir la advertencia");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}