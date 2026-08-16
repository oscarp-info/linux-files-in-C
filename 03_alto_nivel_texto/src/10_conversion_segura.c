/*
 * Ejemplo 10: convierte texto a números validando el resultado con strtol y strtof.
 */

#include <errno.h>  /* errno para detectar errores de conversión. */
#include <limits.h> /* INT_MAX para validar el rango de un int. */
#include <stdio.h>  /* printf y perror */
#include <stdlib.h> /* strtol, strtof, EXIT_SUCCESS y EXIT_FAILURE */

int main(int argc, char *argv[])
{
    char *fin;              /* Primer carácter que no pudo convertirse. */
    long legajo_largo;      /* Legajo antes de verificar que entre en int. */
    int legajo;             /* Legajo convertido y validado. */
    float nota;             /* Nota convertida y validada. */

    if (argc != 3) {
        printf("Uso: %s LEGAJO NOTA\n", argv[0]);
        return EXIT_FAILURE;
    }

    errno = 0;
    legajo_largo = strtol(argv[1], &fin, 10);
    if (errno != 0 || *fin != '\0' || legajo_largo < 1 ||
        legajo_largo > INT_MAX) {
        printf("Legajo inválido.\n");
        return EXIT_FAILURE;
    }
    legajo = (int)legajo_largo;

    errno = 0;
    nota = strtof(argv[2], &fin);
    if (errno != 0 || *fin != '\0') {
        printf("Nota inválida.\n");
        return EXIT_FAILURE;
    }

    printf("Legajo válido: %d\n", legajo);
    printf("Nota válida: %.1f\n", nota);
    return EXIT_SUCCESS;
}