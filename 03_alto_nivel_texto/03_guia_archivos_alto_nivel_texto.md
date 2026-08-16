# Archivos en C — Guía 3: alto nivel y archivos de texto

**Informática 1 · UTN Electrónica**  
**Lenguaje:** C estándar · **Uso:** guía para estudiantes

---

## 1. Objetivos

Esta guía continúa el trabajo con `FILE *`, pero se enfoca en archivos de texto. Al finalizar deberías poder escribir y leer caracteres, líneas y datos con formato.

Las funciones principales son:

| Función | Uso |
|---|---|
| `fputc` / `fgetc` | escribir y leer un carácter |
| `fputs` / `fgets` | escribir y leer texto por líneas |
| `fprintf` / `fscanf` | escribir y leer datos con formato |
| `ferror` | comprobar si ocurrió un error de lectura o escritura |

---

## 2. Texto y datos guardados como bytes

Un archivo de texto guarda caracteres codificados como bytes. Por ejemplo, el texto:

```text
Hola
```

contiene los bytes de `H`, `o`, `l`, `a` y el salto de línea `\n`.

Se puede visualizar con:

```bash
cat archivo.txt
hexdump -C archivo.txt
```

`cat` muestra el texto interpretado; `hexdump -C` permite observar los bytes que lo forman.

---

## 3. Ejemplo 5 — Caracteres con `fputc` y `fgetc`

Archivo: [`05_caracteres.c`](src/05_caracteres.c)

```bash
gcc -Wall 05_caracteres.c -o 05_caracteres
./05_caracteres
cat caracteres.txt
```

`fputc` escribe un único carácter:

```c
fputc('H', fp);
```

`fgetc` lee un carácter y devuelve su valor como `int`:

```c
int caracter;

caracter = fgetc(fp);
```

La variable debe ser `int`, no `char`, porque además de todos los caracteres posibles `fgetc` necesita poder devolver `EOF` al llegar al final del archivo.

El patrón correcto de lectura es:

```c
while ((caracter = fgetc(fp)) != EOF) {
    putchar(caracter);
}
```

---

## 4. Ejemplo 6 — Líneas con `fputs` y `fgets`

Archivo: [`06_lineas.c`](src/06_lineas.c)

```bash
gcc -Wall 06_lineas.c -o 06_lineas
./06_lineas
cat lineas.txt
```

`fputs` escribe una cadena de texto. No agrega un salto de línea automáticamente; si se necesita, debe incluirse `\n` en la cadena:

```c
fputs("Primera línea.\n", fp);
```

`fgets` lee hasta completar una línea, hasta llegar al final del archivo o hasta llenar el buffer menos un carácter. Si lee correctamente, agrega el terminador nulo al final de la cadena.

```c
char linea[80];

while (fgets(linea, sizeof(linea), fp) != NULL) {
    printf("%s", linea);
}
```

Si `fgets` devuelve `NULL`, se debe usar `feof(fp)` y `ferror(fp)` para distinguir final de archivo y error.

---

## 5. Ejemplo 7 — Datos formateados con `fprintf` y `fscanf`

Archivo: [`07_datos_formateados.c`](src/07_datos_formateados.c)

```bash
gcc -Wall 07_datos_formateados.c -o 07_datos_formateados
./07_datos_formateados
cat alumnos.txt
```

`fprintf` escribe texto siguiendo un formato, como `printf`, pero enviándolo al archivo:

```c
fprintf(fp, "%d;%s;%.1f\n", legajo, nombre, nota);
```

El ejemplo utiliza `;` como separador de campos. El archivo queda legible como texto:

```text
1001;Juan;8.5
1002;Maria;9.0
```

`fscanf` interpreta texto de acuerdo con un formato. Su retorno indica cuántos campos pudo recuperar. Si se esperan tres campos, se debe comprobar que devuelva `3`:

```c
campos = fscanf(fp, "%d;%29[^;];%f", &legajo, nombre, &nota);
```

`%29[^;]` lee como máximo 29 caracteres que no sean `;`, dejando espacio para el terminador nulo de `nombre`.

---

## 6. Ejemplo 8 — Agregar líneas con el modo `"a"`

Archivo: [`08_agregar_lineas.c`](src/08_agregar_lineas.c)

```bash
gcc -Wall 08_agregar_lineas.c -o 08_agregar_lineas
./08_agregar_lineas
./08_agregar_lineas
cat registro.txt
```

El modo `"a"` abre el archivo para agregar datos al final. Si el archivo no existe, lo crea. Si ya existe, conserva el contenido anterior.

Cada ejecución agrega una línea nueva. Comparalo con el modo `"w"`, que vacía el archivo al abrirlo.

---

## 7. Ejemplo 9 — Separar campos con `strtok`

Archivo: [`09_separar_campos.c`](src/09_separar_campos.c)

Ejecutar primero `07_datos_formateados` para crear `alumnos.txt`. Luego:

```bash
gcc -Wall 09_separar_campos.c -o 09_separar_campos
./09_separar_campos
```

`strtok` separa una cadena en partes llamadas **tokens**. No lee archivos por sí misma: primero se usa `fgets` para guardar una línea en un buffer y luego `strtok` divide ese texto.

Para una línea como esta:

```text
1001;Juan;8.5
```

las llamadas son:

```c
campo = strtok(linea, ";");      /* "1001" */
campo = strtok(NULL, ";");       /* "Juan" */
campo = strtok(NULL, ";\n");     /* "8.5" */
```

La primera llamada recibe la cadena que se quiere separar. Las llamadas siguientes reciben `NULL` para continuar sobre la misma cadena.

El ejemplo convierte los campos de texto con `atoi` y `atof` y los guarda en una estructura `ALUMNO`.

> **Importante:** `strtok` modifica el buffer original: reemplaza cada separador encontrado por el carácter nulo `\0`. Por eso debe usarse sobre un arreglo que pueda modificarse, como `char linea[100]`, y no directamente sobre una cadena literal.

`strtok` es adecuado para datos sencillos separados por `;`. No es una solución completa para CSV con campos vacíos o con comillas, por ejemplo:

```text
1001;"Juan; Pérez";8.5
```

---
## 8. Ejemplo 10 — Conversiones seguras con `strtol` y `strtof`

Archivo: [`10_conversion_segura.c`](src/10_conversion_segura.c)

El ejemplo 9 usa `atoi` y `atof` porque permiten convertir de forma simple un campo de texto a un número:

```c
alumno.legajo = atoi(campo);
alumno.nota = (float)atof(campo);
```

Estas funciones son útiles para comenzar, pero tienen una limitación: si el texto no representa un número válido, no permiten distinguir claramente el error de un valor numérico `0`.

`strtol` y `strtof` permiten comprobar la conversión. El programa recibe un legajo y una nota como argumentos:

```bash
gcc -Wall 10_conversion_segura.c -o 10_conversion_segura
./10_conversion_segura 1001 8.5
```

También se puede probar un dato incorrecto:

```bash
./10_conversion_segura hola 8.5
```

`strtol` convierte texto a `long` y `strtof` convierte texto a `float`. Ambas funciones reciben un puntero llamado `fin`: al terminar, ese puntero indica el primer carácter que no pudo convertirse.

```c
legajo_largo = strtol(texto, &fin, 10);

if (*fin != '\0') {
    /* El texto incluía caracteres no numéricos. */
}
```

Además, el programa comprueba `errno` para detectar valores fuera de rango antes de convertir el legajo a `int`.

> Para una lectura sencilla de archivos de texto, `atoi` y `atof` pueden ser suficientes. Cuando se necesita validar los datos ingresados o leídos desde un archivo, conviene usar `strtol` y `strtof`.

---

## 9. Ejemplo 11 — `stdout`, `stderr` y redirección

Archivo: [`11_stdout_stderr.c`](src/11_stdout_stderr.c)

Además de los archivos abiertos con `fopen`, un programa en C recibe tres flujos ya abiertos. Los tres son de tipo `FILE *`:

| Flujo | Uso habitual | Destino predeterminado |
|---|---|---|
| `stdin` | entrada del programa | teclado |
| `stdout` | salida normal | terminal |
| `stderr` | mensajes de error o aviso | terminal |

No se deben abrir con `fopen` ni cerrar con `fclose`: ya existen al comenzar `main` y el entorno se ocupa de cerrarlos al finalizar normalmente el programa.

`printf` escribe en `stdout`. Por eso estas dos llamadas tienen el mismo destino:

```c
printf("Proceso terminado.\n");
fprintf(stdout, "Proceso terminado.\n");
```

Para un error o aviso se usa `stderr`:

```c
fprintf(stderr, "No se pudo abrir el archivo.\n");
```

El ejemplo escribe un mensaje por cada flujo:

```bash
gcc -Wall 11_stdout_stderr.c -o 11_stdout_stderr
./11_stdout_stderr
```

Desde la consola se puede redirigir cada flujo por separado:

```bash
./11_stdout_stderr > salida.txt
./11_stdout_stderr 2> errores.txt
./11_stdout_stderr > salida.txt 2> errores.txt
```

`>` redirige `stdout`; `2>` redirige `stderr`. En el último comando, el mensaje normal queda en `salida.txt` y el aviso queda en `errores.txt`.

Para comparar con bajo nivel:

| Alto nivel con `FILE *` | Bajo nivel POSIX |
|---|---|
| `stdin` | `STDIN_FILENO` |
| `stdout` | `STDOUT_FILENO` |
| `stderr` | `STDERR_FILENO` |
| `printf` o `fprintf` | `write` |

> Elegir `stderr` para errores permite separar los resultados normales de los diagnósticos, incluso cuando la salida normal se guarda en un archivo.

---

## 10. Errores frecuentes

| Error | Consecuencia | Prevención |
|---|---|---|
| guardar una línea con `fputs` sin `\n` | las líneas quedan unidas | agregar `\n` cuando corresponda |
| declarar el resultado de `fgetc` como `char` | no se distingue correctamente `EOF` | usar una variable `int` |
| usar `while (!feof(fp))` | se intenta procesar una lectura que no fue correcta | controlar el retorno de `fgetc`, `fgets` o `fscanf` |
| no limitar `%s` o `%[...]` en `fscanf` | se puede desbordar el arreglo | indicar un ancho máximo, como `%29[^;]` |
| usar `"w"` cuando se quiere conservar contenido | se borra el archivo anterior | usar `"a"` para agregar al final |
| enviar un error con printf | se mezcla con la salida normal redirigida | usar fprintf(stderr, ...) |

---

## 11. Actividad y autoevaluación

1. Modificar `05_caracteres.c` para escribir y leer una palabra elegida por vos, carácter por carácter.
2. En `06_lineas.c`, agregar una cuarta línea y aumentar o reducir el tamaño del buffer. ¿Qué ocurre si una línea es más larga que el buffer?
3. Ejecutar `07_datos_formateados.c`, editar manualmente una línea de `alumnos.txt` y observar qué pasa si falta un `;`.
4. Modificar `08_agregar_lineas.c` para que agregue una línea con tu nombre. Ejecutarlo tres veces y verificar el resultado con `cat registro.txt`.
5. Usar `hexdump -C alumnos.txt` y comparar los bytes con el contenido mostrado por `cat`.
6. Ejecutar `09_separar_campos` después de crear `alumnos.txt` con el ejemplo 7. Agregar una línea y comprobar que `strtok` separa sus tres campos.
7. Ejecutar `10_conversion_segura` con valores válidos e inválidos. Explicar por qué `strtol` puede detectar un legajo incorrecto y `atoi` no.
8. Ejecutar 11_stdout_stderr con > salida.txt 2> errores.txt. Abrir ambos archivos y explicar por qué cada mensaje quedó en un archivo distinto.

### Para recordar

```text
fputc / fgetc    → un carácter
fputs / fgets    → una línea o cadena de texto
fprintf / fscanf → datos con formato
"w"              → crea o vacía
"a"              → agrega al final
EOF              → final de archivo o indicador de error en algunas funciones
stdout / stderr  → salida normal / mensajes de error
```

1. ¿Por qué `fgetc` devuelve `int`?
2. ¿Qué debe devolver `fscanf` si recuperó correctamente tres campos?
3. ¿Cuál es la diferencia entre `"w"` y `"a"`?
4. ¿Qué función permite comprobar si falló una lectura?
5. ¿Qué flujo se redirige con 2>?

Respuestas sugeridas: para poder devolver `EOF`; `3`; `"w"` vacía o crea y `"a"` conserva y agrega; `ferror`; `stderr`.