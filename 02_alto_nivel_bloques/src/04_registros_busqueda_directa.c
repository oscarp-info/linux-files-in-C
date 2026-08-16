/*
 * Ejemplo 4: guarda registros, los recupera y busca uno por su identificador.
 */

#include <stdio.h>  /* FILE, fopen, fread, fwrite, fseek, fclose y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

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
    REGISTRO arr[3] = {      /* Registros que se guardarán. */
        {1, "Juan", 'M'},
        {2, "Maria", 'F'},
        {3, "Pedro", 'M'}
    };
    FILE *fp;                /* Referencia al archivo listado.bin. */
    size_t bloques;          /* Cantidad de registros escritos. */

    fp = fopen("listado.bin", "w");
    if (fp == NULL) {
        perror("No se pudo crear listado.bin");
        exit(EXIT_FAILURE);
    }

    bloques = fwrite(arr, sizeof(REGISTRO), 3, fp);
    if (bloques != 3) {
        perror("No se pudieron escribir los registros");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar listado.bin");
        exit(EXIT_FAILURE);
    }
}

static void Leer(void)
{
    REGISTRO arr[3];         /* Arreglo donde se recuperan los registros. */
    FILE *fp;                /* Referencia al archivo listado.bin. */
    size_t bloques;          /* Cantidad de registros leídos. */
    size_t i;                /* Índice para recorrer el arreglo. */

    fp = fopen("listado.bin", "r");
    if (fp == NULL) {
        perror("No se pudo abrir listado.bin");
        exit(EXIT_FAILURE);
    }

    bloques = fread(arr, sizeof(REGISTRO), 3, fp);
    if (bloques != 3) {
        perror("No se pudieron leer los registros");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar listado.bin");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 3; i++)
        printf("%d | %s | %c\n", arr[i].id, arr[i].nombre, arr[i].genero);
}

static void BuscarPorId(int id)
{
    REGISTRO registro;      /* Registro recuperado por la búsqueda. */
    FILE *fp;               /* Referencia al archivo listado.bin. */
    size_t bloques;         /* Cantidad de registros leídos. */
    long posicion;          /* Posición del registro dentro del archivo. */

    fp = fopen("listado.bin", "r");
    if (fp == NULL) {
        perror("No se pudo abrir listado.bin");
        exit(EXIT_FAILURE);
    }

    posicion = (long)(id - 1) * (long)sizeof(REGISTRO);
    if (fseek(fp, posicion, SEEK_SET) != 0) {
        perror("No se pudo buscar el registro");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    bloques = fread(&registro, sizeof(REGISTRO), 1, fp);
    if (bloques == 0) {
        if (feof(fp))
            printf("No encontrado\n");
        else {
            perror("No se pudo leer el registro");
            fclose(fp);
            exit(EXIT_FAILURE);
        }
    } else {
        printf("%d | %s | %c\n", registro.id, registro.nombre, registro.genero);
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar listado.bin");
        exit(EXIT_FAILURE);
    }
}