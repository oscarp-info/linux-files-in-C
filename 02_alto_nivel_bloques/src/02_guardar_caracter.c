/*
 * Ejemplo 2: guarda un carácter en database.dat mediante fwrite.
 */

#include <stdio.h>  /* FILE, fopen, fwrite, fclose, printf y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

int main(void)
{
    char data = '1';       /* Carácter que se guardará. */
    FILE *fp;              /* Referencia al archivo database.dat. */
    size_t bloques;        /* Cantidad de bloques escritos por fwrite. */

    fp = fopen("database.dat", "wb");
    if (fp == NULL) {
        perror("No se pudo crear database.dat");
        return EXIT_FAILURE;
    }

    bloques = fwrite(&data, sizeof(data), 1, fp);
    if (bloques != 1) {
        perror("No se pudo escribir el carácter");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar database.dat");
        return EXIT_FAILURE;
    }

    printf("Se escribió 1 bloque de %zu byte.\n", sizeof(data));
    return EXIT_SUCCESS;
}