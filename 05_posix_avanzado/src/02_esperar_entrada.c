/*
 * Ejemplo 2: espera hasta cinco segundos a que haya una línea disponible en stdin.
 * select informa si se puede llamar a read sin quedar bloqueado.
 */

#include <stddef.h>      /* size_t */
#include <stdio.h>       /* perror */
#include <stdlib.h>      /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/select.h>  /* select, fd_set y FD_* */
#include <sys/types.h>   /* ssize_t */
#include <unistd.h>      /* read, write y STDIN_FILENO */

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
    char buffer[128];         /* Datos obtenidos desde stdin. */
    fd_set lectura;           /* Conjunto de descriptores vigilados para lectura. */
    struct timeval espera;    /* Tiempo máximo que select debe esperar. */
    int listos;               /* Resultado de select. */
    ssize_t leidos;           /* Resultado de read después de select. */

    if (escribir_todo(STDOUT_FILENO,
                       "Escribí una línea antes de cinco segundos y presioná Enter:\n",
                       sizeof("Escribí una línea antes de cinco segundos y presioná Enter:\n") - 1) == -1) {
        perror("No se pudo escribir el aviso");
        return EXIT_FAILURE;
    }

    FD_ZERO(&lectura);
    FD_SET(STDIN_FILENO, &lectura);
    espera.tv_sec = 5;
    espera.tv_usec = 0;

    listos = select(STDIN_FILENO + 1, &lectura, NULL, NULL, &espera);
    if (listos == -1) {
        perror("No se pudo esperar la entrada");
        return EXIT_FAILURE;
    }

    if (listos == 0) {
        if (escribir_todo(STDOUT_FILENO,
                           "No llegó una línea dentro del tiempo indicado.\n",
                           sizeof("No llegó una línea dentro del tiempo indicado.\n") - 1) == -1) {
            perror("No se pudo escribir el resultado");
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    if (!FD_ISSET(STDIN_FILENO, &lectura)) {
        return EXIT_SUCCESS;
    }

    leidos = read(STDIN_FILENO, buffer, sizeof(buffer));
    if (leidos == -1) {
        perror("No se pudo leer stdin");
        return EXIT_FAILURE;
    }

    if (escribir_todo(STDOUT_FILENO, buffer, (size_t)leidos) == -1) {
        perror("No se pudo reproducir la entrada");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}