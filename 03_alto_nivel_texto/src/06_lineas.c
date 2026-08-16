/*
 * Ejemplo 6: escribe líneas con fputs y las recupera con fgets.
 */

#include <stdio.h>  /* FILE, fopen, fputs, fgets, ferror y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

int main(void)
{
    FILE *fp;          /* Referencia al archivo lineas.txt. */
    char linea[80];    /* Buffer donde se almacena cada línea leída. */

    fp = fopen("lineas.txt", "w");
    if (fp == NULL) {
        perror("No se pudo crear lineas.txt");
        return EXIT_FAILURE;
    }

    if (fputs("Primera línea.\n", fp) == EOF ||
        fputs("Segunda línea.\n", fp) == EOF ||
        fputs("Tercera línea.\n", fp) == EOF) {
        perror("No se pudo escribir una línea");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar lineas.txt");
        return EXIT_FAILURE;
    }

    fp = fopen("lineas.txt", "r");
    if (fp == NULL) {
        perror("No se pudo abrir lineas.txt");
        return EXIT_FAILURE;
    }

    while (fgets(linea, sizeof(linea), fp) != NULL)
        printf("Leída: %s", linea);

    if (ferror(fp)) {
        perror("No se pudo leer lineas.txt");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar lineas.txt");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}