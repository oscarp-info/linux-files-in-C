/*
 * Ejemplo 2: carga los registros de example.csv en un arreglo fijo
 * de estructuras PERSONA y luego los recorre desde la memoria.
 */

#include <errno.h>  /* errno y ERANGE */
#include <limits.h> /* INT_MAX e INT_MIN */
#include <stdio.h>  /* FILE, fopen, fgets, printf, fclose y perror */
#include <stdlib.h> /* strtol, EXIT_SUCCESS y EXIT_FAILURE */
#include <string.h> /* strcspn, strtok y strncpy */

#define TAM_LINEA 256
#define MAX_PERSONAS 100
#define TAM_NOMBRE 80
#define TAM_GENERO 30

typedef struct {
    int id;
    char nombre[TAM_NOMBRE];
    char genero[TAM_GENERO];
} PERSONA;

static int descartar_resto_linea(FILE *archivo)
{
    int caracter; /* Carácter leído mientras se descarta la fila. */

    caracter = fgetc(archivo);
    if (caracter == EOF) {
        return 0;
    }

    while (caracter != '\n' && caracter != EOF) {
        caracter = fgetc(archivo);
    }
    return 1;
}
static int convertir_id(const char *texto, int *id)
{
    char *fin;      /* Primer carácter no convertido. */
    long valor;     /* Resultado temporal de strtol. */

    errno = 0;
    valor = strtol(texto, &fin, 10);
    if (errno == ERANGE || *texto == '\0' || *fin != '\0' ||
        valor < INT_MIN || valor > INT_MAX) {
        return 0;
    }

    *id = (int)valor;
    return 1;
}

static int cargar_persona(char *linea, PERSONA *persona)
{
    char *id_texto;    /* Campo de identificador. */
    char *nombre;      /* Campo de nombre. */
    char *genero;      /* Campo de género. */
    char *campo_extra; /* Campo adicional, si existiera. */

    id_texto = strtok(linea, ",");
    nombre = strtok(NULL, ",");
    genero = strtok(NULL, ",");
    campo_extra = strtok(NULL, ",");

    if (id_texto == NULL || nombre == NULL || genero == NULL ||
        campo_extra != NULL || !convertir_id(id_texto, &persona->id)) {
        return 0;
    }

    strncpy(persona->nombre, nombre, sizeof(persona->nombre) - 1);
    persona->nombre[sizeof(persona->nombre) - 1] = '\0';
    strncpy(persona->genero, genero, sizeof(persona->genero) - 1);
    persona->genero[sizeof(persona->genero) - 1] = '\0';

    return 1;
}

int main(void)
{
    FILE *archivo;                      /* Referencia al CSV abierto. */
    char linea[TAM_LINEA];              /* Buffer para una línea. */
    PERSONA personas[MAX_PERSONAS];     /* Arreglo que guarda las personas. */
    size_t cantidad;                    /* Personas cargadas correctamente. */
    size_t i;                           /* Índice para recorrer el arreglo. */
    unsigned long numero_linea;         /* Número de línea actual. */

    archivo = fopen("example.csv", "r");
    if (archivo == NULL) {
        perror("No se pudo abrir example.csv");
        return EXIT_FAILURE;
    }

    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        printf("El archivo no contiene un encabezado.\n");
        fclose(archivo);
        return EXIT_FAILURE;
    }
    if (strchr(linea, '\n') == NULL && descartar_resto_linea(archivo)) {
        printf("El encabezado supera %d caracteres.\n", TAM_LINEA - 1);
        fclose(archivo);
        return EXIT_FAILURE;
    }

    cantidad = 0;
    numero_linea = 1;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numero_linea++;

        if (strchr(linea, '\n') == NULL && descartar_resto_linea(archivo)) {
            printf("Línea %lu ignorada: supera %d caracteres.\n",
                   numero_linea, TAM_LINEA - 1);
            continue;
        }

        linea[strcspn(linea, "\r\n")] = '\0';

        if (cantidad == MAX_PERSONAS) {
            printf("Se alcanzó el máximo de %d personas.\n", MAX_PERSONAS);
            break;
        }

        if (!cargar_persona(linea, &personas[cantidad])) {
            printf("Línea %lu ignorada: formato incorrecto.\n", numero_linea);
            continue;
        }

        cantidad++;
    }

    if (ferror(archivo)) {
        perror("No se pudo leer example.csv");
        fclose(archivo);
        return EXIT_FAILURE;
    }

    if (fclose(archivo) == EOF) {
        perror("No se pudo cerrar example.csv");
        return EXIT_FAILURE;
    }

    printf("Se cargaron %zu personas.\n", cantidad);
    for (i = 0; i < cantidad; i++) {
        printf("%d | %s | %s\n",
               personas[i].id, personas[i].nombre, personas[i].genero);
    }

    return EXIT_SUCCESS;
}
