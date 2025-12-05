# Tarea 2: Sistemas Operativos (2025)

Este repositorio contiene la implementación de la Tarea 2, dividida en dos partes: Sincronización de hebras y Simulador de Memoria Virtual.

## 📋 Archivos
* `Tarea2_parte1.cpp`: Código de la Barrera Reutilizable.
* `Tarea2_parte2.cpp`: Código del Simulador de Memoria Virtual.
* `trace1.txt`: Traza de memoria (tamaño de página: 8 bytes).
* `trace2.txt`: Traza de memoria (tamaño de página: 4096 bytes).

## ⚙️ Compilación y Ejecución

Abre una terminal en la carpeta donde están los archivos y sigue estas instrucciones.

### Parte I: Barrera Reutilizable

**1. Compilar:**
Es necesario enlazar la librería de hilos (`pthread`).
```bash
g++ -o barrera Tarea2_parte1.cpp -pthread
```
2. Ejecutar: Usa los flags -n (número de hebras) y -e (número de etapas).

```bash
./barrera -n 5 -e 4
```

### Parte II: Simulador de Memoria Virtual
1. Compilar:

```bash
g++ -o sim Tarea2_parte2.cpp
```
2. Ejecutar: El formato es: ./sim <num_marcos> <tam_marco> <archivo_traza>

Para trace1.txt (Nota: usar tamaño de marco 8):

```bash
./sim 16 8 trace1.txt
```
Para trace2.txt (Nota: usar tamaño de marco 4096):

```bash

./sim 32 4096 trace2.txt
```


👥 Integrante
Cristobal Fuentealba    2019435086
