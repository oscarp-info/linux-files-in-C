# Archivos en C y Linux — Guía 5: POSIX avanzado, terminal y E/S bloqueante

**Informática 1 · UTN Electrónica**  
**Lenguaje:** C estándar + interfaz POSIX de GNU/Linux  
**Uso:** continuación de la guía de bajo nivel

---

## 1. Objetivos

Esta guía amplía el trabajo con descriptores de archivo hacia la terminal y otros recursos del sistema. Al finalizar deberías poder:

- reconocer cuándo una operación de entrada puede quedar esperando datos;
- consultar y modificar banderas de un descriptor con `fcntl`;
- usar `O_NONBLOCK` para evitar que una lectura espere indefinidamente;
- esperar datos disponibles con un tiempo límite mediante `select`;
- consultar una propiedad de la terminal con `ioctl`;
- distinguir qué partes pertenecen a POSIX y cuáles son específicas de GNU/Linux.

> **Requisito:** estos ejemplos deben ejecutarse en una terminal de GNU/Linux. No son programas de C estándar ni están pensados para ejecutarse desde un entorno que no ofrezca una terminal POSIX.

---

## 2. De archivos regulares a dispositivos

En la guía de bajo nivel se introdujo la idea de **“everything is a file”**: distintas clases de recursos se pueden manejar mediante descriptores y operaciones parecidas a `read`, `write` y `close`.

Un archivo regular conserva datos en el disco. La terminal, en cambio, es un dispositivo de entrada y salida. Ambos pueden usarse mediante un descriptor, pero no tienen exactamente el mismo comportamiento.

| Recurso | Ejemplo | ¿`read` puede esperar? | ¿`lseek` tiene sentido? |
|---|---|---:|---:|
| archivo regular | `notas.txt` | normalmente no | sí |
| terminal | `stdin` | sí | no |
| tubería | salida de otro programa | sí | no |
| dispositivo | terminal, cámara, audio | depende del dispositivo | generalmente no |

Un programa no debe suponer que todo descriptor permite reposicionarse con `lseek`. La función funciona con recursos que tienen una posición, como los archivos regulares; falla en recursos como la terminal o una tubería.

---

## 3. E/S bloqueante

Una operación es **bloqueante** cuando el programa queda detenido esperando que la operación pueda continuar. Por ejemplo, al ejecutar:

```c
read(STDIN_FILENO, buffer, sizeof(buffer));
```

la terminal normalmente espera hasta que la persona escriba una línea y presione Enter. Durante esa espera, el programa no avanza a la instrucción siguiente.

En un archivo regular, `read` suele devolver enseguida los bytes disponibles o `0` al llegar al final. En una terminal, una tubería o un dispositivo, puede ser necesario esperar a que otra persona, proceso o dispositivo produzca datos.

Esperar no es necesariamente un error: es el comportamiento esperado de muchas operaciones de entrada. El problema aparece si un programa necesita seguir haciendo otras tareas mientras espera.

---

## 4. Consultar y modificar banderas con `fcntl`

`fcntl` permite consultar o modificar ciertas propiedades de un descriptor que ya está abierto. Su forma general es:

```c
resultado = fcntl(fd, comando, ...);
```

Dos comandos habituales son:

| Comando | Uso |
|---|---|
| `F_GETFL` | obtiene las banderas de estado actuales del descriptor |
| `F_SETFL` | establece banderas de estado del descriptor |

La bandera `O_NONBLOCK` solicita que una operación no espere datos. Si se intenta leer y todavía no hay nada disponible, `read` devuelve `-1` y deja en `errno` un valor como `EAGAIN` o `EWOULDBLOCK`.

No se debe reemplazar sin más el conjunto de banderas actual. Primero se consulta y luego se agrega la nueva bandera con `|`:

```c
banderas = fcntl(STDIN_FILENO, F_GETFL);
fcntl(STDIN_FILENO, F_SETFL, banderas | O_NONBLOCK);
```

El ejemplo restaura las banderas originales antes de terminar. Esto es importante: la entrada estándar pertenece al entorno de la terminal y no conviene dejarla modificada para el programa siguiente.

### Ejemplo 1 — Lectura no bloqueante desde la entrada estándar

Archivo: [`01_lectura_no_bloqueante.c`](src/01_lectura_no_bloqueante.c)

```bash
gcc -Wall 01_lectura_no_bloqueante.c -o 01_lectura_no_bloqueante
./01_lectura_no_bloqueante
```

Si no había una línea lista para leer, el programa informa que no hay datos y termina sin esperar. Si había datos disponibles, los reproduce en la salida estándar.

---

## 5. Esperar una entrada con tiempo límite: `select`

Evitar el bloqueo no implica necesariamente revisar una y otra vez si llegaron datos. Eso desperdiciaría tiempo de CPU. Una alternativa es pedirle al sistema que espere hasta que un descriptor esté listo, pero sólo durante un tiempo máximo.

`select` recibe conjuntos de descriptores y puede esperar hasta que alguno esté listo para leer o escribir. Para este primer uso sólo se espera a `stdin`:

```c
select(STDIN_FILENO + 1, &lectura, NULL, NULL, &espera);
```

- Si devuelve un valor mayor que cero, algún descriptor está listo.
- Si devuelve `0`, se agotó el tiempo indicado.
- Si devuelve `-1`, ocurrió un error.

`select` no lee datos: sólo informa que una lectura podría realizarse sin esperar. Después se usa `read` normalmente.

### Ejemplo 2 — Esperar hasta cinco segundos una línea del teclado

Archivo: [`02_esperar_entrada.c`](src/02_esperar_entrada.c)

```bash
gcc -Wall 02_esperar_entrada.c -o 02_esperar_entrada
./02_esperar_entrada
```

Escribí una línea y presioná Enter antes de que venza el tiempo. Si no se ingresa nada, el programa informa el vencimiento y termina.

---

## 6. Consultar un dispositivo con `ioctl`

`ioctl` significa *input/output control*. Se usa para enviar órdenes o consultar propiedades que no se expresan simplemente como leer o escribir bytes. La forma y las órdenes disponibles dependen del tipo de recurso y del sistema operativo.

Por eso, a diferencia de las funciones básicas de archivos, `ioctl` no debe considerarse una interfaz portable de C estándar ni una receta universal para todos los dispositivos. Se consulta la documentación del dispositivo o subsistema correspondiente.

> **Alcance:** `fcntl`, `select` e `isatty` forman parte de POSIX. En cambio, `ioctl` y la orden `TIOCGWINSZ` pertenecen a las interfaces de Unix/GNU/Linux y no están garantizadas por POSIX ni por C estándar.

La terminal ofrece una orden habitual de GNU/Linux: `TIOCGWINSZ`. Permite obtener sus filas y columnas visibles.

```c
struct winsize tamanio;

ioctl(STDOUT_FILENO, TIOCGWINSZ, &tamanio);
```

### Ejemplo 3 — Consultar el tamaño de la terminal

Archivo: [`03_consultar_terminal.c`](src/03_consultar_terminal.c)

```bash
gcc -Wall 03_consultar_terminal.c -o 03_consultar_terminal
./03_consultar_terminal
```

El programa primero verifica con `isatty` que la salida esté conectada a una terminal. Si se redirige la salida a un archivo, no hay una ventana de terminal cuya dimensión se pueda consultar.

---

## 7. Dispositivos y audio: ejemplo opcional con OSS

Los dispositivos también pueden exponerse como archivos especiales, habitualmente dentro de `/dev`. Por ejemplo, la terminal se usa mediante los descriptores estándar y algunos sistemas ofrecen dispositivos como `/dev/null` o `/dev/zero`.

El ejemplo de esta sección usa la interfaz histórica **OSS** (*Open Sound System*) a través de `/dev/dsp`. Es útil para estudiar una idea importante: el mismo descriptor se puede configurar con `ioctl`, leer para capturar audio y escribir para reproducirlo.

> **Compatibilidad actual:** `/dev/dsp` no está garantizado en una instalación moderna de GNU/Linux. El kernel puede ofrecerlo mediante la emulación OSS de ALSA; si esa emulación no está habilitada, el dispositivo no existirá y el programa terminará mostrando el error de `open`. Esto no es un error del código ni debe resolverse con `sudo`: simplemente el equipo no ofrece esa interfaz.

Antes de compilar, se puede comprobar si el dispositivo está disponible:

```bash
ls -l /dev/dsp
```

### Ejemplo 4 — Capturar y reproducir audio con `/dev/dsp`

Archivo: [`04_audio_oss.c`](src/04_audio_oss.c)

```bash
gcc -Wall 04_audio_oss.c -o 04_audio_oss
./04_audio_oss
```

El programa solicita audio mono, sin compresión, de 8 bits y 8000 muestras por segundo. Captura dos segundos desde el dispositivo y luego los reproduce. La lectura bloquea mientras llegan las muestras; al final, `SOUND_PCM_SYNC` espera a que termine la reproducción antes de cerrar el descriptor.

La configuración se comprueba después de cada `ioctl`: si el dispositivo no acepta exactamente el formato solicitado, el programa se detiene en vez de interpretar los datos con un formato equivocado. También se verifican `malloc`, las lecturas parciales, las escrituras parciales y `close`.

> **Cuidado:** al ejecutarlo, se grabará lo que entre por la entrada de audio predeterminada y luego se enviará a la salida configurada por el sistema. Probalo con un volumen bajo y evitá auriculares o parlantes cerca del micrófono para prevenir realimentación.

La interfaz OSS permite abrir un único descriptor con `O_RDWR` sólo cuando captura y reproducción usan el mismo formato; este ejemplo usa exactamente la misma tasa, cantidad de canales y tamaño de muestra en ambas direcciones. Para aplicaciones actuales que requieran compatibilidad amplia con los sistemas de audio de escritorio, normalmente se usa una biblioteca específica de ALSA, PipeWire o PulseAudio en lugar de acceder directamente a `/dev/dsp`.

Referencias: [emulación OSS de ALSA en la documentación del kernel](https://docs.kernel.org/sound/designs/oss-emulation.html) y [configuración de ALSA/OSS](https://docs.kernel.org/sound/alsa-configuration.html).

---

## 8. Errores frecuentes

| Error | Consecuencia | Forma de evitarlo |
|---|---|---|
| asumir que `read` siempre devuelve enseguida | el programa puede quedar esperando entrada | considerar si el descriptor es una terminal, tubería o dispositivo |
| dejar `O_NONBLOCK` activado | cambia el comportamiento de lecturas posteriores | guardar y restaurar las banderas originales |
| tratar `EAGAIN` como un error definitivo | se informa un fallo cuando sólo faltaban datos | reconocerlo como “todavía no hay datos disponibles” |
| usar `select` y no revisar qué descriptor está listo | se puede leer del descriptor equivocado | usar `FD_ISSET` antes de llamar a `read` |
| usar `ioctl` sobre una salida redirigida | la consulta de la terminal puede fallar | comprobar antes con `isatty` |
| esperar que `ioctl` sea igual en todos los sistemas | el programa deja de ser portable | consultar la documentación de GNU/Linux y del dispositivo |
| suponer que existe `/dev/dsp` | el ejemplo de audio no puede abrirse | comprobar si el sistema ofrece emulación OSS |

---

## 9. Actividad y autoevaluación

1. Ejecutar `01_lectura_no_bloqueante.c` sin escribir nada. Luego ejecutar `printf "hola\n" | ./01_lectura_no_bloqueante` y comparar ambos resultados.
2. En `02_esperar_entrada.c`, cambiar el tiempo máximo de cinco a diez segundos. ¿Qué variable se modifica y en qué unidad está expresada?
3. Redirigir la salida de `03_consultar_terminal` con `> salida.txt`. Explicar por qué el programa indica que la salida no es una terminal.
4. Explicar la diferencia entre una lectura bloqueante y una no bloqueante.
5. ¿Por qué `EAGAIN` no significa necesariamente que el descriptor esté dañado?
6. Investigar qué dispositivo o interfaz de audio utiliza tu distribución antes de intentar escribir datos de audio desde un programa.

### Para recordar

```text
fcntl       → consulta o modifica propiedades de un descriptor abierto
O_NONBLOCK  → evita esperar indefinidamente por una operación de E/S
EAGAIN      → todavía no hay datos disponibles; se puede intentar más tarde
select      → espera uno o más descriptores con un tiempo límite
ioctl       → consulta o controla propiedades específicas de un dispositivo
isatty      → comprueba si un descriptor está conectado a una terminal
```

1. ¿Qué devuelve `read` si no hay datos y el descriptor usa `O_NONBLOCK`?
2. ¿Qué función permite esperar varios descriptores sin revisar constantemente cada uno?
3. ¿Qué se debe hacer con las banderas originales después de usar temporalmente `O_NONBLOCK`?
4. ¿Por qué `ioctl` requiere consultar documentación específica?

Respuestas sugeridas: `-1` y un valor como `EAGAIN` o `EWOULDBLOCK` en `errno`; `select`; restaurarlas con `fcntl`; porque las órdenes dependen del dispositivo y del sistema operativo.