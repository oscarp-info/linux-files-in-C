/*
 * Ejemplo 8: agrega una línea al final de un archivo usando el modo a.
 */

#include <stdio.h>  /* FILE, fopen, fputs, fclose y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

int main(void)
{
    FILE *fp; /* Referencia al archivo registro.txt. */

    fp = fopen("registro.txt", "a");
    if (fp == NULL) {
        perror("No se pudo abrir registro.txt");
        return EXIT_FAILURE;
    }

    if (fputs("Nueva ejecución del programa.\n", fp) == EOF) {
        perror("No se pudo agregar la línea");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar registro.txt");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}