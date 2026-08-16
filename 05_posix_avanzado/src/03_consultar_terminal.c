/*
 * Ejemplo 3: consulta con ioctl las filas y columnas de la terminal actual.
 * La orden TIOCGWINSZ es específica de sistemas tipo Unix, como GNU/Linux.
 */

#include <stddef.h>      /* size_t */
#include <stdio.h>       /* perror y snprintf */
#include <stdlib.h>      /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/ioctl.h>   /* ioctl, TIOCGWINSZ y struct winsize */
#include <sys/types.h>   /* ssize_t */
#include <unistd.h>      /* isatty, write y STDOUT_FILENO */

static int escribir_todo(int fd, const char *buffer, size_t cantidad)
{
    size_t escritos;       /* Bytes enviados correctamente. */
    ssize_t resultado;     /* Resultado de cada llamada a write. */

    escritos = 0;
    while (escritos < cantidad) {
        resultado = write(fd, buffer + escritos, cantidad - escritos);
        if (resultado == -1) {
            return -1;
        }

        escritos += (size_t)resultado;
    }

    return 0;
}

int main(void)
{
    char mensaje[128];          /* Texto preparado para informar el resultado. */
    int cantidad;               /* Caracteres escritos por snprintf. */
    struct winsize tamanio;     /* Filas y columnas que informa la terminal. */

    if (!isatty(STDOUT_FILENO)) {
        if (escribir_todo(STDERR_FILENO,
                           "La salida estándar no está conectada a una terminal.\n",
                           sizeof("La salida estándar no está conectada a una terminal.\n") - 1) == -1) {
            perror("No se pudo escribir el diagnóstico");
        }

        return EXIT_FAILURE;
    }

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &tamanio) == -1) {
        perror("No se pudo consultar el tamaño de la terminal");
        return EXIT_FAILURE;
    }

    cantidad = snprintf(mensaje, sizeof(mensaje),
                        "Terminal: %u columnas x %u filas\n",
                        (unsigned int)tamanio.ws_col, (unsigned int)tamanio.ws_row);
    if (cantidad < 0 || (size_t)cantidad >= sizeof(mensaje)) {
        if (escribir_todo(STDERR_FILENO,
                           "No se pudo preparar el mensaje de salida.\n",
                           sizeof("No se pudo preparar el mensaje de salida.\n") - 1) == -1) {
            perror("No se pudo escribir el diagnóstico");
        }

        return EXIT_FAILURE;
    }

    if (escribir_todo(STDOUT_FILENO, mensaje, (size_t)cantidad) == -1) {
        perror("No se pudo informar el tamaño de la terminal");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}