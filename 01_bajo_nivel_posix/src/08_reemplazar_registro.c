/*
 * Ejemplo 8: usa lseek para reemplazar un registro dentro de un archivo.
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

static int escribir_alumno(int fd, const Alumno *alumno)
{
    return write(fd, alumno, sizeof(*alumno)) == (ssize_t)sizeof(*alumno) ? 0 : -1;
}

int main(void)
{
    Alumno curso[] = {{1001, 6.0f}, {1002, 7.0f}, {1003, 8.0f}}; /* Registros iniciales. */
    Alumno nuevo = {1002, 10.0f}; /* Registro que reemplazará al segundo. */
    Alumno alumno;                 /* Registro leído para mostrarlo. */
    const int indice = 1;         /* Índice del registro que se reemplazará. */
    int fd;                       /* Descriptor del archivo curso.dat. */
    off_t posicion;             /* Posición del registro dentro del archivo. */
    ssize_t leidos;            /* Cantidad de bytes leídos. */
    size_t i;                   /* Índice para recorrer los registros. */

    fd = open("curso.dat", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("No se pudo abrir curso.dat");
        return EXIT_FAILURE;
    }

    for (i = 0; i < sizeof(curso) / sizeof(curso[0]); i++) {
        if (escribir_alumno(fd, &curso[i]) == -1) {
            perror("No se pudo escribir un alumno");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    posicion = (off_t)indice * (off_t)sizeof(Alumno);
    if (lseek(fd, posicion, SEEK_SET) == (off_t)-1) {
        perror("No se pudo buscar el registro");
        close(fd);
        return EXIT_FAILURE;
    }

    if (escribir_alumno(fd, &nuevo) == -1) {
        perror("No se pudo reemplazar el alumno");
        close(fd);
        return EXIT_FAILURE;
    }

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        perror("No se pudo volver al comienzo");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Curso guardado en el archivo:\n");
    while ((leidos = read(fd, &alumno, sizeof(alumno))) == (ssize_t)sizeof(alumno))
        printf("Legajo %d, nota %.1f\n", alumno.legajo, alumno.nota);

    if (leidos == -1) {
        perror("No se pudo leer curso.dat");
        close(fd);
        return EXIT_FAILURE;
    }
    if (leidos != 0) {
        (void)write(STDERR_FILENO,
                    "El archivo termino con un registro incompleto.\n",
                    sizeof("El archivo termino con un registro incompleto.\n") - 1);
        close(fd);
        return EXIT_FAILURE;
    }
    if (close(fd) == -1) {
        perror("No se pudo cerrar curso.dat");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
