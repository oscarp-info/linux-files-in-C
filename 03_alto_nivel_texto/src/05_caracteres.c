/*
 * Ejemplo 5: escribe caracteres con fputc y los recupera con fgetc.
 */

#include <stdio.h>  /* FILE, fopen, fputc, fgetc, putchar y perror */
#include <stdlib.h> /* EXIT_SUCCESS y EXIT_FAILURE */

int main(void)
{
    FILE *fp;       /* Referencia al archivo caracteres.txt. */
    int caracter;   /* Carácter leído; debe ser int para poder comparar con EOF. */

    fp = fopen("caracteres.txt", "w");
    if (fp == NULL) {
        perror("No se pudo crear caracteres.txt");
        return EXIT_FAILURE;
    }

    if (fputc('H', fp) == EOF || fputc('o', fp) == EOF ||
        fputc('l', fp) == EOF || fputc('a', fp) == EOF ||
        fputc('\n', fp) == EOF) {
        perror("No se pudo escribir un carácter");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar caracteres.txt");
        return EXIT_FAILURE;
    }

    fp = fopen("caracteres.txt", "r");
    if (fp == NULL) {
        perror("No se pudo abrir caracteres.txt");
        return EXIT_FAILURE;
    }

    while ((caracter = fgetc(fp)) != EOF) {
        if (putchar(caracter) == EOF) {
            perror("No se pudo mostrar un carácter");
            fclose(fp);
            return EXIT_FAILURE;
        }
    }

    if (ferror(fp)) {
        perror("No se pudo leer caracteres.txt");
        fclose(fp);
        return EXIT_FAILURE;
    }

    if (fclose(fp) == EOF) {
        perror("No se pudo cerrar caracteres.txt");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}