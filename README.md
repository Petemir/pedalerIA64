pedalerIA64
===========

Efectos
--------------

* Delay simple:
  * ~~C: hecho.~~
  * ~~ASM: hecho.~~
  * ~~Diferencias de audio entre C y ASM en STEREO: hecho.~~
  * ~~Generar varios ejemplos con argumentos variados y ponerlos en una carpeta para cerrar el efecto: hecho.~~


* Delay multiple:
  * No hecho


* Flanger:
  * ~~C: hecho.~~
  * ~~ASM: hecho.~~
  * ~~Corroborar si hay diferencias: hecho.~~
  * ~~Probar con varios parametros: hecho.~~
  * Rehacer los audios.
  * Chequear qué pasa con ./main inputExamples/gibson.wav output-c.wav -f 0.01 3.98 0.69, entre assembler y C


* Vibrato:
  * ~~Hecho en C.~~
  * ~~ASM: falta.~~
  * ~~Ver diferencias.~~
  * ~~Agregar tiempo restante al buffer.~~ No hace falta.
  * Ejemplos con varios parametros


* Bitcrusher:
  * ~~C: hecho.~~
  * ~~ASM: hecho.~~
  * Corroborar diferencias.
  * Ejemplos con varios parametros.

* Overdrive:
  * https://github.com/nashj/Algorithms/blob/master/DigitalMusic/Overdrive.m


* WahWah:
  * ~~C: hecho.~~
  * ~~Mejorar codigo hecho en C, no está igual a matlab.~~
  * ~~ASM: no hecho.~~
  * Corroborar diferencias.
  * Ejemplos con varios parametros.


* Normalize:
 * ~~C: modularizar.~~
 * ~~ASM: no hecho.~~
 * ~~Los ejemplos no hacen falta.~~
 * ~~Falta codigo para ultimos dos frames~~
 * ~~Pensar si conviene llamarla siempre que se termina de usar algún efecto.~~
 * ~~Falta hacer que sume a cantCiclos (ultimo item ToDo)~~

* MaxSamp:
 * ~~C: hecho.~~
 * ~~ASM: hecho.~~
 * ~~Falta codigo para ultimos dos frames~~
 * ~~Falta hacer que sume a cantCiclos (ultimo item ToDo)~~

* CleanBuffer
 * ~~C: hecho.~~
 * ASM: no hecho.

* Copy:
 * ~~C: hecho.~~
 * ~~ASM: hecho.~~
 * Comparar que den lo mismo.

ToDo
--------------

* Entrada del programa:
  * Mejor manejo de parámetros, para que se pueda usar más de un efecto - que en la segunda pasada abra el archivo de salida anterior, no el de entrada.
  * Chequear valores que puede tomar cada parámetro según el efecto.
  * Que el audio de salida dure más (input + delay)

* Evaluación de performance:
  * Tomar cantidad de iteraciones como parámetro y correrlo esa cantidad de veces
