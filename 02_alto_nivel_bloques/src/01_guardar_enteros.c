/*
 * Ejemplo 1: guarda dos enteros en database.dat mediante fwrite.
 */

#include <stdio.h>  /* FILE, fopen, fwrite, fclose, printf y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

int main(void)
{
    int data_01 = 1;       /* Primer entero que se guardará. */
    int data_02 = 2;       /* Segundo entero que se guardará. */
    FILE *fp;              /* Referencia al archivo database.dat. */
    size_t bloques;        /* Cantidad de bloques escritos por fwrite. */

    fp = fopen("database.dat", "wb");
    if (fp == NULL) {
        perror("No se pudo crear database.dat");
        return EXIT_FAILURE;
    }

    bloques = fwrite(&data_01, sizeof(data_01), 1, fp);
    if (bloques != 1) {
        perror("No se pudo escribir el primer entero");
        fclose(fp);
        return EXIT_FAILURE;
    }

    bloques = fwrite(&data_02, sizeof(data_02), 1, fp);
    if (bloques != 1) {
        perror("No se pudo escribir el segundo entero");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar database.dat");
        return EXIT_FAILURE;
    }

    printf("Se escribieron 2 bloques de %zu bytes cada uno.\n", sizeof(data_01));
    return EXIT_SUCCESS;
}