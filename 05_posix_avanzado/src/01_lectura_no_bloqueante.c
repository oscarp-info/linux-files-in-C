/*
 * Ejemplo 1: consulta temporalmente stdin en modo no bloqueante.
 * Si no hay una línea disponible, read no espera y devuelve EAGAIN.
 */

#include <errno.h>      /* EAGAIN y EWOULDBLOCK */
#include <fcntl.h>      /* fcntl, F_GETFL, F_SETFL y O_NONBLOCK */
#include <stddef.h>     /* size_t */
#include <stdio.h>      /* perror */
#include <stdlib.h>     /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h>  /* ssize_t */
#include <unistd.h>     /* read, write y descriptores estándar */

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
    char buffer[128];          /* Datos leídos desde la entrada estándar. */
    int banderas_originales;   /* Banderas que stdin tenía al comenzar. */
    int estado;                /* Resultado de restaurar las banderas. */
    ssize_t leidos;            /* Cantidad de bytes obtenidos por read. */

    if (escribir_todo(STDOUT_FILENO,
                       "Se revisará stdin sin esperar datos.\n",
                       sizeof("Se revisará stdin sin esperar datos.\n") - 1) == -1) {
        perror("No se pudo escribir el aviso");
        return EXIT_FAILURE;
    }

    banderas_originales = fcntl(STDIN_FILENO, F_GETFL);
    if (banderas_originales == -1) {
        perror("No se pudieron consultar las banderas de stdin");
        return EXIT_FAILURE;
    }

    if (fcntl(STDIN_FILENO, F_SETFL, banderas_originales | O_NONBLOCK) == -1) {
        perror("No se pudo activar O_NONBLOCK");
        return EXIT_FAILURE;
    }

    leidos = read(STDIN_FILENO, buffer, sizeof(buffer));

    estado = fcntl(STDIN_FILENO, F_SETFL, banderas_originales);
    if (estado == -1) {
        perror("No se pudieron restaurar las banderas de stdin");
        return EXIT_FAILURE;
    }

    if (leidos == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (escribir_todo(STDOUT_FILENO,
                               "No hay datos disponibles; el programa no esperó.\n",
                               sizeof("No hay datos disponibles; el programa no esperó.\n") - 1) == -1) {
                perror("No se pudo escribir el resultado");
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

        perror("No se pudo leer stdin");
        return EXIT_FAILURE;
    }

    if (escribir_todo(STDOUT_FILENO, buffer, (size_t)leidos) == -1) {
        perror("No se pudo reproducir la entrada");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}