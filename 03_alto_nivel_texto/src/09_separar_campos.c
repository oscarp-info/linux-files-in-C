/*
 * Ejemplo 9: lee líneas de alumnos.txt y separa sus campos con strtok.
 */

#include <stdio.h>  /* FILE, fopen, fgets, printf, fclose y perror */
#include <stdlib.h> /* atoi, atof, EXIT_SUCCESS y EXIT_FAILURE */
#include <string.h> /* strtok y strncpy */

typedef struct {
    int legajo;
    char nombre[30];
    float nota;
} ALUMNO;

int main(void)
{
    FILE *fp;               /* Referencia al archivo alumnos.txt. */
    char linea[100];        /* Buffer donde se guarda cada línea leída. */
    char *campo;            /* Puntero al campo actual separado por strtok. */
    ALUMNO alumno;          /* Registro armado a partir de una línea. */

    fp = fopen("alumnos.txt", "r");
    if (fp == NULL) {
        perror("No se pudo abrir alumnos.txt");
        return EXIT_FAILURE;
    }

    while (fgets(linea, sizeof(linea), fp) != NULL) {
        campo = strtok(linea, ";");
        if (campo == NULL) {
            printf("Línea sin legajo.\n");
            continue;
        }
        alumno.legajo = atoi(campo);

        campo = strtok(NULL, ";");
        if (campo == NULL) {
            printf("Línea sin nombre.\n");
            continue;
        }
        strncpy(alumno.nombre, campo, sizeof(alumno.nombre) - 1);
        alumno.nombre[sizeof(alumno.nombre) - 1] = '\0';

        campo = strtok(NULL, ";\n");
        if (campo == NULL) {
            printf("Línea sin nota.\n");
            continue;
        }
        alumno.nota = (float)atof(campo);

        printf("Legajo %d | %s | %.1f\n",
               alumno.legajo, alumno.nombre, alumno.nota);
    }

    if (ferror(fp)) {
        perror("No se pudo leer alumnos.txt");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar alumnos.txt");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}