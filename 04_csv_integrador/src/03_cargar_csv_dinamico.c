/*
 * Ejemplo 3: carga example.csv en un arreglo dinámico. Cada persona
 * posee sus propias copias de nombre y género; no conserva punteros
 * al buffer que reutiliza fgets.
 */

#include <errno.h>  /* errno y ERANGE */
#include <limits.h> /* INT_MAX e INT_MIN */
#include <stdio.h>  /* FILE, fopen, fgets, printf, fclose y perror */
#include <stdlib.h> /* malloc, realloc, free, strtol y constantes EXIT */
#include <string.h> /* strlen, memcpy, strcspn y strtok */

#define TAM_LINEA 256

typedef struct {
    int id;
    char *nombre;
    char *genero;
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

static char *duplicar_texto(const char *texto)
{
    char *copia;      /* Memoria reservada para la copia. */
    size_t longitud;  /* Caracteres de texto sin el terminador. */

    longitud = strlen(texto);
    copia = malloc(longitud + 1);
    if (copia != NULL) {
        memcpy(copia, texto, longitud + 1);
    }
    return copia;
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

static void liberar_persona(PERSONA *persona)
{
    free(persona->nombre);
    free(persona->genero);
    persona->nombre = NULL;
    persona->genero = NULL;
}

static int cargar_persona(char *linea, PERSONA *persona)
{
    char *id_texto;    /* Campo de identificador. */
    char *nombre;      /* Campo de nombre. */
    char *genero;      /* Campo de género. */
    char *campo_extra; /* Campo adicional, si existiera. */

    persona->nombre = NULL;
    persona->genero = NULL;
    id_texto = strtok(linea, ",");
    nombre = strtok(NULL, ",");
    genero = strtok(NULL, ",");
    campo_extra = strtok(NULL, ",");

    if (id_texto == NULL || nombre == NULL || genero == NULL ||
        campo_extra != NULL || !convertir_id(id_texto, &persona->id)) {
        return 0;
    }

    persona->nombre = duplicar_texto(nombre);
    persona->genero = duplicar_texto(genero);
    if (persona->nombre == NULL || persona->genero == NULL) {
        liberar_persona(persona);
        return 0;
    }

    return 1;
}

static void liberar_personas(PERSONA *personas, size_t cantidad)
{
    size_t i; /* Índice para liberar cada persona. */

    for (i = 0; i < cantidad; i++) {
        liberar_persona(&personas[i]);
    }
    free(personas);
}

int main(void)
{
    FILE *archivo;              /* Referencia al CSV abierto. */
    char linea[TAM_LINEA];      /* Buffer reutilizado para cada línea. */
    PERSONA persona;            /* Registro temporal recién interpretado. */
    PERSONA *personas;          /* Arreglo dinámico de personas. */
    PERSONA *personas_nuevas;   /* Resultado temporal de realloc. */
    size_t cantidad;            /* Personas cargadas. */
    size_t capacidad;           /* Espacio reservado en el arreglo. */
    size_t i;                   /* Índice para mostrar las personas. */
    unsigned long numero_linea; /* Número de línea actual. */

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

    personas = NULL;
    cantidad = 0;
    capacidad = 0;
    numero_linea = 1;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numero_linea++;

        if (strchr(linea, '\n') == NULL && descartar_resto_linea(archivo)) {
            printf("Línea %lu ignorada: supera %d caracteres.\n",
                   numero_linea, TAM_LINEA - 1);
            continue;
        }

        linea[strcspn(linea, "\r\n")] = '\0';

        if (!cargar_persona(linea, &persona)) {
            printf("Línea %lu ignorada: formato incorrecto o sin memoria.\n",
                   numero_linea);
            continue;
        }

        if (cantidad == capacidad) {
            capacidad = (capacidad == 0) ? 8 : capacidad * 2;
            personas_nuevas = realloc(personas, capacidad * sizeof(*personas));
            if (personas_nuevas == NULL) {
                perror("No se pudo ampliar el arreglo");
                liberar_persona(&persona);
                liberar_personas(personas, cantidad);
                fclose(archivo);
                return EXIT_FAILURE;
            }
            personas = personas_nuevas;
        }

        personas[cantidad] = persona;
        cantidad++;
    }

    if (ferror(archivo)) {
        perror("No se pudo leer example.csv");
        liberar_personas(personas, cantidad);
        fclose(archivo);
        return EXIT_FAILURE;
    }

    if (fclose(archivo) == EOF) {
        perror("No se pudo cerrar example.csv");
        liberar_personas(personas, cantidad);
        return EXIT_FAILURE;
    }

    printf("Se cargaron %zu personas.\n", cantidad);
    for (i = 0; i < cantidad; i++) {
        printf("%d | %s | %s\n",
               personas[i].id, personas[i].nombre, personas[i].genero);
    }

    liberar_personas(personas, cantidad);
    return EXIT_SUCCESS;
}
