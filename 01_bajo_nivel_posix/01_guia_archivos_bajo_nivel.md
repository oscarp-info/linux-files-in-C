# Archivos en C y Linux — Guía 1: bajo nivel desde cero

**Informática 1 · UTN Electrónica**  
**Lenguaje:** C · **Sistema:** GNU/Linux  
**Uso:** guía para proyectar y entregar a estudiantes

**Material complementario:** [presentación de bajo nivel con POSIX](01_archivos_bajo_nivel_posix.pptx)

---

## 1. Objetivos

En esta guía se trabajará con la interfaz POSIX de Linux para acceder a archivos. Al finalizar deberías poder:

- abrir un archivo y obtener su descriptor con `open`;
- elegir banderas adecuadas para leer, escribir, crear, vaciar o agregar datos;
- guardar y recuperar enteros, texto y estructuras mediante `write` y `read`;
- recorrer un archivo de forma secuencial y reposicionar su posición con `lseek`;
- usar los descriptores estándar para leer desde el teclado y escribir en la terminal;
- comprobar errores y distinguir el final de un archivo de una falla;
- consultar el tamaño y los permisos de un archivo con `stat`.

Las principales llamadas POSIX de la guía son:

| Función | Acción | Devuelve |
|---|---|---|
| `open` | abre y, opcionalmente, crea un archivo | descriptor o `-1` |
| `write` | escribe bytes | cantidad escrita o `-1` |
| `read` | lee bytes | cantidad leída, `0` al final o `-1` |
| `lseek` | cambia o consulta la posición actual | nueva posición o `-1` |
| `close` | cierra el descriptor | `0` o `-1` |
| `stat` | consulta metadatos del archivo | `0` o `-1` |

---

## 2. ¿Por qué archivos?

Las variables existen mientras el programa está ejecutándose. Al terminar, sus valores se pierden. Un **archivo** permite guardar información de manera persistente: el programa termina, pero los datos permanecen en el disco.

Un archivo es una secuencia ordenada de **bytes**. Para usarlo, el programa le pide permiso al sistema operativo. Linux responde con un número entero llamado **descriptor de archivo**, que guardaremos en una variable `int`.

### C, POSIX y Linux

El lenguaje de estos programas es **C estándar**, pero las funciones `open`, `read`, `write`, `lseek` y `close` pertenecen a **POSIX**. POSIX es un conjunto de normas para sistemas operativos tipo Unix, como GNU/Linux, que define una forma común de trabajar con archivos, procesos y otros recursos del sistema.
Las funciones `open`, `read`, `write`, `lseek` y `close` están definidas por POSIX. En Linux, el programa las invoca a través de la biblioteca de C del sistema, que actúa como puente entre el código en C y el kernel. El kernel es quien realiza finalmente la operación sobre el archivo, la terminal o el dispositivo.

```text
programa en C → biblioteca de C → kernel de Linux
```

Por eso estos ejemplos se compilan con C en Linux, pero no son portables sin cambios a cualquier entorno que sólo implemente la biblioteca estándar de C. La alternativa exclusivamente estándar usaría `fopen`, `fread`, `fwrite` y `fclose`; en esta guía trabajamos más cerca del sistema operativo mediante POSIX.

También aparecen dos tipos para contar bytes:

| Tipo | Uso |
|---|---|
| `size_t` | tamaños que no pueden ser negativos; es el tipo que devuelve `sizeof(...)` |
| `ssize_t` | cantidad de bytes leídos o escritos, o `-1` si ocurre un error; es el tipo que devuelven `read` y `write` |

Por ejemplo:

```c
ssize_t leidos;

leidos = read(fd, buffer, sizeof(buffer));
```

Si `read` tiene éxito, `leidos` contiene una cantidad mayor o igual a cero. Si falla, contiene `-1`. Usar `ssize_t` evita que un resultado válido de `read` o `write` sea demasiado grande para un `int`. Para mostrar un `ssize_t` con `printf`, se usa `%zd`.
### “Everything is a file”

Una idea clásica de Unix es **“everything is a file”**: muchos recursos del sistema se pueden usar mediante una interfaz similar a la de un archivo. No significa que literalmente todo sea un archivo guardado en disco, sino que el sistema ofrece una forma común de acceder a recursos distintos.

Por ejemplo, un programa puede leer o escribir usando operaciones parecidas sobre:

- archivos comunes, como `mensaje.txt`;
- directorios;
- la terminal;
- dispositivos, como un disco o una impresora;
- canales de comunicación entre programas, como tuberías y sockets.

Una clasificación sencilla ayuda a reconocerlos:

| Tipo de recurso | Ejemplo | Idea principal |
|---|---|---|
| archivo regular | `mensaje.txt` | guarda datos en el sistema de archivos |
| directorio | `documentos/` | organiza nombres de otros recursos |
| dispositivo | terminal, disco, impresora | permite comunicarse con hardware mediante un controlador |
| tubería o socket | comunicación entre procesos | permite intercambiar datos entre programas |
| enlace simbólico | acceso directo a otro recurso | guarda una referencia a una ruta |

Cuando un recurso se abre, Linux suele entregar un **descriptor de archivo**. Con ese número se pueden aplicar operaciones como `read`, `write` y `close`. Esta idea permite que las mismas funciones que escriben en un archivo también puedan enviar texto a la terminal mediante `STDOUT_FILENO` o mensajes de error mediante `STDERR_FILENO`.

Un descriptor no es el archivo ni contiene sus datos: es un número que identifica un recurso abierto dentro de un proceso. Por eso dos programas pueden usar el mismo archivo y recibir números de descriptor distintos. Al llamar a `close`, el número queda libre y el sistema puede reutilizarlo en una apertura posterior. Los valores `0`, `1` y `2` tienen un uso habitual para la entrada, la salida y los errores estándar; los demás no deben suponerse fijos.

### Permisos de archivos en Linux

Linux controla quién puede acceder a cada archivo mediante permisos. Hay tres grupos de usuarios:

| Grupo | Representa |
|---|---|
| usuario | la persona propietaria del archivo |
| grupo | las personas que pertenecen al mismo grupo |
| otros | cualquier otra persona del sistema |

Para cada grupo se pueden asignar estos permisos:

| Letra | Permiso | Valor numérico |
|---|---|---|
| `r` | leer (*read*) | `4` |
| `w` | escribir (*write*) | `2` |
| `x` | ejecutar (*execute*) | `1` |

Al ejecutar `ls -l`, una salida como esta:

```text
-rw-r--r-- 1 usuario grupo 18 ago 14 12:00 mensaje.txt
```

indica que el propietario puede leer y escribir (`rw-`), mientras que el grupo y los demás usuarios sólo pueden leer (`r--`).

Los permisos también se pueden expresar en octal. Cada dígito suma los valores de `r`, `w` y `x` para un grupo:

```text
0644  →  usuario: 6 = r + w
         grupo:   4 = r
         otros:   4 = r
```

Por eso, cuando un programa crea un archivo con:

```c
open("mensaje.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

solicita permisos `rw-r--r--`. El cero inicial indica que el valor está escrito en octal. El sistema puede restringir esos permisos mediante la configuración `umask`, por lo que los permisos finales nunca serán más amplios que los solicitados.

#### Cambiar permisos con `chmod`

El tercer argumento de `open` se usa al **crear** un archivo. Para cambiar los permisos de un archivo que ya existe desde la terminal se usa `chmod`. Sólo la persona propietaria del archivo, o el administrador del sistema, puede hacerlo.

Primero conviene observar los permisos actuales:

```bash
ls -l mensaje.txt
```

Después se puede indicar el valor octal, igual que en `open`:

```bash
chmod 600 privado.txt   # rw-------: sólo la persona propietaria puede leer y escribir
chmod 644 mensaje.txt   # rw-r--r--: la persona propietaria escribe; los demás sólo leen
chmod 755 programa      # rwxr-xr-x: permite ejecutar el programa a todas las personas
```

También existe una forma simbólica. `u` representa al usuario propietario, `g` al grupo y `o` a otros:

```bash
chmod u+x programa      # agrega permiso de ejecución al propietario
chmod go-w informe.txt  # quita permiso de escritura al grupo y a otros
chmod o+r publico.txt   # agrega permiso de lectura a otros
```

`chmod` no modifica el contenido del archivo: sólo cambia quién puede leerlo, escribirlo o ejecutarlo. Evitá usar permisos como `777` por costumbre, porque otorgan todos los permisos a cualquier usuario del sistema.

En los directorios, `x` permite entrar o atravesar el directorio; por eso tiene un significado diferente que en los archivos comunes.
Cada descriptor tiene una **posición actual**: indica desde dónde se hará la próxima lectura o escritura. Al leer o escribir, esa posición avanza.

```text
Archivo:   H  o  l  a  \n
Posición:                 ^
```



## 3. Preparación

En una terminal Linux, abrí la carpeta de esta guía:

```bash
cd ~/archivos/02_low_level
```

> Si la carpeta está en otro lugar, usá la ruta en la que la hayas descargado. Los programas crean sus archivos de salida en la carpeta actual.

Para compilar los ejemplos usaremos siempre:

```bash
gcc -Wall NOMBRE.c -o PROGRAMA
```

Las advertencias activadas por esas opciones son parte de la ayuda: no conviene ignorarlas.

### Ver los bytes con `hexdump`

Muchos de estos ejemplos guardan datos binarios: enteros y estructuras no se interpretan correctamente con `cat`. Para observar los bytes reales del archivo se puede usar:

```bash
hexdump -C NOMBRE_ARCHIVO
```

La opción `-C` muestra, en cada línea, la posición dentro del archivo, los bytes en hexadecimal y una representación de los caracteres imprimibles. Por ejemplo:

```bash
hexdump -C database.dat
```

#### Instalar `hexdump` si no está disponible

Primero comprobá si el comando ya está instalado:

```bash
hexdump --version
```

En Debian, Ubuntu y distribuciones derivadas, `hexdump` se instala con el paquete `bsdextrautils`:

```bash
sudo apt update
sudo apt install bsdextrautils
```

En Arch Linux forma parte de `util-linux`:

```bash
sudo pacman -S util-linux
```

Usá `cat` cuando el archivo contenga texto y `hexdump -C` cuando quieras inspeccionar los bytes guardados.

### Datos binarios: enteros y estructuras

Un entero o una estructura no se guarda como texto: se guarda como una secuencia de bytes. Por eso, al inspeccionar `database.dat` o `listado.bin` con `hexdump -C`, los valores pueden no verse como los números que escribimos en el programa.

El orden en que se almacenan los bytes de un número se denomina **orden de bytes** o *endianness*. Además, una estructura puede incluir bytes de relleno (*padding*) entre sus campos. Por estas razones, guardar estructuras directamente es práctico para aprender y para usar archivos propios en la misma máquina, pero no es una forma portable de intercambiar datos entre arquitecturas o entre versiones distintas de un programa.


---

## 4. Abrir archivos con `open`

La llamada `open` abre un archivo y devuelve un **descriptor de archivo**: un número entero que se usa luego con `read`, `write`, `lseek` y `close`.

```c
int open(const char *ruta, int banderas, ...);
```

- `ruta` indica el archivo que se quiere abrir.
- `banderas` especifica el acceso y otras opciones; se combinan con `|`.
- El tercer argumento de permisos sólo se indica cuando se usa `O_CREAT`.
- Si tiene éxito, devuelve un descriptor mayor o igual que cero; si falla, devuelve `-1`.

Las banderas más frecuentes se agrupan en dos clases.

Primero se elige **una sola** bandera de acceso:

| Bandera | Acceso permitido |
|---|---|
| `O_RDONLY` | sólo lectura |
| `O_WRONLY` | sólo escritura |
| `O_RDWR` | lectura y escritura |

Luego se pueden combinar opciones adicionales con `|`:

| Opción | Efecto |
|---|---|
| `O_CREAT` | crea el archivo si no existe; por eso requiere el tercer argumento de permisos |
| `O_EXCL` | junto con `O_CREAT`, hace que `open` falle si el archivo ya existe |
| `O_TRUNC` | vacía el archivo al abrirlo; se empieza desde cero |
| `O_APPEND` | conserva el contenido y hace que cada `write` se agregue al final |

Por ejemplo, `O_WRONLY | O_CREAT | O_TRUNC` crea o reemplaza el contenido de un archivo. En cambio, `O_WRONLY | O_CREAT | O_APPEND` permite agregar nuevos datos sin borrar los anteriores. Para crear un archivo nuevo sin sobrescribir uno existente se puede usar `O_WRONLY | O_CREAT | O_EXCL`.

`O_TRUNC` y `O_APPEND` tienen sentido al escribir. No se deben combinar entre sí: una bandera vacía el archivo y la otra conserva su contenido para agregar datos al final.

---
## 5. Comprobar errores

Cada llamada debe verificar su valor de retorno. La tabla inicial indica qué resultado representa un error en cada función: muchas devuelven `-1`, mientras que `read` devuelve `0` cuando llega al final de un archivo regular.

Cuando una llamada falla, Linux guarda el motivo en `errno`. La función `perror` imprime un mensaje asociado a ese motivo en la salida de error:

```c
perror("No se pudo abrir el archivo");
```

Podría mostrar, por ejemplo:

```text
No se pudo abrir el archivo: No such file or directory
```

`perror` debe ejecutarse inmediatamente después de la llamada que falló, antes de invocar otra función que pudiera modificar `errno`.

---

## 6. Ejemplo 1 — Guardar enteros

Archivo: [`01_guardar_enteros.c`](src/01_guardar_enteros.c)

```bash
gcc -Wall 01_guardar_enteros.c -o 01_guardar_enteros
./01_guardar_enteros
hexdump -C database.dat
```

El programa guarda los enteros `1` y `2` en `database.dat`. Cada llamada a `write` recibe la dirección de un entero y `sizeof(data_01)` indica cuántos bytes debe guardar.

---

## 7. Ejemplo 2 — Guardar un carácter

Archivo: [`02_guardar_caracter.c`](src/02_guardar_caracter.c)

```bash
gcc -Wall 02_guardar_caracter.c -o 02_guardar_caracter
./02_guardar_caracter
hexdump -C database.dat
```

Este programa reemplaza el contenido de `database.dat` por el carácter `'1'`. Como un `char` ocupa un byte, `write` guarda un único byte.

---

## 8. Ejemplo 3 — Leer caracteres secuencialmente

Archivo: [`03_leer_caracteres.c`](src/03_leer_caracteres.c)

```bash
gcc -Wall 03_leer_caracteres.c -o 03_leer_caracteres
./03_leer_caracteres
hexdump -C database.dat
```

El programa intenta leer tres caracteres de `database.dat`. Cada `read` comienza en la posición posterior a la lectura anterior: esto es acceso secuencial. Si alcanza el final del archivo, `read` devuelve `0` y el programa lo informa.

> Para observar tres lecturas correctas, `database.dat` debe contener al menos tres caracteres. Podés crearlo, por ejemplo, con `printf "123" > database.dat`.

---
## 9. Acceso secuencial, acceso directo y `lseek`

Un archivo se puede recorrer de dos maneras.

- **Acceso secuencial:** se lee o escribe desde la posición actual y se continúa desde donde terminó la operación anterior. Cada `read` y cada `write` avanza automáticamente la posición del descriptor.
- **Acceso directo o aleatorio:** se elige previamente la posición a la que se quiere ir. Esto permite leer o modificar un dato concreto sin recorrer todos los anteriores.

Por ejemplo, al leer un archivo de texto bloque por bloque se usa acceso secuencial. En cambio, si un archivo contiene registros de igual tamaño y se quiere modificar el alumno de índice `i`, se usa acceso directo.



Al terminar un `write`, la posición actual queda al final. Si queremos leer desde el inicio usando el mismo descriptor, debemos cambiarla mediante acceso directo.

```c
lseek(fd, desplazamiento, referencia);
```

| Referencia | Punto desde el que se cuenta |
|---|---|
| `SEEK_SET` | inicio del archivo |
| `SEEK_CUR` | posición actual |
| `SEEK_END` | final del archivo |

Para volver al comienzo se usa:

```c
lseek(fd, 0, SEEK_SET);
```

### Obtener el tamaño con `lseek`

También se puede conocer el tamaño de un archivo buscando su final. La posición obtenida al desplazarse cero bytes desde `SEEK_END` coincide con la cantidad de bytes del archivo:

```c
off_t tamanio;

tamanio = lseek(fd, 0, SEEK_END);
if (tamanio == (off_t)-1) {
    perror("No se pudo obtener el tamaño");
}
```

Este ejemplo es útil para entender la posición actual: después de esa llamada, el cursor queda al final. Si se necesita conservar la posición anterior, primero se la guarda y luego se la restaura:

```c
off_t posicion_actual;
off_t tamanio;

posicion_actual = lseek(fd, 0, SEEK_CUR);
tamanio = lseek(fd, 0, SEEK_END);
lseek(fd, posicion_actual, SEEK_SET);
```

`lseek` funciona con archivos que permiten reposicionamiento, como los archivos comunes. Para consultar el tamaño sin mover la posición del descriptor, `stat` es la opción más apropiada; el ejemplo 12 muestra cómo usarla.

### Una posición compartida por `read` y `write`

Un descriptor tiene una única posición actual. Tanto `read` como `write` usan esa posición y la avanzan según la cantidad de bytes que procesan. Por eso se pueden alternar ambas funciones sobre un descriptor abierto con `O_RDWR`:

```c
leidos = read(fd, buffer, sizeof(buffer));
/* La posición avanza por la cantidad leída. */

if (leidos > 0) {
    write(fd, buffer, (size_t)leidos);
    /* La escritura comienza en la nueva posición. */
}
```

En este caso, los bytes leídos se escriben a continuación de la zona que se acaba de leer. Si se quisieran modificar esos mismos bytes, antes de `write` habría que volver la posición con `lseek`.

> Se deben verificar los valores devueltos por `read` y `write`: una operación puede procesar menos bytes que los solicitados.

---

## 10. Ejemplo 4 — Registros y búsqueda por identificador

Archivo: [`04_registros.c`](src/04_registros.c)

```bash
gcc -Wall 04_registros.c -o 04_registros
./04_registros
hexdump -C listado.bin
```

El programa guarda tres registros en `listado.bin`, los lee completos y busca el registro cuyo identificador es `3`. La búsqueda usa acceso directo: calcula la posición `(id - 1) * sizeof(REGISTRO)` y la alcanza con `lseek` antes de leer.

---
## 11. Ejemplo 5 — Crear y escribir

Archivo: [`05_guardar_texto.c`](src/05_guardar_texto.c)

```bash
gcc -Wall 05_guardar_texto.c -o 05_guardar_texto
./05_guardar_texto
cat mensaje.txt
hexdump -C mensaje.txt
```

El programa crea `mensaje.txt` y escribe una línea. La apertura es:

```c
open("mensaje.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644)
```

- `O_WRONLY`: sólo se escribirá.
- `O_CREAT`: crear si no existe.
- `O_TRUNC`: si existe, vaciarlo antes de escribir.
- `0644`: permisos al crear el archivo.

La llamada `write(fd, mensaje, cantidad)` copia bytes desde la memoria al archivo. Su valor de retorno debe comprobarse: puede haber error o una escritura incompleta.

`sizeof(mensaje) - 1` evita guardar el byte `\0` que C usa al final de las cadenas.

**Preguntas:** ¿qué ocurriría sin `O_CREAT`? ¿Qué peligro tiene usar `O_TRUNC` sin querer? ¿Por qué se verifica el resultado de `write`?

---

## 12. Ejemplo 6 — Abrir y leer

Archivo: [`06_leer_texto.c`](src/06_leer_texto.c)

Ejecutá primero el ejemplo 5 y luego:

```bash
gcc -Wall 06_leer_texto.c -o 06_leer_texto
./06_leer_texto
```

El archivo se abre con `O_RDONLY`: sólo lectura. `read` toma bytes del archivo y los copia a `buffer`.

```c
leidos = read(fd, buffer, sizeof(buffer) - 1);
buffer[leidos] = '\0';
```

`read` no agrega el terminador de texto `\0`. Lo agregamos porque luego imprimimos el buffer con `%s`. Pedimos como máximo 127 bytes en un arreglo de 128 para reservar ese lugar final.

| Resultado de `read` | Significado |
|---|---|
| mayor que `0` | leyó esa cantidad de bytes |
| `0` | llegó al final del archivo |
| `-1` | ocurrió un error |

**Preguntas:** ¿por qué no es seguro imprimir el buffer como texto antes de agregar `\0`? Si el archivo tiene 300 bytes, ¿cuántos muestra este programa en una lectura?

---

## 13. Ejemplo 7 — Guardar y recuperar una estructura

Archivo: [`07_guardar_recuperar_estructura.c`](src/07_guardar_recuperar_estructura.c)

```bash
gcc -Wall 07_guardar_recuperar_estructura.c -o 07_guardar_recuperar_estructura
./07_guardar_recuperar_estructura
ls -l alumno.dat
hexdump -C alumno.dat
```

El programa guarda un `Alumno` en `alumno.dat`, vuelve al comienzo y lo recupera. Una estructura ocupa bytes en memoria; este ejemplo escribe y lee esos bytes directamente.

La apertura usa `O_RDWR`:

```c
open("alumno.dat", O_RDWR | O_CREAT | O_TRUNC, 0644)
```

`O_RDWR` es necesario porque el mismo descriptor se usa para **escribir y luego leer**.

> **Regla importante:** si el archivo se abre con `O_RDONLY`, no se puede escribir. Si se abre con `O_WRONLY`, no se puede leer. El modo debe coincidir con la operación que realizará el programa.

Después de `write`, el cursor está al final. El `lseek` es necesario antes de `read`.

**Nota:** guardar estructuras directamente es útil para aprender bajo nivel. En otras unidades veremos por qué no siempre es adecuado para intercambiar datos entre arquitecturas o versiones distintas de un programa.

---

## 14. Ejemplo 8 — Buscar y reemplazar un registro

Archivo: [`08_reemplazar_registro.c`](src/08_reemplazar_registro.c)

```bash
gcc -Wall 08_reemplazar_registro.c -o 08_reemplazar_registro
./08_reemplazar_registro
hexdump -C curso.dat
```

Si todos los registros tienen igual tamaño, la posición del registro de índice `i` es:

```text
posición = i × sizeof(TipoDeRegistro)
```

El primer registro tiene índice 0. El programa crea tres alumnos y reemplaza el que está en el índice 1 —el segundo— mediante `lseek`.

```text
Índice:       0              1              2
Archivo:  [1001, 6.0]   [1002, 7.0]   [1003, 8.0]
                           ↑
              lseek(fd, 1 × sizeof(Alumno), SEEK_SET)
                           ↓
Archivo:  [1001, 6.0]  [1002, 10.0]  [1003, 8.0]
```

---

## 15. Ejemplo 9 — `stdout`, `stderr` y `write`

Archivo: [`09_salidas_estandar.c`](src/09_salidas_estandar.c)

Este programa no trabaja con un archivo. Sirve para distinguir la **salida normal** del programa, llamada `stdout`, de la **salida de errores**, llamada `stderr`.

```bash
gcc -Wall 09_salidas_estandar.c -o 09_salidas_estandar
./09_salidas_estandar
```

El programa usa ambos descriptores de salida:

```c
write(STDOUT_FILENO, resultado, sizeof(resultado) - 1);
write(STDERR_FILENO, advertencia, sizeof(advertencia) - 1);
```

`STDOUT_FILENO` representa la salida normal y su valor es `1`. `STDERR_FILENO` representa la salida de errores y su valor es `2`. Ambos se definen en `<unistd.h>`.

En bajo nivel, `write` es el equivalente para enviar bytes a cualquiera de esos destinos. A diferencia de las funciones de salida con formato de la biblioteca estándar, `write` no interpreta formatos como `%d` o `%s`: recibe los bytes ya preparados en memoria.

Para una salida fija, como la de este ejemplo, alcanza con escribir la cadena directamente. Para construir un mensaje con números u otros datos, primero se puede usar `snprintf` para preparar el texto en un buffer y luego enviarlo con `write`.

En la terminal ambas salidas suelen verse juntas. La diferencia aparece al redirigir la salida normal a un archivo:

```bash
./09_salidas_estandar > salida.txt
cat salida.txt
```

En la terminal seguirá apareciendo la advertencia, mientras que `salida.txt` contendrá solamente:

```text
Resultado: se procesaron 3 alumnos.
```

Para guardar cada salida por separado:

```bash
./09_salidas_estandar > salida.txt 2> errores.txt
cat salida.txt
cat errores.txt
```

El número `2` representa `stderr`; `>` redirige `stdout` y `2>` redirige `stderr`. Esta separación permite conservar los resultados de un programa sin mezclar mensajes de error o advertencias.

---
## 16. Ejemplo 10 — Copiar un archivo completo

Archivo: [`10_copiar_archivo.c`](src/10_copiar_archivo.c)

```bash
gcc -Wall 10_copiar_archivo.c -o 10_copiar_archivo
./10_copiar_archivo database.dat copia.dat
hexdump -C copia.dat
```

El programa copia un archivo por bloques. Repite `read` hasta que devuelve `0`, que indica el final del archivo. Para cada bloque leído, la función `escribir_todo` repite `write` hasta guardar todos los bytes.

Este patrón es importante porque `read` y `write` pueden procesar menos bytes de los solicitados. Una lectura menor puede ser normal; una escritura menor obliga a volver a escribir los bytes que faltan.

---

## 17. Ejemplo 11 — Entrada estándar

Archivo: [`11_entrada_estandar.c`](src/11_entrada_estandar.c)

```bash
gcc -Wall 11_entrada_estandar.c -o 11_entrada_estandar
./11_entrada_estandar
```

Los tres descriptores que un programa recibe habitualmente al comenzar son:

| Descriptor | Constante | Uso |
|---|---|---|
| `0` | `STDIN_FILENO` | entrada estándar, normalmente el teclado |
| `1` | `STDOUT_FILENO` | salida normal, normalmente la terminal |
| `2` | `STDERR_FILENO` | salida de errores, normalmente la terminal |

El programa lee lo que se escribe por teclado mediante `read(STDIN_FILENO, ...)` y lo envía a la salida normal con `write(STDOUT_FILENO, ...)`.

También se puede redirigir la entrada desde un archivo:

```bash
./11_entrada_estandar < database.dat
```

---

## 18. Ejemplo 12 — Información de un archivo con `stat`

Archivo: [`12_info_archivo.c`](src/12_info_archivo.c)

```bash
gcc -Wall 12_info_archivo.c -o 12_info_archivo
./12_info_archivo database.dat
```

`stat` consulta los metadatos de un archivo sin abrirlo para leer o escribir. El programa muestra dos de esos datos: el tamaño, mediante `st_size`, y los permisos, mediante `st_mode`.

La función se declara en `<sys/stat.h>` y recibe la ruta del archivo junto con una estructura donde deja los datos:

```c
#include <sys/stat.h>

int stat(const char *ruta, struct stat *info);
```

Devuelve `0` si tiene éxito y `-1` si ocurre un error. El programa declara:

```c
struct stat info;
```

y luego llama a `stat(argv[1], &info)`. De todos los campos disponibles, en este ejemplo se usan:

| Campo | Qué informa |
|---|---|
| `info.st_size` | tamaño del archivo en bytes |
| `info.st_mode` | tipo del recurso y permisos |

`st_mode` no contiene solamente los permisos. La expresión `info.st_mode & 0777` conserva únicamente los nueve bits de permisos para mostrarlos en octal.

También se puede comprobar el efecto de `chmod` con un archivo nuevo:

```bash
touch prueba.txt
chmod 640 prueba.txt
./12_info_archivo prueba.txt
ls -l prueba.txt
```

El programa debe mostrar permisos `640`, mientras que `ls -l` los representa como `rw-r-----`.

Compará el resultado con:

```bash
ls -l database.dat
```

---
## 19. Errores frecuentes

| Error | Consecuencia | Prevención |
|---|---|---|
| no comprobar `open` | descriptor inválido | verificar `fd == -1` |
| abrir con `O_RDONLY` e intentar escribir | `write` falla | usar `O_WRONLY` u `O_RDWR` |
| abrir con `O_WRONLY` e intentar leer | `read` falla | usar `O_RDONLY` u `O_RDWR` |
| usar `O_TRUNC` sin querer | se pierde el contenido anterior | usarlo sólo al comenzar de cero |
| ignorar el retorno de `read` o `write` | datos incompletos | comparar la cantidad obtenida |
| imprimir un buffer sin `\0` | salida incorrecta | terminarlo sólo si se trata de texto |
| leer después de escribir sin `lseek` | lectura desde el final | volver con `lseek` |
| no llamar a `close` | recurso abierto; error de cierre oculto | cerrar todo descriptor abierto |

---

## 20. Actividad y autoevaluación

### Práctica básica

1. Ejecutar `01_guardar_enteros` y observar `database.dat` con `hexdump -C`. ¿Cuántos bytes ocupa cada entero? ¿Cómo aparecen los valores en hexadecimal?
2. Ejecutar `02_guardar_caracter` y comprobar con `hexdump -C database.dat` que se guardó un único byte. Luego crear un archivo con tres caracteres y ejecutar el ejemplo de lectura secuencial:

   ```bash
   printf "123" > database.dat
   ./03_leer_caracteres
   ```

   Explicar cómo cambia la posición del descriptor después de cada `read`.
3. En `04_registros.c`, agregar un cuarto registro y buscarlo por su identificador. Mostrar con `hexdump -C listado.bin` cómo se almacenan los registros.

### Práctica de archivos y posiciones

4. En `05_guardar_texto.c`, cambiar `O_TRUNC` por `O_APPEND` y ejecutar dos veces. Comparar el resultado con `cat mensaje.txt` y `hexdump -C mensaje.txt`.
5. En `07_guardar_recuperar_estructura.c`, quitar temporalmente el `lseek` antes de `read`. Explicar por qué la lectura llega al final del archivo. Restaurar la línea y comprobar el resultado.
6. En `08_reemplazar_registro.c`, agregar un cuarto alumno y modificar el de índice `3`. Antes y después, inspeccionar `curso.dat` con `hexdump -C`.
7. Usar `10_copiar_archivo` para copiar `database.dat` en `copia.dat`. Comparar ambos con `hexdump -C` y consultar su tamaño con `12_info_archivo`.

   ```bash
   ./10_copiar_archivo database.dat copia.dat
   hexdump -C database.dat
   hexdump -C copia.dat
   ./12_info_archivo copia.dat
   ```

8. Ejecutar `11_entrada_estandar`, escribir una línea y observar que aparece nuevamente en pantalla. Después redirigir la entrada desde `database.dat`:

   ```bash
   ./11_entrada_estandar < database.dat
   ```

### Desafío

Modificar `10_copiar_archivo.c` para que el archivo de destino se abra con `O_APPEND` en lugar de `O_TRUNC`. Copiar el mismo archivo dos veces sobre el destino y explicar, con ayuda de `hexdump -C`, por qué el contenido queda repetido.

### Para recordar

```text
open  → abre y entrega un descriptor
write → memoria hacia archivo
read  → archivo hacia memoria
lseek → mueve la posición
close → termina el uso del descriptor
```

1. ¿Qué representa el valor devuelto por `open` si sale bien?
2. ¿Qué devuelve `read` al llegar al final de un archivo regular?
3. ¿Cuál es la diferencia entre `O_RDONLY`, `O_WRONLY` y `O_RDWR`?
4. ¿Cuál es el desplazamiento del cuarto registro de estructuras iguales?
5. ¿Qué valor devuelve `lseek(fd, 0, SEEK_END)` si tiene éxito?
6. ¿Qué diferencia hay entre obtener el tamaño con `lseek` y obtenerlo con `stat`?

Respuestas sugeridas: un descriptor; `0`; sólo lectura, sólo escritura y ambos; `3 * sizeof(TipoDeRegistro)`; el tamaño del archivo en bytes y deja el cursor al final; `lseek` mueve la posición de un descriptor abierto, mientras que `stat` consulta los metadatos sin moverlo.