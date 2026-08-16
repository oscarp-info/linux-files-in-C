/*
 * Ejemplo 11: muestra la diferencia entre stdout y stderr.
 * La salida normal se envía a stdout y los mensajes de error a stderr.
 */

#include <stdio.h>  /* printf, fprintf, stdout y stderr */
#include <stdlib.h> /* EXIT_SUCCESS */

int main(void)
{
    printf("Resultado: el archivo se procesó correctamente.\n");
    fprintf(stderr, "Aviso: este mensaje se envía por stderr.\n");

    return EXIT_SUCCESS;
}