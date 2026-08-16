/*
 * Ejemplo 7: guarda una estructura Alumno y luego la recupera del archivo.
 */
#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <sys/types.h> /* ssize_t y off_t */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

typedef struct {
    int legajo;
    float nota;
} Alumno;

int main(void)
{
    Alumno original = {12345, 8.5f}; /* Estructura que se guardará. */
    Alumno recuperado;             /* Estructura leída desde el archivo. */
    int fd;                         /* Descriptor del archivo alumno.dat. */
    ssize_t cantidad;              /* Bytes leídos o escritos. */

    fd = open("alumno.dat", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("No se pudo abrir alumno.dat");
        return EXIT_FAILURE;
    }

    cantidad = write(fd, &original, sizeof(original));
    if (cantidad != (ssize_t)sizeof(original)) {
        if (cantidad == -1) perror("No se pudo escribir el alumno");
        else
            (void)write(STDERR_FILENO,
                        "La escritura quedo incompleta.\n",
                        sizeof("La escritura quedo incompleta.\n") - 1);
        close(fd);
        return EXIT_FAILURE;
    }

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("No se pudo volver al comienzo");
        close(fd);
        return EXIT_FAILURE;
    }

    cantidad = read(fd, &recuperado, sizeof(recuperado));
    if (cantidad != (ssize_t)sizeof(recuperado)) {
        if (cantidad == -1) perror("No se pudo leer el alumno");
        else
            (void)write(STDERR_FILENO,
                        "No se leyo una estructura completa.\n",
                        sizeof("No se leyo una estructura completa.\n") - 1);
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Legajo: %d\nNota: %.1f\n", recuperado.legajo, recuperado.nota);

    if (close(fd) == -1) {
        perror("No se pudo cerrar alumno.dat");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
