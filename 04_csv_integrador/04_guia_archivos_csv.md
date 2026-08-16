# Archivos en C — Guía 4: CSV como ejercicio integrador

**Informática 1 · UTN Electrónica**  
**Lenguaje:** C estándar · **Uso:** guía para estudiantes

---

## 1. Objetivos

En esta guía se integran archivos de texto, estructuras, arreglos, punteros y memoria dinámica. Al finalizar deberías poder:

- reconocer la organización básica de un archivo CSV;
- leer el archivo una línea por vez con `fgets`;
- separar los campos de una línea con `strtok`;
- convertir y validar un identificador de texto a `int`;
- detectar filas que superan la capacidad del buffer de lectura;
- cargar registros en un arreglo de estructuras;
- comprender por qué no se deben conservar punteros al buffer usado por `fgets`;
- ampliar un arreglo dinámico con `realloc` y liberar toda la memoria reservada.

Los ejemplos usan [`example.csv`](src/example.csv), que contiene personas con tres campos: `id`, `name` y `gender`.

---

## 2. ¿Qué es un CSV?

CSV significa *Comma-Separated Values*: valores separados por comas. Es un formato de texto sencillo para representar una tabla. Cada línea suele representar un registro y las comas separan sus campos.

```text
id,name,gender
1,Albina Thunders,Female
2,Randie Carde,Polygender
```

La primera línea se llama **encabezado**: nombra las columnas. Las siguientes son los datos.

Aunque se llama CSV, el separador puede ser otro, como `;` o `|`. Lo importante es que el programa conozca cuál se está usando.

> En esta guía se trabaja con un CSV simple: tres campos, separados por comas, sin comas dentro de los datos y sin comillas. Un CSV completo puede incluir campos vacíos, comillas y comas dentro de un nombre; `strtok` no alcanza para analizar correctamente esos casos.

---

## 3. Recorrido general

Para procesar el archivo se sigue siempre esta idea:

```text
abrir → leer una línea → quitar el salto de línea → separar campos
     → validar y guardar → repetir → cerrar
```

El archivo se lee de manera **secuencial**: `fgets` comienza en la posición actual y la avanza hasta la línea siguiente.

---

## 4. Leer una línea completa con `fgets`

La forma general es:

```c
char linea[256];

while (fgets(linea, sizeof(linea), archivo) != NULL) {
    /* Procesar linea. */
}
```

`fgets` guarda, como máximo, `sizeof(linea) - 1` caracteres y agrega el terminador nulo `\0`. Si encuentra un salto de línea antes, también lo guarda. Por eso una línea leída puede verse así en memoria:

```text
"1,Albina Thunders,Female\n\0"
```

Antes de separar campos, conviene quitar `\n` y también `\r`, que puede aparecer en archivos creados con Windows:

```c
linea[strcspn(linea, "\r\n")] = '\0';
```

`strcspn` devuelve la posición del primer `\r` o `\n`; si no encuentra ninguno, devuelve la longitud de la cadena. Por lo tanto, esta asignación es segura en ambos casos.

> El buffer debe ser suficientemente grande para la línea más larga esperada. Si una línea supera el tamaño del buffer, `fgets` la leerá en varias partes y ya no representará un registro completo.

Para evitar interpretar esas partes como si fueran registros distintos, los ejemplos comprueban si la lectura terminó con `\n`. Si no lo hizo, leen un carácter más: si ya se alcanzó el final del archivo, la última fila es válida; si hay otro carácter, la fila supera la capacidad del buffer. La función siguiente descarta los caracteres restantes hasta el próximo salto de línea e informa si había texto pendiente:

```c
static int descartar_resto_linea(FILE *archivo)
{
    int caracter;

    caracter = fgetc(archivo);
    if (caracter == EOF) {
        return 0; /* Era la última línea, sin salto final. */
    }

    while (caracter != '\n' && caracter != EOF) {
        caracter = fgetc(archivo);
    }
    return 1; /* Había más caracteres en la misma fila. */
}
```

La verificación se realiza antes de procesar los tokens:

```c
if (strchr(linea, '\n') == NULL && descartar_resto_linea(archivo)) {
    /* Informar y continuar con la próxima fila. */
    continue;
}
```

Así, una última línea sin salto de línea se procesa como una fila válida.

---

## 5. Separar campos con `strtok`

Luego de leer una línea, se pueden obtener sus tres campos:

```c
char *id;
char *nombre;
char *genero;

id = strtok(linea, ",");
nombre = strtok(NULL, ",");
genero = strtok(NULL, ",");
```

La primera llamada recibe el arreglo que se desea separar. Las siguientes reciben `NULL` para continuar con el mismo texto.

`strtok` **modifica** el arreglo: reemplaza los separadores por `\0` y devuelve punteros a zonas dentro del mismo buffer.

```text
Antes:   1,Albina Thunders,Female\0
Después: 1\0Albina Thunders\0Female\0
         ↑  ↑                ↑
         id nombre           genero
```

Por ese motivo, `id`, `nombre` y `genero` no son copias independientes: apuntan a `linea`.

Para comprobar que se recibieron exactamente tres campos se obtiene un cuarto token:

```c
char *campo_extra;

campo_extra = strtok(NULL, ",");
if (id == NULL || nombre == NULL || genero == NULL || campo_extra != NULL) {
    /* Línea inválida. */
}
```

---

## 6. Ejemplo 1 — Recorrer y mostrar el CSV

Archivo: [`01_recorrer_csv.c`](src/01_recorrer_csv.c)

Este primer ejemplo no almacena datos. Sólo muestra el encabezado y los tres campos de cada línea válida. Sirve para observar el recorrido del archivo y el efecto de `strtok`.

```bash
gcc -Wall 01_recorrer_csv.c -o 01_recorrer_csv
./01_recorrer_csv
```

Antes de procesar registros, el programa omite la primera línea porque es el encabezado:

```c
if (numero_linea == 1) {
    printf("Encabezado: %s\n", linea);
    continue;
}
```

No se compara el contenido del primer campo con `"id"`: se toma como regla del formato que la primera línea es el encabezado. Así, un dato cuyo ID fuera texto no se confundiría con esa regla.

---

## 7. De texto a número: validar el ID

El campo leído desde un CSV siempre es texto. Por ejemplo, el token `"25"` es una cadena y todavía no es un `int`.

`atoi` permite una conversión breve:

```c
persona.id = atoi(id_texto);
```

Pero ante un dato como `"abc"` no permite distinguir con claridad un error del valor numérico `0`. En estos ejemplos se utiliza `strtol`, que informa dónde terminó la conversión y permite detectar valores fuera del rango de `int`.

```c
errno = 0;
valor = strtol(texto, &fin, 10);

if (errno == ERANGE || *texto == '\0' || *fin != '\0') {
    /* El identificador no es válido. */
}
```

El segundo argumento, `fin`, queda apuntando al primer carácter no convertido. Si `*fin` no es `\0`, quedaron caracteres sin interpretar, por ejemplo en `"12x"`.

---

## 8. Ejemplo 2 — Cargar en un arreglo fijo de estructuras

Archivo: [`02_cargar_csv_arreglo.c`](src/02_cargar_csv_arreglo.c)

La estructura guarda sus propios arreglos de caracteres:

```c
typedef struct {
    int id;
    char nombre[TAM_NOMBRE];
    char genero[TAM_GENERO];
} PERSONA;
```

El programa reserva espacio para un máximo conocido de personas:

```c
PERSONA personas[MAX_PERSONAS];
size_t cantidad;
```

Después de separar una línea, no se asignan directamente los punteros devueltos por `strtok`. Se copian los textos al interior de la estructura:

```c
strncpy(persona->nombre, nombre, sizeof(persona->nombre) - 1);
persona->nombre[sizeof(persona->nombre) - 1] = '\0';
```

Esto hace que cada `PERSONA` conserve su información aunque `linea` se reutilice en la próxima llamada a `fgets`.

```bash
gcc -Wall 02_cargar_csv_arreglo.c -o 02_cargar_csv_arreglo
./02_cargar_csv_arreglo
```

La variable `cantidad` es fundamental: indica cuántas posiciones del arreglo contienen registros válidos. Un arreglo de capacidad 100 no implica que tenga 100 personas cargadas.

---

## 9. El error habitual con punteros y `strtok`

Este problema aparece cuando la estructura guarda textos mediante punteros, como en el ejemplo dinámico que sigue:

```c
typedef struct {
    int id;
    char *nombre;
    char *genero;
} PERSONA;

PERSONA persona;
```

En ese caso, la siguiente asignación **compila**, pero no es correcta si se quiere conservar el dato después de leer otra línea:

```c
persona.nombre = nombre;
```

`nombre` apunta dentro de `linea`, el buffer que `fgets` reutiliza para leer cada fila. La asignación sólo copia la dirección, no copia los caracteres. Cuando se lee la siguiente fila, el contenido apuntado cambia. Al terminar el recorrido, los punteros pueden mostrar la última línea leída o datos que ya no representan a cada persona.

En cambio, en el ejemplo anterior `nombre` está declarado como un arreglo dentro de la estructura (`char nombre[TAM_NOMBRE]`). En ese caso, `persona.nombre = nombre;` directamente no compila: los arreglos no se asignan con `=`. Allí se usa `strncpy` para copiar los caracteres. En el caso dinámico, la solución será reservar y copiar una cadena independiente, como se muestra en el ejemplo siguiente.

```text
linea ──┐
        ├── nombre de persona 1  (incorrecto: apunta al mismo buffer)
        ├── nombre de persona 2  (incorrecto: apunta al mismo buffer)
        └── nombre de persona 3  (incorrecto: apunta al mismo buffer)
```

Hay dos soluciones válidas:

- usar arreglos de caracteres dentro de la estructura, como en el ejemplo 2;
- reservar una copia independiente para cada texto con `malloc`, y liberarla luego con `free`.

---

## 10. Ejemplo 3 — Arreglo dinámico y copias independientes

Archivo: [`03_cargar_csv_dinamico.c`](src/03_cargar_csv_dinamico.c)

En este ejemplo tanto el arreglo de personas como los textos de cada persona usan memoria dinámica:

```c
typedef struct {
    int id;
    char *nombre;
    char *genero;
} PERSONA;
```

La función `duplicar_texto` reserva exactamente los bytes necesarios y copia el texto:

```c
longitud = strlen(texto);
copia = malloc(longitud + 1);
memcpy(copia, texto, longitud + 1);
```

El `+ 1` es necesario para incluir el terminador nulo. Ahora cada puntero de una `PERSONA` apunta a una reserva distinta, no al buffer `linea`.

El arreglo comienza vacío y crece cuando se llena:

```c
capacidad = (capacidad == 0) ? 8 : capacidad * 2;
personas_nuevas = realloc(personas, capacidad * sizeof(*personas));
```

Se usa una variable temporal para el resultado de `realloc`. Si la reserva falla, `realloc` devuelve `NULL`, pero el puntero original continúa siendo válido; asignar directamente el resultado haría perderlo.

Al finalizar, se debe liberar primero el texto de cada persona y luego el arreglo:

```c
liberar_personas(personas, cantidad);
```

```bash
gcc -Wall 03_cargar_csv_dinamico.c -o 03_cargar_csv_dinamico
./03_cargar_csv_dinamico
```

---

## 11. Errores frecuentes

| Error | Consecuencia | Forma de evitarlo |
|---|---|---|
| no comprobar `fopen` | se usa un archivo no abierto | verificar `archivo == NULL` |
| procesar el encabezado como dato | falla la conversión de `id` | leer y descartar la primera línea |
| olvidar quitar `\n` | el último campo puede incluir el salto de línea | usar `strcspn` antes de separar |
| guardar los punteros de `strtok` | los datos cambian al reutilizar `linea` | copiar el texto o reservar memoria |
| no controlar los tokens | se aceptan líneas incompletas | verificar los tres campos y que no exista un cuarto |
| usar `atoi` sin validar | no se detectan algunos datos incorrectos | usar `strtol` cuando se necesite validación |
| usar `realloc` directamente | puede perderse el puntero original ante un error | usar un puntero temporal |
| no llamar a `free` | queda memoria reservada sin liberar | liberar cada texto y el arreglo final |

---

## 12. Actividad y autoevaluación

1. Ejecutar `01_recorrer_csv.c` y seguir una línea en el depurador o con impresiones: observar el valor de `linea` antes y después de `strtok`.
2. Agregar una línea inválida a `example.csv`, por ejemplo `20,Nombre sin genero`, y comprobar que el programa la informa sin detenerse.
3. Agregar una línea con cuatro campos y verificar que también se rechaza.
4. Ejecutar los tres ejemplos con [`example_con_errores.csv`](src/example_con_errores.csv) (renombrarlo temporalmente como `example.csv` o adaptar el nombre de apertura). Identificar qué error informa cada fila y comprobar que la fila larga se descarta por completo.
5. En `02_cargar_csv_arreglo.c`, implementar una función que reciba el arreglo y un ID, y devuelva un puntero a la persona encontrada o `NULL`.
6. Modificar el ejemplo 2 para informar cuántos registros corresponden a cada valor de `genero`.
7. Cambiar `TAM_NOMBRE` por un tamaño pequeño y observar qué ocurre con los nombres largos. Explicar por qué el programa sigue teniendo una cadena terminada en `\0`.
8. En `03_cargar_csv_dinamico.c`, agregar una función que ordene el arreglo por ID antes de imprimirlo.
9. **Desafío integrador:** escribir un programa que lea `example.csv`, cargue todas las personas, permita buscar por ID y guarde el resultado de la búsqueda en un nuevo archivo de texto con encabezado. Definir qué debe ocurrir si el ID no existe y verificar todos los retornos de las funciones de archivo y memoria.

### Para recordar

```text
fgets       → lee una línea de texto
strcspn     → permite localizar \r o \n para quitarlo
strtok      → separa una cadena y modifica el buffer original
strtol      → convierte texto a long y permite validar la conversión
struct      → agrupa los campos de un registro
realloc     → cambia el tamaño de una reserva dinámica
free        → libera la memoria reservada con malloc o realloc
```
