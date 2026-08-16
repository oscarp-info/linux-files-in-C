/*
 * Ejemplo 4: guarda registros, los lee y busca uno por su identificador.
 */
#include <fcntl.h>      /* open y banderas como O_RDONLY, O_CREAT y O_TRUNC */
#include <stdio.h>      /* printf y perror */
#include <stdlib.h>    /* EXIT_SUCCESS y EXIT_FAILURE */
#include <sys/types.h> /* ssize_t y off_t */
#include <unistd.h>     /* read, write, lseek, close y descriptores estándar */

typedef struct {
    int id;
    char nombre[30];
    char genero;
} REGISTRO;

static void Escribir(void);
static void Leer(void);
static void BuscarPorId(int id);

int main(void)
{
    Escribir();
    Leer();
    BuscarPorId(3);

    return EXIT_SUCCESS;
}

static void Escribir(void)
{
    REGISTRO arr[3] = {  /* Registros que se guardarán en el archivo. */
        {1, "Juan", 'M'},
        {2, "Maria", 'F'},
        {3, "Pedro", 'M'}
    };
    int fd;               /* Descriptor del archivo listado.bin. */
    ssize_t escritos;    /* Cantidad de bytes escritos. */

    fd = open("listado.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("No se pudo abrir listado.bin");
        exit(EXIT_FAILURE);
    }

    escritos = write(fd, arr, sizeof(arr));
    if (escritos != (ssize_t)sizeof(arr)) {
        perror("No se pudieron escribir los registros");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("No se pudo cerrar listado.bin");
        exit(EXIT_FAILURE);
    }
}

static void Leer(void)
{
    REGISTRO arr[3];      /* Arreglo donde se recuperan los registros. */
    int fd;               /* Descriptor del archivo listado.bin. */
    ssize_t leidos;       /* Cantidad de bytes leídos. */
    size_t i;            /* Índice para recorrer el arreglo. */

    fd = open("listado.bin", O_RDONLY);
    if (fd == -1) {
        perror("No se pudo abrir listado.bin");
        exit(EXIT_FAILURE);
    }

    leidos = read(fd, arr, sizeof(arr));
    if (leidos != (ssize_t)sizeof(arr)) {
        perror("No se pudieron leer los registros");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("No se pudo cerrar listado.bin");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
        printf("%d | %s | %c\n", arr[i].id, arr[i].nombre, arr[i].genero);
}

static void BuscarPorId(int id)
{
    REGISTRO a;          /* Registro recuperado por la búsqueda. */
    int fd;               /* Descriptor del archivo listado.bin. */
    off_t posicion;      /* Desplazamiento del registro dentro del archivo. */
    ssize_t leidos;       /* Cantidad de bytes leídos. */

    fd = open("listado.bin", O_RDONLY);
    if (fd == -1) {
        perror("No se pudo abrir listado.bin");
        exit(EXIT_FAILURE);
    }

    posicion = (off_t)(id - 1) * (off_t)sizeof(REGISTRO);
    if (lseek(fd, posicion, SEEK_SET) == (off_t)-1) {
        perror("No se pudo buscar el registro");
        close(fd);
        exit(EXIT_FAILURE);
    }

    leidos = read(fd, &a, sizeof(a));
    if (leidos == 0) {
        printf("No encontrado\n");
    } else if (leidos != (ssize_t)sizeof(a)) {
        perror("No se pudo leer el registro");
        close(fd);
        exit(EXIT_FAILURE);
    } else {
        printf("%d | %s | %c\n", a.id, a.nombre, a.genero);
    }

    if (close(fd) == -1) {
        perror("No se pudo cerrar listado.bin");
        exit(EXIT_FAILURE);
    }
}