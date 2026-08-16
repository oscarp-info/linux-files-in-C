# Guías de archivos en C

**Informática 1 · UTN Electrónica**  
**Lenguaje:** C estándar; la primera guía utiliza además la interfaz POSIX de Linux.

Este material está organizado en el orden sugerido de cursada. Cada módulo contiene su guía y una carpeta `src` con los programas y archivos de datos necesarios para ejecutarla.

## Recorrido sugerido

| Orden | Módulo | Contenido principal |
|---|---|---|
| 1 | [Bajo nivel con POSIX](01_bajo_nivel_posix/01_guia_archivos_bajo_nivel.md) | descriptores, `open`, `read`, `write`, `lseek`, permisos y archivos de registros. [Presentación](01_bajo_nivel_posix/01_archivos_bajo_nivel_posix.pptx) |
| 2 | [Alto nivel: bloques y registros](02_alto_nivel_bloques/02_guia_archivos_alto_nivel_bloques.md) | `FILE *`, `fopen`, `fread`, `fwrite`, `fseek` y `ftell` |
| 3 | [Alto nivel: archivos de texto](03_alto_nivel_texto/03_guia_archivos_alto_nivel_texto.md) | caracteres, líneas, formatos, `strtok` y conversiones validadas |
| 4 | [CSV integrador](04_csv_integrador/04_guia_archivos_csv.md) | lectura de CSV simple, estructuras, arreglos y memoria dinámica |

## Organización de cada módulo

Todos los módulos siguen la misma lógica:

- La guía numerada es el material principal que se entrega o se consulta primero.
- La carpeta `src/` contiene los programas de esa guía, ordenados con el mismo número que aparece en el documento.
- Los ejemplos que requieren un archivo de entrada incluyen ese archivo dentro de su propio `src/`. Por eso conviene compilar y ejecutar cada programa desde esa carpeta.
- Los enlaces de cada guía llevan directamente al código correspondiente, para poder alternar entre la explicación y el ejemplo.

El módulo 1 suma, además, una [presentación de bajo nivel con POSIX](01_bajo_nivel_posix/01_archivos_bajo_nivel_posix.pptx) y [una actividad integradora de base de datos de personas](01_bajo_nivel_posix/actividad_base_datos_personas.md). La actividad es una propuesta de práctica posterior a los ejemplos de bajo nivel.

## Árbol resumido

```text
guias_archivos_c/
├── README.md
├── 01_bajo_nivel_posix/
│   ├── 01_guia_archivos_bajo_nivel.md
│   ├── 01_archivos_bajo_nivel_posix.pptx
│   ├── actividad_base_datos_personas.md
│   └── src/
├── 02_alto_nivel_bloques/
│   ├── 02_guia_archivos_alto_nivel_bloques.md
│   └── src/
├── 03_alto_nivel_texto/
│   ├── 03_guia_archivos_alto_nivel_texto.md
│   └── src/
└── 04_csv_integrador/
    ├── 04_guia_archivos_csv.md
    └── src/
```