/*
 * Ejemplo 4: captura dos segundos de audio y los reproduce mediante /dev/dsp.
 * Requiere la interfaz OSS disponible; muchos Linux actuales no la habilitan.
 */

#include <errno.h>          /* EIO */
#include <fcntl.h>          /* open y O_RDWR */
#include <stddef.h>         /* size_t */
#include <stdio.h>          /* perror */
#include <stdlib.h>         /* malloc, free, EXIT_SUCCESS y EXIT_FAILURE */
#include <string.h>         /* strlen */
#include <sys/ioctl.h>      /* ioctl */
#include <sys/soundcard.h>  /* SOUND_PCM_* */
#include <sys/types.h>      /* ssize_t */
#include <unistd.h>         /* read, write y close */

#define DURACION_SEGUNDOS 2U  /* Tiempo que se captura antes de reproducir. */
#define TASA_MUESTREO 8000U   /* Muestras por segundo. */
#define BITS_POR_MUESTRA 8U   /* Tamaño de cada muestra, en bits. */
#define CANTIDAD_CANALES 1U   /* Un canal: audio mono. */

static int escribir_todo(int fd, const char *buffer, size_t cantidad)
{
    size_t escritos;       /* Bytes enviados correctamente. */
    ssize_t resultado;     /* Resultado de cada llamada a write. */

    escritos = 0;
    while (escritos < cantidad) {
        resultado = write(fd, buffer + escritos, cantidad - escritos);
        if (resultado <= 0) {
            return -1;
        }

        escritos += (size_t)resultado;
    }

    return 0;
}

static int leer_todo(int fd, unsigned char *buffer, size_t cantidad)
{
    size_t leidos;         /* Bytes capturados correctamente. */
    ssize_t resultado;     /* Resultado de cada llamada a read. */

    leidos = 0;
    while (leidos < cantidad) {
        resultado = read(fd, buffer + leidos, cantidad - leidos);
        if (resultado <= 0) {
            if (resultado == 0) {
                errno = EIO;
            }

            return -1;
        }

        leidos += (size_t)resultado;
    }

    return 0;
}

static int informar_texto(int fd, const char *texto)
{
    return escribir_todo(fd, texto, strlen(texto));
}

static int configurar_audio(int fd)
{
    int valor;             /* Valor solicitado y luego informado por el dispositivo. */

    valor = (int)BITS_POR_MUESTRA;
    if (ioctl(fd, SOUND_PCM_WRITE_BITS, &valor) == -1) {
        perror("No se pudo configurar el tamaño de muestra");
        return -1;
    }

    if (valor != (int)BITS_POR_MUESTRA) {
        informar_texto(STDERR_FILENO,
                       "El dispositivo no admite exactamente 8 bits por muestra.\n");
        return -1;
    }

    valor = (int)CANTIDAD_CANALES;
    if (ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &valor) == -1) {
        perror("No se pudo configurar la cantidad de canales");
        return -1;
    }

    if (valor != (int)CANTIDAD_CANALES) {
        informar_texto(STDERR_FILENO,
                       "El dispositivo no admite exactamente un canal.\n");
        return -1;
    }

    valor = (int)TASA_MUESTREO;
    if (ioctl(fd, SOUND_PCM_WRITE_RATE, &valor) == -1) {
        perror("No se pudo configurar la tasa de muestreo");
        return -1;
    }

    if (valor != (int)TASA_MUESTREO) {
        informar_texto(STDERR_FILENO,
                       "El dispositivo no admite exactamente 8000 muestras por segundo.\n");
        return -1;
    }

    return 0;
}

int main(void)
{
    unsigned char *buffer;  /* Reserva que guarda las muestras capturadas. */
    size_t cantidad;        /* Bytes necesarios para los dos segundos de audio. */
    int fd;                 /* Descriptor del dispositivo OSS. */
    int estado_cierre;      /* Resultado de close. */

    cantidad = (size_t)DURACION_SEGUNDOS * TASA_MUESTREO *
               (BITS_POR_MUESTRA / 8U) * CANTIDAD_CANALES;

    fd = open("/dev/dsp", O_RDWR);
    if (fd == -1) {
        perror("No se pudo abrir /dev/dsp");
        return EXIT_FAILURE;
    }

    if (configurar_audio(fd) == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    buffer = malloc(cantidad);
    if (buffer == NULL) {
        perror("No se pudo reservar memoria para el audio");
        close(fd);
        return EXIT_FAILURE;
    }

    if (informar_texto(STDOUT_FILENO,
                       "Grabando dos segundos; hablá cerca del micrófono.\n") == -1) {
        perror("No se pudo escribir el aviso");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }

    if (leer_todo(fd, buffer, cantidad) == -1) {
        perror("No se pudo capturar el audio");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }

    if (informar_texto(STDOUT_FILENO, "Reproduciendo el audio capturado.\n") == -1) {
        perror("No se pudo escribir el aviso");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }

    if (escribir_todo(fd, (const char *)buffer, cantidad) == -1) {
        perror("No se pudo reproducir el audio");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }

    if (ioctl(fd, SOUND_PCM_SYNC, 0) == -1) {
        perror("No se pudo esperar el final de la reproducción");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }

    free(buffer);
    estado_cierre = close(fd);
    if (estado_cierre == -1) {
        perror("No se pudo cerrar /dev/dsp");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}