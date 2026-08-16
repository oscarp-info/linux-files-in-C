/*
 * Ejemplo 1: lee example.csv línea por línea y muestra los tres campos
 * de cada registro usando fgets y strtok.
 */

#include <stdio.h>  /* FILE, fopen, fgets, printf, fclose y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */
#include <string.h> /* strcspn y strtok */

#define TAM_LINEA 256

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
int main(void)
{
    FILE *archivo;              /* Referencia al archivo CSV abierto. */
    char linea[TAM_LINEA];      /* Buffer para una línea del archivo. */
    char *id;                   /* Primer campo de la línea. */
    char *nombre;               /* Segundo campo de la línea. */
    char *genero;               /* Tercer campo de la línea. */
    char *campo_extra;          /* Campo adicional, si existiera. */
    unsigned long numero_linea; /* Número de línea para informar errores. */

    archivo = fopen("example.csv", "r");
    if (archivo == NULL) {
        perror("No se pudo abrir example.csv");
        return EXIT_FAILURE;
    }

    numero_linea = 0;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numero_linea++;

        if (strchr(linea, '\n') == NULL && descartar_resto_linea(archivo)) {
            printf("Línea %lu ignorada: supera %d caracteres.\n",
                   numero_linea, TAM_LINEA - 1);
            continue;
        }

        linea[strcspn(linea, "\r\n")] = '\0';

        if (numero_linea == 1) {
            printf("Encabezado: %s\n", linea);
            continue;
        }

        id = strtok(linea, ",");
        nombre = strtok(NULL, ",");
        genero = strtok(NULL, ",");
        campo_extra = strtok(NULL, ",");

        if (id == NULL || nombre == NULL || genero == NULL ||
            campo_extra != NULL) {
            printf("Línea %lu con formato incorrecto.\n", numero_linea);
            continue;
        }

        printf("ID: %s | Nombre: %s | Género: %s\n", id, nombre, genero);
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

    return EXIT_SUCCESS;
}
