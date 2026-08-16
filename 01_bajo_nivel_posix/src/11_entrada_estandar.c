/*
 * Ejemplo 11: lee texto desde la entrada estándar y lo envía a la salida normal.
 */

#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h> /* ssize_t */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

int main(void)
{
    char buffer[128];      /* Espacio para el texto ingresado. */
    ssize_t leidos;       /* Cantidad de bytes leídos desde stdin. */
    ssize_t escritos;    /* Cantidad de bytes ya enviados a stdout. */
    ssize_t resultado;    /* Resultado de cada llamada a write. */

    printf("Escribí un texto y presioná Enter:\n");

    leidos = read(STDIN_FILENO, buffer, sizeof(buffer));
    if (leidos == -1) {
        perror("No se pudo leer la entrada estándar");
        return EXIT_FAILURE;
    }

    escritos = 0;
    while (escritos < leidos) {
        resultado = write(STDOUT_FILENO, buffer + escritos,
                                  (size_t)(leidos - escritos));

        if (resultado <= 0) {
            perror("No se pudo escribir la salida normal");
            return EXIT_FAILURE;
        }

        escritos += resultado;
    }

    return EXIT_SUCCESS;
}