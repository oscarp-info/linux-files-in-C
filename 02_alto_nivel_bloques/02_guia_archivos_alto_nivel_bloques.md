# Archivos en C — Guía 2: alto nivel con `FILE *`

**Informática 1 · UTN Electrónica**  
**Lenguaje:** C estándar · **Uso:** guía para estudiantes

---

## 1. Objetivos

En esta guía se trabajará con archivos mediante las funciones de la biblioteca estándar de C. Al finalizar deberías poder:

- abrir y cerrar un archivo con `fopen` y `fclose`;
- guardar y recuperar enteros, caracteres y estructuras con `fwrite` y `fread`;
- recorrer un archivo de forma secuencial;
- reposicionar el cursor con `fseek`;
- conocer la posición actual con `ftell`;
- buscar un registro de tamaño fijo mediante acceso directo.

En esta interfaz, un archivo abierto se representa con un puntero de tipo `FILE *`:

```c
FILE *fp;
```

`fp` no contiene los datos del archivo. Es una referencia que la biblioteca de C utiliza para trabajar con el archivo abierto.

---

## 2. La interfaz de alto nivel

Las funciones principales de esta guía son:

| Función | Acción | Devuelve |
|---|---|---|
| `fopen` | abre o crea un archivo | `FILE *` o `NULL` |
| `fwrite` | escribe bloques de bytes | cantidad de bloques escritos |
| `fread` | lee bloques de bytes | cantidad de bloques leídos |
| `fseek` | cambia la posición actual | `0` o un valor distinto de `0` |
| `ftell` | informa la posición actual | posición o `-1L` |
| `fclose` | cierra el archivo | `0` o `EOF` |

A diferencia de `read` y `write`, `fread` y `fwrite` trabajan con **bloques**. Por eso reciben cuatro argumentos:

```c
fwrite(direccion, tamano_de_cada_bloque, cantidad_de_bloques, fp);
fread(direccion, tamano_de_cada_bloque, cantidad_de_bloques, fp);
```

Por ejemplo:

```c
fwrite(&dato, sizeof(dato), 1, fp);
```

indica: “escribir un bloque de tamaño `sizeof(dato)` desde la dirección `&dato`”. El resultado es la cantidad de bloques efectivamente escritos; si todo salió bien, debe ser `1`.

---

## 3. Abrir archivos con `fopen`

La forma general es:

```c
fp = fopen("nombre_del_archivo", "modo");
```

Siempre se debe verificar que el resultado no sea `NULL`:

```c
if (fp == NULL) {
    perror("No se pudo abrir el archivo");
    return EXIT_FAILURE;
}
```

### Modos de apertura

| Modo | Acción |
|---|---|
| `"r"` | abre un archivo existente sólo para lectura |
| `"w"` | abre para escritura; crea el archivo o borra su contenido si ya existe |
| `"a"` | abre para agregar datos al final; crea el archivo si no existe |
| `"r+"` | abre un archivo existente para lectura y escritura |
| `"w+"` | abre para lectura y escritura; crea o vacía el archivo |
| `"a+"` | abre o crea para lectura y agregado al final |

> **Importante:** un archivo abierto con `"r"` no permite escribir. Para usar `fwrite`, se necesita un modo de escritura, como `"w"`, `"a"`, `"w+"` o `"r+"`.

---

## 4. Ejemplo 1 — Guardar enteros

Archivo: [`01_guardar_enteros.c`](src/01_guardar_enteros.c)

El objetivo del ejemplo es guardar valores `int` en `database.dat`.

Para escribir, la apertura debe ser:

```c
fp = fopen("database.dat", "w");
```

Luego se guardan los enteros con:

```c
r = fwrite(&data_01, sizeof(data_01), 1, fp);
```

Si `r` vale `1`, se escribió un bloque completo. Para guardar un segundo entero, se repite la operación:

```c
r = fwrite(&data_02, sizeof(data_02), 1, fp);
```

Después de cada `fwrite`, la posición actual avanza automáticamente. Si se guardan dos enteros consecutivos, el archivo contiene los bytes del primero seguidos por los del segundo.

Para inspeccionarlos:

```bash
hexdump -C database.dat
```

> El ejemplo abre `database.dat` con `"w"`: crea el archivo o vacía su contenido anterior antes de guardar los dos enteros.

---

## 5. Ejemplo 2 — Guardar un carácter

Archivo: [`02_guardar_caracter.c`](src/02_guardar_caracter.c)

El programa guarda el carácter `'1'` en `database.dat`:

```c
char data = '1';
r = fwrite(&data, sizeof(data), 1, fp);
```

Como un `char` ocupa un byte, el archivo tendrá un único byte. El modo `"w"` usado por el ejemplo crea el archivo si no existe y, si existe, descarta su contenido anterior.

Podés verificarlo con:

```bash
hexdump -C database.dat
```

El valor hexadecimal `31` corresponde al carácter ASCII `'1'`.

---

## 6. Ejemplo 3 — Lectura secuencial

Archivo: [`03_lectura_secuencial.c`](src/03_lectura_secuencial.c)

Este ejemplo usa tres llamadas consecutivas a `fread`:

```c
r = fread(&data, sizeof(data), 1, fp);
```

Cada lectura comienza donde terminó la anterior. Esto es **acceso secuencial**.

`fread` devuelve la cantidad de bloques leídos. Si se pidió un bloque y devuelve `1`, la lectura fue correcta. Si devuelve `0`, puede haberse llegado al final del archivo o haber ocurrido un error. Para distinguir esos casos se usan:

```c
feof(fp);    /* indica si se llegó al final */
ferror(fp);  /* indica si ocurrió un error */
```

Para probar tres lecturas correctas, preparar el archivo antes de ejecutar el programa:

```bash
printf "123" > database.dat
./03_files
```

La salida debe mostrar, en orden, los caracteres `1`, `2` y `3`.

---

## 7. Posición actual, `fseek` y `ftell`

Un archivo abierto tiene una posición actual. Cada `fread` o `fwrite` la desplaza según la cantidad de bytes procesados.

`fseek` permite mover esa posición:

```c
fseek(fp, desplazamiento, referencia);
```

| Referencia | Punto desde el que se cuenta |
|---|---|
| `SEEK_SET` | inicio del archivo |
| `SEEK_CUR` | posición actual |
| `SEEK_END` | final del archivo |

Para volver al inicio:

```c
fseek(fp, 0, SEEK_SET);
```

También se puede usar la función equivalente y más expresiva:

```c
rewind(fp);
```

`ftell` informa la posición actual:

```c
long posicion;

posicion = ftell(fp);
```

Para obtener el tamaño de un archivo, se puede buscar el final y consultar la posición:

```c
long tamanio;

fseek(fp, 0, SEEK_END);
tamanio = ftell(fp);
fseek(fp, 0, SEEK_SET);
```

> **Portabilidad:** los desplazamientos por cantidad de bytes, como los usados para buscar registros, son apropiados para los archivos de datos de estos ejemplos. Si se trabaja con un archivo de texto y se busca mantener C estándar portable, lo más seguro es volver al inicio con `rewind` o `fseek(fp, 0, SEEK_SET)`, o reposicionarse con una posición obtenida previamente mediante `ftell`.

---

## 8. Ejemplo 4 — Registros y búsqueda directa por ID

Archivo: [`04_registros_busqueda_directa.c`](src/04_registros_busqueda_directa.c)

El ejemplo define una estructura `REGISTRO` con identificador, nombre y género. Luego guarda un arreglo de tres registros en `listado.bin`:

```c
fwrite(arr, sizeof(REGISTRO), 3, fp);
```

Como todos los registros tienen el mismo tamaño, el registro de índice `i` comienza en:

```text
posición = i × sizeof(REGISTRO)
```

Si el identificador comienza en `1` y coincide con la posición lógica del registro, para buscar el ID `3` se usa:

```c
fseek(fp, sizeof(REGISTRO) * (id - 1), SEEK_SET);
fread(&a, sizeof(REGISTRO), 1, fp);
```

Esto es **acceso directo**: no hace falta leer los registros anteriores para llegar al buscado.

> **Alcance del ejemplo:** guardar una estructura directamente es práctico para aprender y para usarla desde el mismo programa. No es un formato pensado para intercambiar datos entre computadoras o versiones distintas: el tamaño de los tipos, el relleno interno de la estructura y la representación de los números pueden variar.

Para observar el contenido guardado:

```bash
hexdump -C listado.bin
```

> El programa ejecuta primero `Escribir()`, luego `Leer()` y finalmente `BuscarPorId(3)`, por lo que puede ejecutarse directamente.

---

## 9. Cerrar y manejar errores

Al terminar siempre se debe cerrar el archivo:

```c
if (fclose(fp) == EOF) {
    perror("No se pudo cerrar el archivo");
    return EXIT_FAILURE;
}
```

Las funciones de alto nivel trabajan mediante una estructura `FILE *` que puede tener buffers internos. `fclose` vacía la información pendiente, libera los recursos asociados y finaliza el uso del archivo.

### Buffers y `fflush`

Al escribir mediante `FILE *`, la biblioteca de C puede conservar temporalmente los datos en un **buffer** de memoria antes de enviarlos al archivo. Esto evita muchas escrituras pequeñas y puede mejorar el rendimiento.

Normalmente alcanza con cerrar el archivo, porque `fclose` escribe los datos pendientes antes de finalizar. Si se necesita forzar la escritura antes de cerrar, se usa:

```c
if (fflush(fp) == EOF) {
    perror("No se pudo actualizar el archivo");
}
```

También hay una regla importante para los archivos abiertos con `"r+"`, `"w+"` o `"a+"`: si se alterna entre leer y escribir, debe hacerse un `fseek`, `rewind` o `fflush` entre ambas operaciones. Así se sincroniza el buffer con la posición del archivo.

Esta regla distingue a `FILE *` de un descriptor de bajo nivel. Con un descriptor abierto con `O_RDWR`, `read` y `write` comparten directamente una única posición actual. Con `FILE *`, la biblioteca puede conservar datos leídos o escritos en sus buffers; por eso se necesita la operación de sincronización antes de cambiar de dirección.

```text
Bajo nivel:  read(fd, ...)  →  write(fd, ...)  usa la misma posición del descriptor
Alto nivel:  fread(...)     →  fseek, rewind o fflush  →  fwrite(...)
```

---

## 10. Errores frecuentes

| Error | Consecuencia | Prevención |
|---|---|---|
| abrir con `"r"` e intentar `fwrite` | la escritura falla | usar `"w"`, `"a"`, `"r+"` o `"w+"` |
| abrir con `"w"` sin querer | se pierde el contenido anterior | usar `"a"` para agregar o `"r+"` para conservar |
| no verificar `fp == NULL` | se usa un archivo no abierto | comprobar el resultado de `fopen` |
| ignorar el retorno de `fread` o `fwrite` | datos incompletos | comparar la cantidad de bloques obtenida |
| usar `fread` como si agregara `\0` | texto mal terminado | agregar el terminador sólo al leer texto |
| buscar sin `fseek` | se lee desde otra posición | calcular y mover a la posición correcta |
| no llamar a `fclose` | datos pendientes o recursos abiertos | cerrar cada `FILE *` abierto |

---

## 11. Alto nivel y bajo nivel: comparación

Las dos interfaces permiten trabajar con archivos, pero lo hacen con distintos niveles de abstracción.

| Alto nivel | Bajo nivel |
|---|---|
| usa `FILE *` | usa un descriptor de archivo de tipo `int` |
| `fopen`, `fread`, `fwrite`, `fseek`, `fclose` | `open`, `read`, `write`, `lseek`, `close` |
| trabaja con bloques y buffers de la biblioteca de C | trabaja directamente con bytes y descriptores |
| `fread` y `fwrite` devuelven cantidad de bloques | `read` y `write` devuelven cantidad de bytes |
| forma parte de la biblioteca estándar de C | pertenece a la interfaz POSIX de sistemas tipo Unix |
| usa modos como `"r"`, `"w"` y `"a"` | usa banderas como `O_RDONLY`, `O_WRONLY` y `O_CREAT` |

### Ventajas y límites

**Alto nivel**

- El código suele ser más corto y legible.
- La biblioteca administra buffers internos automáticamente.
- Es una buena opción para programas que trabajan con archivos comunes.
- Al estar basado en la biblioteca estándar de C, resulta más portable.
- Ofrece menos control directo sobre los descriptores y recursos del sistema operativo.

**Bajo nivel**

- Permite trabajar directamente con descriptores de archivo.
- Puede usar la misma interfaz para archivos, terminal, dispositivos, tuberías y otros recursos de Unix/Linux.
- Da un control más preciso sobre los bytes, la posición y los modos de apertura.
- Requiere verificar más detalles y escribir código más extenso.
- Depende de POSIX, por lo que no pertenece sólo al C estándar.

### ¿Cuál conviene usar?

Un **archivo regular** es el tipo de archivo habitual que se guarda en el sistema de archivos: por ejemplo, `notas.txt`, `personas.dat` o `listado.bin`. No es un directorio, una terminal, un dispositivo, una tubería ni un socket.

Para elegir una interfaz, conviene mirar qué necesita el programa:

| Situación | Interfaz recomendada | Motivo |
|---|---|---|
| leer o escribir un archivo regular de texto | alto nivel | la biblioteca de C ofrece buffers y operaciones orientadas a texto |
| guardar o recuperar arreglos y estructuras en un archivo regular | alto nivel | `fread` y `fwrite` permiten trabajar por bloques de forma directa |
| escribir un programa que dependa sólo de C estándar | alto nivel | `FILE *` y sus funciones pertenecen a la biblioteca estándar de C |
| controlar permisos, crear exclusivamente o elegir banderas de apertura | bajo nivel | `open` permite usar opciones como `O_CREAT`, `O_EXCL`, `O_APPEND` y permisos como `0644` |
| trabajar directamente con terminal, entrada, salida o errores | bajo nivel | se usan los descriptores `STDIN_FILENO`, `STDOUT_FILENO` y `STDERR_FILENO` |
| trabajar con tuberías, sockets o dispositivos | bajo nivel | esos recursos se manejan naturalmente con descriptores POSIX |
| necesitar controlar con precisión cada lectura, escritura y posición | bajo nivel | `read`, `write` y `lseek` exponen directamente esas operaciones |

La interfaz de alto nivel no es “peor” ni necesariamente más lenta: sus buffers internos pueden hacer eficiente el trabajo con archivos regulares. Bajo nivel no se elige por ser “más rápido”, sino cuando se necesita control directo de la interfaz POSIX.

Las dos interfaces pueden convivir en un mismo programa, pero no conviene alternar sin cuidado operaciones de ambas sobre el mismo archivo abierto: los buffers de `FILE *` pueden dejar datos pendientes o una posición distinta de la esperada.

---

## 12. Actividad y autoevaluación

1. Ejecutar `01_guardar_enteros.c`, guardar los dos enteros e inspeccionar `database.dat` con `hexdump -C`.
2. Ejecutar `02_guardar_caracter.c`, observar el byte guardado y explicar por qué reemplaza el contenido anterior de `database.dat`.
3. Crear un archivo con `printf "123" > database.dat` y ejecutar `03_lectura_secuencial.c`. Agregar una verificación con `feof` y `ferror` cuando `fread` devuelve `0`.
4. Seguir el recorrido de `04_registros_busqueda_directa.c`: guardar, leer y buscar el registro de ID `3`. Luego buscar un ID inexistente.
5. Agregar un cuarto registro en `04_registros_busqueda_directa.c` y buscarlo mediante su ID.
6. Usar `fseek` y `ftell` para informar el tamaño de `listado.bin`.
7. Reemplazar la apertura `"w"` por `"a"` en un programa de prueba y ejecutar dos veces. Explicar el resultado.
8. **Práctica integradora:** crear un programa que trabaje con un archivo de `REGISTRO` de tamaño fijo. Debe guardar cuatro registros, listarlos mediante lecturas secuenciales, buscar uno por ID usando `fseek` y modificar el registro encontrado escribiéndolo en la misma posición. Verificar el cambio listando nuevamente el archivo.

### Para recordar

```text
FILE *  → referencia a un archivo abierto
fopen   → abre un archivo según un modo
fwrite  → memoria hacia archivo, por bloques
fread   → archivo hacia memoria, por bloques
fseek   → mueve la posición actual
ftell   → informa la posición actual
fclose  → termina el uso del archivo
```

1. ¿Qué devuelve `fopen` si no puede abrir un archivo?
2. ¿Qué valor debe devolver `fwrite(..., 1, fp)` si escribió un bloque completo?
3. ¿Cuál es la diferencia entre `"w"` y `"a"`?
4. ¿Qué función permite volver al inicio del archivo?
5. ¿Qué relación hay entre `sizeof(REGISTRO)` y la búsqueda directa?

Respuestas sugeridas: `NULL`; `1`; `"w"` crea o vacía, mientras que `"a"` agrega al final; `fseek(fp, 0, SEEK_SET)` o `rewind(fp)`; permite calcular la posición de cada registro.

---

