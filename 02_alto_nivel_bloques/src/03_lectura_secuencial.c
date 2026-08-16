/*
 * Ejemplo 3: lee hasta tres caracteres de database.dat de forma secuencial.
 */

#include <stdio.h>  /* FILE, fopen, fread, feof, ferror, fclose y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

int main(void)
{
    char data;             /* Carácter recuperado de cada lectura. */
    FILE *fp;              /* Referencia al archivo database.dat. */
    size_t bloques;        /* Cantidad de bloques leídos por fread. */
    int i;                 /* Contador de las tres lecturas. */

    fp = fopen("database.dat", "rb");
    if (fp == NULL) {
        perror("No se pudo abrir database.dat");
        return EXIT_FAILURE;
    }

    for (i = 0; i < 3; i++) {
        bloques = fread(&data, sizeof(data), 1, fp);
        if (bloques == 1) {
            printf("Se leyó 1 bloque de %zu byte: data = %c\n", sizeof(data), data);
        } else if (feof(fp)) {
            printf("Se llegó al final del archivo.\n");
            break;
        } else {
            perror("No se pudo leer database.dat");
            fclose(fp);
            return EXIT_FAILURE;
        }
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar database.dat");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}