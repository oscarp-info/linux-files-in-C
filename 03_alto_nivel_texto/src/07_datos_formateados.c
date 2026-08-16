/*
 * Ejemplo 7: guarda y recupera datos de texto con fprintf y fscanf.
 */

#include <stdio.h>  /* FILE, fprintf, fscanf, printf y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

int main(void)
{
    FILE *fp;             /* Referencia al archivo alumnos.txt. */
    int legajo;           /* Legajo leído desde el archivo. */
    char nombre[30];      /* Nombre leído desde el archivo. */
    float nota;           /* Nota leída desde el archivo. */
    int campos;           /* Cantidad de campos recuperados por fscanf. */

    fp = fopen("alumnos.txt", "w");
    if (fp == NULL) {
        perror("No se pudo crear alumnos.txt");
        return EXIT_FAILURE;
    }

    if (fprintf(fp, "1001;Juan;8.5\n") < 0 ||
        fprintf(fp, "1002;Maria;9.0\n") < 0 ||
        fprintf(fp, "1003;Pedro;7.5\n") < 0) {
        perror("No se pudieron escribir los datos");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar alumnos.txt");
        return EXIT_FAILURE;
    }

    fp = fopen("alumnos.txt", "r");
    if (fp == NULL) {
        perror("No se pudo abrir alumnos.txt");
        return EXIT_FAILURE;
    }

    while ((campos = fscanf(fp, "%d;%29[^;];%f", &legajo, nombre, &nota)) == 3) {
        printf("Legajo %d | %s | %.1f\n", legajo, nombre, nota);
    }

    if (campos != EOF) {
        printf("Se encontró una línea con formato incorrecto.\n");
        fclose(fp);
        return EXIT_FAILURE;
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