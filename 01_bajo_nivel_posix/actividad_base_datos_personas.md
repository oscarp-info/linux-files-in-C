# Trabajo práctico integrador — Base de datos de personas

## Objetivo

Desarrollar en C una pequeña base de datos de personas almacenada en un archivo de datos. El trabajo integra los contenidos vistos hasta ahora: estructuras, arreglos, memoria dinámica, descriptores de archivo, `open`, `read`, `write`, `lseek`, `close`, `stat`, búsqueda secuencial, acceso directo y manejo de errores.

El archivo de datos se llamará `personas.dat`.

---

## Condiciones generales

- Trabajar con llamadas POSIX de bajo nivel: `open`, `read`, `write`, `lseek`, `close` y, cuando resulte útil, `stat`.
- No usar `FILE *`, `fopen`, `fread`, `fwrite` ni `fseek`. En este trabajo, `lseek` cumple el rol de reposicionar el cursor dentro del archivo.
- Comprobar el resultado de cada operación que pueda fallar.
- Cerrar todo descriptor de archivo que haya sido abierto.
- Usar `perror` inmediatamente después de detectar un error de una llamada al sistema.
- Usar `hexdump -C personas.dat` durante las pruebas para observar el contenido binario del archivo.
- Guardar en el archivo registros de tipo `PERSONA`, no punteros.

> **Importante:** una dirección de memoria sólo tiene sentido mientras el programa está ejecutándose. Por eso, aunque la base en memoria use punteros, el archivo debe guardar los bytes de cada estructura `PERSONA`; nunca las direcciones de los punteros.

---

## Estructura de datos

Usar la siguiente definición. Se permite agregar funciones auxiliares, pero no campos dinámicos dentro de la estructura.

```c
#define STR_NAME_LEN 30

typedef struct {
    int id;
    char nombre[STR_NAME_LEN];
    int edad;
    float altura;
    float peso;
} PERSONA;
```

El campo `id` debe ser positivo y único. Para que la inserción ordenada tenga sentido, no es obligatorio que sea consecutivo: por ejemplo, pueden existir los identificadores `1`, `3` y `8`, pero no puede haber dos personas con el mismo `id`.

Un registro de prueba válido sería:

```c
PERSONA p = {1, "Juan", 12, 1.40f, 40.0f};
```

---

## Etapas de desarrollo

### A. Guardar y recuperar una estructura

Crear funciones para guardar una única estructura `PERSONA` en `personas.dat` y para recuperarla.

Probar con una persona de ejemplo e imprimir todos sus campos luego de leerla desde el archivo.

Preguntas para responder:

1. ¿Cuántos bytes ocupa un registro `PERSONA`?
2. ¿Qué muestra `hexdump -C personas.dat`?
3. ¿Por qué `sizeof(PERSONA)` puede incluir bytes que no corresponden directamente a un campo visible?

### B. Guardar y recuperar un arreglo de estructuras

Crear un arreglo estático de personas y guardarlo completo en el archivo. Luego leerlo en otro arreglo e imprimir los registros recuperados.

La escritura puede hacerse de una sola vez:

```c
write(fd, arreglo, cantidad * sizeof(PERSONA));
```

Verificar que la cantidad de bytes escrita o leída coincida con la esperada.

### C. Trabajar con memoria dinámica

Reemplazar el arreglo estático por un arreglo dinámico de punteros:

```c
PERSONA **personas;
```

Cada elemento del arreglo debe apuntar a una `PERSONA` reservada con memoria dinámica. Al finalizar, liberar toda la memoria reservada.

Al guardar en el archivo, escribir `*personas[i]`, es decir, la estructura apuntada. Al cargar desde el archivo, reservar una estructura para cada registro leído y guardar su dirección en el arreglo de punteros.

> No escribir `personas[i]` directamente en el archivo: eso guardaría una dirección de memoria inválida para una ejecución posterior.

### D. Menú y carga inicial desde archivo

Implementar un menú que se repita hasta que la persona usuaria elija salir. Al iniciar el programa, cargar desde `personas.dat` los registros existentes en memoria.

El menú debe incluir, como mínimo:

```text
1. Cargar persona
2. Buscar por nombre
3. Buscar por ID
0. Salir
```

Se recomienda agregar también estas opciones:

```text
4. Listar personas
5. Modificar persona por ID
```

Al cargar una persona nueva, validar que el identificador no exista. Después de cada cambio, actualizar el archivo para que los datos permanezcan disponibles en la próxima ejecución.

La búsqueda por nombre puede ser secuencial y debe informar todas las coincidencias. Para comparar nombres puede utilizarse `strcmp`.

### E. Mantener la base ordenada por ID

Modificar la carga de personas para que el arreglo en memoria permanezca siempre ordenado de menor a mayor por `id`.

Cuando se agregue una persona nueva:

1. localizar la posición donde debe insertarse;
2. desplazar los punteros necesarios una posición hacia la derecha;
3. insertar el nuevo puntero;
4. actualizar el archivo para que conserve el mismo orden.

No se permite agregar un registro con un `id` repetido.

### F. Búsqueda eficiente por ID en archivo

Aprovechar que los registros están ordenados por `id` para implementar una búsqueda eficiente directamente sobre el archivo.

Usar búsqueda binaria. Para acceder al registro de índice `i`, calcular:

```text
posición = i × sizeof(PERSONA)
```

Luego posicionar el descriptor con:

```c
lseek(fd, posicion, SEEK_SET);
```

y leer únicamente ese registro.

Para conocer la cantidad de registros, obtener el tamaño del archivo y dividirlo por `sizeof(PERSONA)`. Se puede usar `stat` o `lseek(fd, 0, SEEK_END)`; si se usa `lseek`, recordar restaurar la posición si fuera necesario.

La función debe devolver o informar claramente si el ID fue encontrado o no.

### G. Modificar un registro por ID

Implementar una función que localice una persona por `id` y permita modificar `nombre`, `edad`, `altura` y `peso`.

El `id` no debe modificarse, porque cambiarlo podría romper el orden de la base de datos. Una vez localizado el registro, volver a su posición con `lseek` y sobrescribirlo con `write`.

Comprobar con `hexdump -C personas.dat` que el tamaño del archivo no cambió y que sólo se modificaron los bytes del registro correspondiente.

---

## Funciones sugeridas

Los nombres son orientativos. Se pueden modificar si se mantiene una organización clara.

```c
void imprimir_persona(const PERSONA *p);
int cargar_desde_archivo(const char *nombre_archivo,
                         PERSONA ***personas,
                         size_t *cantidad);
int guardar_en_archivo(const char *nombre_archivo,
                       PERSONA *const *personas,
                       size_t cantidad);
int agregar_ordenada(PERSONA ***personas, size_t *cantidad,
                     const PERSONA *nueva);
int buscar_por_nombre(PERSONA *const *personas, size_t cantidad,
                      const char *nombre);
PERSONA *buscar_por_id_memoria(PERSONA *const *personas,
                               size_t cantidad, int id);
int buscar_por_id_archivo(const char *nombre_archivo, int id,
                          PERSONA *resultado);
int modificar_por_id(const char *nombre_archivo, int id);
void liberar_personas(PERSONA **personas, size_t cantidad);
```

---

## Pruebas mínimas

Antes de entregar, comprobar al menos los siguientes casos:

1. Crear una base vacía y cargar varias personas.
2. Cerrar y volver a ejecutar el programa; verificar que los datos se recuperan desde `personas.dat`.
3. Intentar cargar un ID repetido.
4. Buscar un ID existente y uno inexistente.
5. Buscar por nombre con una coincidencia y con ninguna coincidencia.
6. Insertar personas en un orden de ID no ordenado y comprobar que la base queda ordenada.
7. Modificar una persona y verificar que su ID no cambia.
8. Comparar el tamaño informado por el programa con `ls -l personas.dat`.
9. Inspeccionar el archivo con:

   ```bash
   hexdump -C personas.dat
   ```

---

## Entrega

Entregar:

- el o los archivos fuente `.c`;
- una breve explicación en un archivo `README.md` que indique cómo compilar y ejecutar el programa;
- una captura o transcripción de una ejecución que demuestre carga, búsqueda, modificación y persistencia de datos.
